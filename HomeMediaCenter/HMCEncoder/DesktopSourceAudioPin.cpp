#include "StdAfx.h"
#include "DesktopSourceAudioPin.h"
#include "DesktopSourceFilter.h"
#include <avrt.h>

#define REFTIMES_PER_SEC 10000000

DesktopSourceAudioPin::DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, DesktopSourceFilter * filter, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, filter, pName), m_device(NULL), m_audioClient(NULL), m_captureClient(NULL), m_blockAlign(0), 
	  m_rtLastFrame(0), m_rtFrameLength(REFTIMES_PER_SEC), m_sleepTime(0), m_cBufferData(0), m_buffer(NULL), m_pFilter(filter),
	  m_silenceStartedEvent(NULL), m_silenceStopEvent(NULL), m_silenceThread(NULL), m_silenceHr(S_OK)
{
	HRESULT hr = S_OK;

	IMMDeviceEnumerator * devEnumerator = NULL;
	IAudioClient * audioClient = NULL;
	IMMDevice * device = NULL;
	REFERENCE_TIME devicePeriod;

    CHECK_SUCCEED(hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&devEnumerator));

	//Nastavi vystup z konzoly - systemove zvuky,...
    CHECK_SUCCEED(hr = devEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device));

    CHECK_SUCCEED(hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient));

	CHECK_SUCCEED(hr = audioClient->GetDevicePeriod(&devicePeriod, NULL));
	this->m_sleepTime = (DWORD)(devicePeriod / 10000 / 2);

    this->m_audioClient = audioClient;
	audioClient = NULL;
	this->m_device = device;
	device = NULL;

done:

	SAFE_RELEASE(devEnumerator);
	SAFE_RELEASE(audioClient);
	SAFE_RELEASE(device);
}


DesktopSourceAudioPin::~DesktopSourceAudioPin(void)
{
	SAFE_RELEASE(this->m_device);
	SAFE_RELEASE(this->m_audioClient);
	if (this->m_buffer != NULL)
	{
		delete [] this->m_buffer;
		this->m_buffer = NULL;
	}
}

STDMETHODIMP DesktopSourceAudioPin::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCDesktopSource)
	{
		return GetInterface(this->m_pFilter->m_params, ppv);
	}

	return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT DesktopSourceAudioPin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
	if(iPosition < 0)
        return E_INVALIDARG;

	HRESULT hr = S_OK;
	WAVEFORMATEX * waveFormat = NULL, * waveTemp = NULL;

	CAutoLock cAutoLock(m_pFilter->pStateLock());

	if (this->m_audioClient == NULL || iPosition > 0)
		CHECK_HR(hr = VFW_S_NO_MORE_ITEMS);

	CHECK_SUCCEED(hr = this->m_audioClient->GetMixFormat(&waveFormat));

	waveTemp = (WAVEFORMATEX*)pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
    if(NULL == waveTemp)
        CHECK_HR(hr = E_OUTOFMEMORY);
    ZeroMemory(waveTemp, sizeof(WAVEFORMATEX));

	pmt->SetSubtype(&MEDIASUBTYPE_PCM);
	pmt->SetType(&MEDIATYPE_Audio);
	pmt->SetFormatType(&FORMAT_WaveFormatEx);
    pmt->SetTemporalCompression(FALSE);
    pmt->SetSampleSize(waveFormat->nBlockAlign);

	//Skopiruj do WAVEFORMATEX, waveFormat moze byt aj WAVEFORMATEXTENSIBLE
	waveTemp->wFormatTag = WAVE_FORMAT_PCM; //vzdy bude WAVEFORMATEX, nie WAVEFORMATEXTENSIBLE...
	waveTemp->nChannels = waveFormat->nChannels;
	waveTemp->nSamplesPerSec = waveFormat->nSamplesPerSec;
	waveTemp->wBitsPerSample = 16;
	waveTemp->nBlockAlign = waveTemp->nChannels * waveTemp->wBitsPerSample / 8;
	waveTemp->nAvgBytesPerSec = waveTemp->nBlockAlign * waveTemp->nSamplesPerSec;
	waveTemp->cbSize = 0;

done:

	CoTaskMemFree(waveFormat);

	return hr;
}

