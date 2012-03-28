#include "StdAfx.h"
#include "DesktopSourceAudioPin.h"
#include <mmdeviceapi.h>
#include "DesktopSourceFilter.h"

#define REFTIMES_PER_SEC 10000000

DesktopSourceAudioPin::DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, pms, pName), m_audioClient(NULL), m_captureClient(NULL), m_blockAlign(0), 
	  m_iFrameNumber(0), m_nextTick(0), m_rtFrameLength(REFTIMES_PER_SEC), m_lastSync(0)
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

done:

	SAFE_RELEASE(devEnumerator);
	SAFE_RELEASE(audioClient);
	SAFE_RELEASE(device);
}


DesktopSourceAudioPin::~DesktopSourceAudioPin(void)
{
	SAFE_RELEASE(this->m_audioClient);
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
	DWORD sampleDataCount, actualDataCount, flags, nowTick;
	BYTE * sampleData, * inData;

	CAutoLock cAutoLockShared(&m_cSharedState);
    
	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    actualDataCount = sampleDataCount = pSample->GetSize();
	
	nowTick = GetTickCount();
	while (nowTick < this->m_nextTick)
    {
		//Ak sa este neprekrocil cast stanovany na jeden zapis do buffera, zaznamenavaj do vystupneho buffera
		//4-krat za stanoveny cas skontroluje vstupny buffer
		Sleep((DWORD)(this->m_rtFrameLength / (10000 * 4)));

        CHECK_SUCCEED(hr = this->m_captureClient->GetNextPacketSize(&packetLength));
        while (packetLength != 0)
        {
			//Ak je nieco v buffery / zaznamenavaj do vystupneho buffera
            CHECK_SUCCEED(hr = this->m_captureClient->GetBuffer(&inData, &numFramesAvailable, &flags, NULL, NULL));

			//Zisti pocet bajtov pre vsetky frame a ziskaj hodnotu, kolko je mozne zapisat do buffera
			UINT32 bytesAvailable = numFramesAvailable * this->m_blockAlign;
			bytesAvailable = min(bytesAvailable, actualDataCount);

			//Zapis do vystupneho buffera - samplu
			for (UINT32 i = 0; i < bytesAvailable; i++)
				sampleData[i] = inData[i];

			//Aktualizuj dostupnu velkost buffera a posun smernik na prazdne miesto
			sampleData = &sampleData[bytesAvailable];
			actualDataCount -= bytesAvailable;

            CHECK_SUCCEED(hr = this->m_captureClient->ReleaseBuffer(numFramesAvailable));
            CHECK_SUCCEED(hr = this->m_captureClient->GetNextPacketSize(&packetLength));
        }

		//Ak sa uz nezmesti do buffera ani jeden frame - koniec
		if (actualDataCount < this->m_blockAlign)
			break;

		nowTick = GetTickCount();
    }

	//Vyaze buffer, ak sa nic nezapisalo ostane ticho, teda 0
	ZeroMemory(sampleData, actualDataCount);

	//Synchronizacia audio / video
	nowTick = GetTickCount();
	if (nowTick > (this->m_lastSync + DesktopSourceFilter::SyncTime))
	{
		((DesktopSourceFilter*)this->m_pFilter)->SyncPins(1);
		this->m_lastSync = this->m_nextTick = nowTick = GetTickCount();

		//Vycistenie vstupneho buffera ak sa v nom nieco nachadza
		CHECK_SUCCEED(hr = this->m_captureClient->GetNextPacketSize(&packetLength));
        while (packetLength != 0)
        {
			CHECK_SUCCEED(hr = this->m_captureClient->GetBuffer(&inData, &numFramesAvailable, &flags, NULL, NULL));
			CHECK_SUCCEED(hr = this->m_captureClient->ReleaseBuffer(numFramesAvailable));
            CHECK_SUCCEED(hr = this->m_captureClient->GetNextPacketSize(&packetLength));
		}
	}
	this->m_nextTick = (DWORD)(this->m_nextTick + (this->m_rtFrameLength / 10000));

	REFERENCE_TIME rtStart = this->m_iFrameNumber * this->m_rtFrameLength;
    REFERENCE_TIME rtStop  = rtStart + this->m_rtFrameLength;
	this->m_iFrameNumber++;

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

	return CSourceStream::OnThreadDestroy();
}
