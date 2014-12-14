#include "StdAfx.h"
#include "DesktopSourceAudioPin.h"
#include "DesktopSourceFilter.h"
#include "DesktopSourceSilenceGenerator.h"

#define REFTIMES_PER_SEC 10000000

DesktopSourceAudioPin::DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, DesktopSourceFilter * filter, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, filter, pName), m_device(NULL), m_audioClient(NULL), m_captureClient(NULL), m_blockAlign(0), 
	  m_rtLastFrame(0), m_rtFrameLength(REFTIMES_PER_SEC), m_sleepTime(0), m_cBufferData(0), m_buffer(NULL), m_pFilter(filter)
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

	WAVEFORMATEX * waveFormat = NULL;
	IAudioCaptureClient * captureClient = NULL;

	CAutoLock cAutoLockShared(&m_cSharedState);

	if (this->m_captureClient != NULL)
		CHECK_HR(hr = E_FAIL);

	CHECK_HR(hr = DesktopSourceSilenceGenerator::Get()->Start(this->m_device));

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
		CHECK_HR(hr = DesktopSourceSilenceGenerator::Get()->GeneratorHR());

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

	DesktopSourceSilenceGenerator::Get()->Stop();

	return CSourceStream::OnThreadDestroy();
}