HRESULT DesktopSourceAudioPin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	if((*pMediaType->Type() != MEDIATYPE_Audio) || (!pMediaType->IsFixedSize()) || (pMediaType->Subtype() == NULL) || (pMediaType->Format() == NULL))
		return VFW_E_NO_ACCEPTABLE_TYPES; 

	return S_OK;
}

HRESULT DesktopSourceAudioPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;
	WAVEFORMATEX * waveFormat = NULL;

    CAutoLock cAutoLock(m_pFilter->pStateLock());

	if(this->m_audioClient == NULL)
		CHECK_HR(hr = E_FAIL);

	waveFormat = reinterpret_cast<WAVEFORMATEX *>(this->m_mt.pbFormat);

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = (long)(waveFormat->nAvgBytesPerSec / (REFTIMES_PER_SEC / this->m_rtFrameLength));
    CHECK_SUCCEED(hr = pAlloc->SetProperties(pProperties, &aProp));

    if(aProp.cbBuffer < pProperties->cbBuffer)
		CHECK_HR(hr = E_FAIL);

	//Nastavenie zbytkoveho buffera
	if (this->m_buffer != NULL)
		delete [] this->m_buffer;
	this->m_buffer = new BYTE[aProp.cbBuffer];
	if (this->m_buffer == NULL)
		CHECK_HR(hr = E_FAIL);
	this->m_cBufferData = 0;

done:
    return hr;
}

