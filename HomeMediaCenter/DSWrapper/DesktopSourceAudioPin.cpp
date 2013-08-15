#include "StdAfx.h"
#include "DesktopSourceAudioPin.h"
#include "DesktopSourceFilter.h"

#define REFTIMES_PER_SEC 10000000

DesktopSourceAudioPin::DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, pms, pName), m_device(NULL), m_audioClient(NULL), m_captureClient(NULL), m_blockAlign(0), 
	  m_rtLastFrame(0), m_rtFrameLength(REFTIMES_PER_SEC), m_cBufferData(0), m_buffer(NULL)
{
	HRESULT hr = S_OK;

	IMMDeviceEnumerator * devEnumerator = NULL;
	IAudioClient * audioClient = NULL;
	IMMDevice * device = NULL;

    CHECK_SUCCEED(hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&devEnumerator));

	//Nastavi vystup z konzoly - systemove zvuky,...
    CHECK_SUCCEED(hr = devEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device));

    CHECK_SUCCEED(hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient));

    this->m_audioClient = audioClient;
	audioClient = NULL;
	this->m_silenceArgs.device = this->m_device = device;
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
	waveTemp->cbSize = sizeof(WAVEFORMATEX);

done:

	CoTaskMemFree(waveFormat);

	return hr;
}

HRESULT DesktopSourceAudioPin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	if((*pMediaType->Type() != MEDIATYPE_Audio) || (!pMediaType->IsFixedSize()) || (pMediaType->Subtype() == NULL) || (pMediaType->Format() == NULL))
		return E_INVALIDARG; 

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
    this->m_silenceArgs.hr = S_OK;
    if ((this->m_silenceArgs.startedEvent = waitArray[0] = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		CHECK_HR(hr = E_FAIL);
    if ((this->m_silenceArgs.stopEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		CHECK_HR(hr = E_FAIL);
    if ((this->m_silenceArgs.thread = waitArray[1] = CreateThread(NULL, 0, PlaySilenceFunction, &this->m_silenceArgs, 0, NULL)) == NULL)
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
	CHECK_SUCCEED(hr = this->m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, this->m_rtFrameLength, 0, waveFormat, NULL));
	CHECK_SUCCEED(hr = this->m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient)); 

	//Synchronizacia casovej peciatky s audio / video
	((DesktopSourceFilter*)this->m_pFilter)->SyncPins(1);

	//Spusti nahravanie
	CHECK_SUCCEED(hr = this->m_audioClient->Start());

	CHECK_HR(hr =  CSourceStream::OnThreadCreate());

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
	for (UINT32 i = 0; i < this->m_cBufferData; i++)
		sampleData[i] = this->m_buffer[i];

	actualDataCount -= this->m_cBufferData;
	sampleData = &sampleData[this->m_cBufferData];
	this->m_cBufferData = 0;
	
	while (true)
    {
		//Skontroluje sa, ci je spustene vlakno generujuce ticho
		CHECK_HR(hr = this->m_silenceArgs.hr);

		//Ak je nieco v buffery / zaznamenavaj do vystupneho buffera
        CHECK_SUCCEED(hr = this->m_captureClient->GetBuffer(&inData, &numFramesAvailable, &flags, NULL, NULL));

		//Zisti pocet bajtov pre vsetky frame a ziskaj hodnotu, kolko je mozne zapisat do buffera
		UINT32 totalBytes = numFramesAvailable * this->m_blockAlign;
		UINT32 bytesAvailable = min(totalBytes, actualDataCount);

		//Zapis do vystupneho buffera - samplu
		for (UINT32 i = 0; i < bytesAvailable; i++)
			sampleData[i] = inData[i];

		//Aktualizuj dostupnu velkost buffera a posun smernik na prazdne miesto
		sampleData = &sampleData[bytesAvailable];
		actualDataCount -= bytesAvailable;

		//Ak sa uz nezmesti do buffera ani jeden frame - koniec
		if (actualDataCount < this->m_blockAlign)
		{
			//Ak ostane zvysok, zapis do zbytkoveho buffera
			totalBytes = min(totalBytes, sampleDataCount);
			for (UINT32 i = bytesAvailable; i < totalBytes; i++)
				this->m_buffer[i - bytesAvailable] = inData[i];
			this->m_cBufferData = totalBytes - bytesAvailable;

			CHECK_SUCCEED(hr = this->m_captureClient->ReleaseBuffer(numFramesAvailable));

			break;
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
		SAFE_RELEASE(this->m_captureClient);
	}

	//Pockanie na ukoncenie vlakna generujuceho ticho
	if (this->m_silenceArgs.stopEvent != NULL)
		SetEvent(this->m_silenceArgs.stopEvent);
	if (this->m_silenceArgs.thread != NULL)
	{
		WaitForSingleObject(this->m_silenceArgs.thread, INFINITE);
		CloseHandle(this->m_silenceArgs.thread);
	}

	//Uzavrenie premennych typu HANDLE a nastavenie na NULL
	if (this->m_silenceArgs.startedEvent != NULL)
		CloseHandle(this->m_silenceArgs.startedEvent);

	if (this->m_silenceArgs.stopEvent != NULL)
		CloseHandle(this->m_silenceArgs.stopEvent);

	this->m_silenceArgs.thread = NULL;
	this->m_silenceArgs.stopEvent = NULL;
	this->m_silenceArgs.startedEvent = NULL;

	return CSourceStream::OnThreadDestroy();
}