HRESULT DesktopSourceAudioPin::OnThreadCreate(void)
{
	HRESULT hr = S_OK;
	DWORD waitResult;
	HANDLE waitArray[2];

	WAVEFORMATEX * waveFormat = NULL;
	IAudioCaptureClient * captureClient = NULL;

	CAutoLock cAutoLockShared(&m_cSharedState);

	if (this->m_captureClient != NULL)
		CHECK_HR(hr = E_FAIL);

	//Nastavenie argumentov a spustenie vlakna generujuceho ticho - koli kontinualnemu zaznamu cez WASAPI
    this->m_silenceHr = S_OK;
    if ((this->m_silenceStartedEvent = waitArray[0] = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		CHECK_HR(hr = E_FAIL);
    if ((this->m_silenceStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		CHECK_HR(hr = E_FAIL);
    if ((this->m_silenceThread = waitArray[1] = CreateThread(NULL, 0, PlaySilenceFunction, this, 0, NULL)) == NULL)
		CHECK_HR(hr = E_FAIL);

	//Pocka sa, kym sa spusti alebo ukonci vlakno generujuce ticho
    waitResult = WaitForMultipleObjects(ARRAYSIZE(waitArray), waitArray, FALSE, INFINITE);
	if (waitResult != WAIT_OBJECT_0)
		CHECK_HR(hr = E_FAIL);

	//Nastavenie parametrov pri prvom spusteni
	CHECK_SUCCEED(hr = this->m_audioClient->GetMixFormat(&waveFormat));

	waveFormat->wBitsPerSample = 16;
	this->m_blockAlign = waveFormat->nBlockAlign = waveFormat->nChannels * waveFormat->wBitsPerSample / 8;
    waveFormat->nAvgBytesPerSec = waveFormat->nBlockAlign * waveFormat->nSamplesPerSec;

	if (waveFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		waveFormat->wFormatTag = WAVE_FORMAT_PCM;
	}
	else if (waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		PWAVEFORMATEXTENSIBLE waveFormatEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(waveFormat);
		waveFormatEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		waveFormatEx->Samples.wValidBitsPerSample = 16;
	}

	//Inicializacia objektov
	hr = this->m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, this->m_rtFrameLength, 0, waveFormat, NULL);
	if (hr != AUDCLNT_E_ALREADY_INITIALIZED)
		CHECK_SUCCEED(hr);

	CHECK_SUCCEED(hr = this->m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient)); 

	//Synchronizacia casovej peciatky s audio / video
	((DesktopSourceFilter*)this->m_pFilter)->SyncPins(1);

	//Spusti nahravanie
	CHECK_SUCCEED(hr = this->m_audioClient->Start());

	CHECK_HR(hr =  CSourceStream::OnThreadCreate());

	this->m_rtLastFrame = 0;
	this->m_captureClient = captureClient;
	captureClient = NULL;

done:

	CoTaskMemFree(waveFormat);
	SAFE_RELEASE(captureClient);

	return hr;
}

HRESULT DesktopSourceAudioPin::FillBuffer(IMediaSample * pSample)
{
	HRESULT hr = S_OK;

    UINT32 numFramesAvailable, packetLength = 0;
	DWORD sampleDataCount, actualDataCount, flags;
	BYTE * sampleData, * inData;

	CAutoLock cAutoLockShared(&m_cSharedState);
    
	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    actualDataCount = sampleDataCount = pSample->GetSize();

	//Ak sa v zbytkovom bufferi nieco nachadza - zapise sa do sample
	memcpy(sampleData, this->m_buffer, this->m_cBufferData);

	actualDataCount -= this->m_cBufferData;
	sampleData += this->m_cBufferData;
	this->m_cBufferData = 0;
	
	while (true)
    {
		//Skontroluje sa, ci je spustene vlakno generujuce ticho
		CHECK_HR(hr = this->m_silenceHr);

		//Ak je nieco v buffery / zaznamenavaj do vystupneho buffera
        CHECK_SUCCEED(hr = this->m_captureClient->GetBuffer(&inData, &numFramesAvailable, &flags, NULL, NULL));

		if (numFramesAvailable > 0)
		{
			//Zisti pocet bajtov pre vsetky frame a ziskaj hodnotu, kolko je mozne zapisat do buffera
			UINT32 totalBytes = numFramesAvailable * this->m_blockAlign;
			UINT32 bytesAvailable = min(totalBytes, actualDataCount);

			//Zapis do vystupneho buffera - samplu
			memcpy(sampleData, inData, bytesAvailable);

			//Aktualizuj dostupnu velkost buffera a posun smernik na prazdne miesto
			sampleData += bytesAvailable;
			actualDataCount -= bytesAvailable;

			//Ak sa uz nezmesti do buffera ani jeden frame - koniec
			if (actualDataCount < this->m_blockAlign)
			{
				//Ak ostane zvysok, zapis do zbytkoveho buffera
				totalBytes = min(totalBytes, sampleDataCount);
				memcpy(this->m_buffer, inData + bytesAvailable, totalBytes - bytesAvailable);
				this->m_cBufferData = totalBytes - bytesAvailable;

				CHECK_SUCCEED(hr = this->m_captureClient->ReleaseBuffer(numFramesAvailable));

				break;
			}
		}
		else
		{
			Sleep(this->m_sleepTime);
		}

		CHECK_SUCCEED(hr = this->m_captureClient->ReleaseBuffer(numFramesAvailable));
    }

	REFERENCE_TIME rtStart = this->m_rtLastFrame;
    REFERENCE_TIME rtStop  = rtStart + this->m_rtFrameLength;
	this->m_rtLastFrame = rtStop;

    CHECK_HR(hr = pSample->SetTime(&rtStart, &rtStop));
	CHECK_HR(hr = pSample->SetSyncPoint(TRUE));
	CHECK_HR(hr = pSample->SetActualDataLength(sampleDataCount));

done:
	return hr;
}

HRESULT DesktopSourceAudioPin::OnThreadDestroy(void)
{
	CAutoLock cAutoLockShared(&m_cSharedState);

	if (this->m_captureClient != NULL)
	{
		this->m_audioClient->Stop();
		this->m_audioClient->Reset();
		SAFE_RELEASE(this->m_captureClient);
	}

	//Pockanie na ukoncenie vlakna generujuceho ticho
	if (this->m_silenceStopEvent != NULL)
		SetEvent(this->m_silenceStopEvent);
	if (this->m_silenceThread != NULL)
	{
		WaitForSingleObject(this->m_silenceThread, INFINITE);
		CloseHandle(this->m_silenceThread);
	}

	//Uzavrenie premennych typu HANDLE a nastavenie na NULL
	if (this->m_silenceStartedEvent != NULL)
		CloseHandle(this->m_silenceStartedEvent);

	if (this->m_silenceStopEvent != NULL)
		CloseHandle(this->m_silenceStopEvent);

	this->m_silenceThread = NULL;
	this->m_silenceStopEvent = NULL;
	this->m_silenceStartedEvent = NULL;

	return CSourceStream::OnThreadDestroy();
}

//SOURCE: http://blogs.msdn.com/b/matthew_van_eerde/archive/2008/12/10/sample-playing-silence-via-wasapi-event-driven-pull-mode.aspx

DWORD WINAPI DesktopSourceAudioPin::PlaySilenceFunction(LPVOID pContext)
{
    DesktopSourceAudioPin * pArgs = (DesktopSourceAudioPin *)pContext;

	HRESULT hr = S_OK;
	DWORD dwWaitResult, nTaskIndex = 0;
	UINT32 nFramesInBuffer, nFramesOfPadding;
	BYTE * pData;

	WAVEFORMATEX * pwfx = NULL;
	IAudioClient * pAudioClient = NULL;
	IAudioRenderClient * pAudioRenderClient = NULL;
	HANDLE hTask, waitArray[2] = { pArgs->m_silenceStopEvent, NULL };
	waitArray[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (waitArray[1] == NULL)
        CHECK_HR(hr = E_FAIL);

    CHECK_HR(hr = CoInitialize(NULL));

    CHECK_HR(hr = pArgs->m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient));

    CHECK_HR(hr = pAudioClient->GetMixFormat(&pwfx));

    CHECK_HR(hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, pwfx, NULL));
    
    CHECK_HR(hr = pAudioClient->GetBufferSize(&nFramesInBuffer));

    CHECK_HR(hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pAudioRenderClient));

    CHECK_HR(hr = pAudioClient->SetEventHandle(waitArray[1]));
    
    CHECK_HR(hr = pAudioRenderClient->GetBuffer(nFramesInBuffer, &pData));

    CHECK_HR(hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer, AUDCLNT_BUFFERFLAGS_SILENT));

    hTask = AvSetMmThreadCharacteristics(L"Playback", &nTaskIndex);
    if (NULL == hTask)
		CHECK_HR(hr = E_FAIL);

    CHECK_HR(hr = pAudioClient->Start());

    SetEvent(pArgs->m_silenceStartedEvent);

    while (true)
	{
        dwWaitResult = WaitForMultipleObjects(ARRAYSIZE(waitArray), waitArray, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == dwWaitResult)
		{
            break;
        }

        if (WAIT_OBJECT_0 + 1 != dwWaitResult)
		{
            CHECK_HR(hr = E_UNEXPECTED);
        }

        // got "feed me" event - see how much padding there is
        //
        // padding is how much of the buffer is currently in use
        //
        // note in particular that event-driven (pull-mode) render should not
        // call GetCurrentPadding multiple times
        // in a single processing pass
        // this is in stark contrast to timer-driven (push-mode) render
        
        CHECK_HR(hr = pAudioClient->GetCurrentPadding(&nFramesOfPadding));

        if (nFramesOfPadding == nFramesInBuffer)
		{
			continue;
        }
    
        CHECK_HR(hr = pAudioRenderClient->GetBuffer(nFramesInBuffer - nFramesOfPadding, &pData));

        // *** AT THIS POINT ***
        // If you wanted to render something besides silence,
        // you would fill the buffer pData
        // with (nFramesInBuffer - nFramesOfPadding) worth of audio data
        // this should be in the same wave format
        // that the stream was initialized with
        //
        // In particular, if you didn't want to use the mix format,
        // you would need to either ask for a different format in IAudioClient::Initialize
        // or do a format conversion
        //
        // If you do, then change the AUDCLNT_BUFFERFLAGS_SILENT flags value below to 0

        CHECK_HR(hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer - nFramesOfPadding, AUDCLNT_BUFFERFLAGS_SILENT));     
    }

done:

	if (pAudioClient != NULL)
	{
		pAudioClient->Stop();
		SAFE_RELEASE(pAudioClient);
	}
	SAFE_RELEASE(pAudioRenderClient);
    AvRevertMmThreadCharacteristics(hTask);
    CloseHandle(waitArray[1]);
	if (pwfx != NULL)
		CoTaskMemFree(pwfx);

	CoUninitialize();

	pArgs->m_silenceHr = hr;
    return 0;
}