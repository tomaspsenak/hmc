#include "StdAfx.h"
#include "StaticSourcePins.h"
#include "StaticSourceFilter.h"
#include "ColorConverter.h"

#define REFTIMES_PER_SEC 10000000

//********************************************
//*********** StaticSourceAudioPin ***********
//********************************************

StaticSourceAudioPin::StaticSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, StaticSourceFilter * filter, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, filter, pName), m_pFilter(filter), m_rtLastFrame(0), m_isStopping(FALSE)
{
}

StaticSourceAudioPin::~StaticSourceAudioPin(void)
{
}

HRESULT StaticSourceAudioPin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
	if(iPosition < 0)
        return E_INVALIDARG;

	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	CAutoLock cAutoLock(this->m_pLock);

	HRESULT hr = S_OK;
	WAVEFORMATEX * waveFormat = (WAVEFORMATEX*)pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
    if(NULL == waveFormat)
        CHECK_HR(hr = E_OUTOFMEMORY);
    ZeroMemory(waveFormat, sizeof(WAVEFORMATEX));

	pmt->SetSubtype(&MEDIASUBTYPE_PCM);
	pmt->SetType(&MEDIATYPE_Audio);
	pmt->SetFormatType(&FORMAT_WaveFormatEx);
    pmt->SetTemporalCompression(FALSE);
    pmt->SetSampleSize(8);

	waveFormat->wFormatTag = WAVE_FORMAT_PCM;
	waveFormat->nChannels = 2;
	waveFormat->nSamplesPerSec = 44100;
	waveFormat->wBitsPerSample = 16;
	waveFormat->nBlockAlign = waveFormat->nChannels * waveFormat->wBitsPerSample / 8;
	waveFormat->nAvgBytesPerSec = waveFormat->nBlockAlign * waveFormat->nSamplesPerSec;
	waveFormat->cbSize = 0;

done:
	return hr;
}

HRESULT StaticSourceAudioPin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	CAutoLock cAutoLock(this->m_pLock);

	if((pMediaType->majortype != MEDIATYPE_Audio) || (!pMediaType->IsFixedSize()) || (pMediaType->formattype != FORMAT_WaveFormatEx))
		return VFW_E_NO_ACCEPTABLE_TYPES;

	if (pMediaType->subtype != MEDIASUBTYPE_PCM)
		return VFW_E_NO_ACCEPTABLE_TYPES;

	return S_OK;
}

HRESULT StaticSourceAudioPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;
	WAVEFORMATEX * waveFormat = NULL;

	CAutoLock cAutoLock(this->m_pLock);

	waveFormat = reinterpret_cast<WAVEFORMATEX *>(this->m_mt.pbFormat);

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = (long)waveFormat->nAvgBytesPerSec;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(pProperties, &aProp));

    if(aProp.cbBuffer != pProperties->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

HRESULT StaticSourceAudioPin::FillBuffer(IMediaSample * pSample)
{
	HRESULT hr = S_OK;
	DWORD sampleDataCount;
	BYTE * sampleData;

	//Nastavenie hodnoty casu zaciatku a konca sampla
	REFERENCE_TIME rtStart = this->m_rtLastFrame;
    REFERENCE_TIME rtStop  = rtStart + REFTIMES_PER_SEC;
	CHECK_HR(hr = pSample->SetTime(&rtStart, &rtStop));

	if (this->m_pFilter->m_rtStop > 0 && rtStop >= this->m_pFilter->m_rtStop)
	{
		//Ak je nastaveny cas konca a prekroci sa, ukonci sa stream
		hr = S_FALSE;
		goto done;
	}
	this->m_rtLastFrame = rtStop;

	//Ak zvuk predbieha video - pockaj, kontroluj ci sa pin nezastavuje
	for (REFERENCE_TIME vTime = this->m_pFilter->m_sourceVideoPin->GetLastFrameTime(); 
		!this->m_isStopping && vTime > 0 && (vTime + StaticSourceFilter::SyncThreshold) < this->m_rtLastFrame;
		vTime = this->m_pFilter->m_sourceVideoPin->GetLastFrameTime())
	{
		Sleep(50);
	}
    
	//Ziskanie buffera a jeho velkosti
	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    sampleDataCount = pSample->GetSize();

	//Ak je nastavena bitmapa, pouzi ticho, inak nastav sum
	if (this->m_pFilter->m_params->m_bitmapData == NULL)
		for (DWORD i = 0; i < sampleDataCount; i++)
			sampleData[i] = (BYTE)(rand() % 32);
	else
		ZeroMemory(sampleData, sampleDataCount);

	CHECK_HR(hr = pSample->SetSyncPoint(TRUE));
	CHECK_HR(hr = pSample->SetActualDataLength(sampleDataCount));

done:
	return hr;
}

//********************************************
//*********** StaticSourceVideoPin ***********
//********************************************

StaticSourceVideoPin::StaticSourceVideoPin(TCHAR * pObjectName, HRESULT * phr, StaticSourceFilter * filter, LPCWSTR pName)
	: CSourceStream(pObjectName, phr, filter, pName), m_pFilter(filter), m_rtLastFrame(0)
{
}

StaticSourceVideoPin::~StaticSourceVideoPin(void)
{
}

HRESULT StaticSourceVideoPin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
	if(iPosition < 0)
        return E_INVALIDARG;

	CAutoLock cAutoLock(this->m_pLock);

	HRESULT hr = S_OK;
	VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
    if(NULL == pvi)
        CHECK_HR(hr = E_OUTOFMEMORY);
    ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));

	if (iPosition == 0)
		pmt->SetSubtype(&MEDIASUBTYPE_RGB32);
	else if (iPosition == 1)
		pmt->SetSubtype(&MEDIASUBTYPE_RGB24);
	else if (iPosition == 2)
		pmt->SetSubtype(&MEDIASUBTYPE_YUY2);
	else if (iPosition == 3)
		pmt->SetSubtype(&MEDIASUBTYPE_YV12);
	else
		CHECK_HR(hr = VFW_S_NO_MORE_ITEMS);

	pmt->SetType(&MEDIATYPE_Video);
	pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);

	CHECK_HR(hr = ApplyParametersToMT(pmt));

done:
	return hr;
}

HRESULT StaticSourceVideoPin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	HRESULT hr = S_OK;

	CAutoLock cAutoLock(this->m_pLock);

	if((pMediaType->majortype != MEDIATYPE_Video) || (!pMediaType->IsFixedSize()) || (pMediaType->formattype != FORMAT_VideoInfo))
		CHECK_HR(hr = VFW_E_NO_ACCEPTABLE_TYPES);

	if (pMediaType->subtype != MEDIASUBTYPE_RGB24 && pMediaType->subtype != MEDIASUBTYPE_RGB32 && pMediaType->subtype != MEDIASUBTYPE_YUY2 && 
		pMediaType->subtype != MEDIASUBTYPE_YV12)
		CHECK_HR(hr = VFW_E_NO_ACCEPTABLE_TYPES);

	VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *)pMediaType->pbFormat;

	//Neakceptovat otoceny obraz - napriklad pre EVR
	if (pvi->bmiHeader.biHeight < 0)
		CHECK_HR(hr = E_INVALIDARG);

done:
    return hr;
}

HRESULT StaticSourceVideoPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;

    CAutoLock cAutoLock(this->m_pLock);

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = this->m_mt.lSampleSize;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(pProperties, &aProp));

    if(aProp.cbBuffer != pProperties->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

HRESULT StaticSourceVideoPin::FillBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	DWORD frameDataCount;
	BYTE * frameData;

	//Nastavenie hodnoty casu zaciatku a konca snimky
	REFERENCE_TIME rtStart = this->m_rtLastFrame;
	REFERENCE_TIME rtStop  = rtStart + this->m_pFilter->m_params->m_rtFrameLength;
	pSample->SetTime(&rtStart, &rtStop);

	if (this->m_pFilter->m_rtStop > 0 && rtStop >= this->m_pFilter->m_rtStop)
	{
		//Ak je nastaveny cas konca a prekroci sa, ukonci sa stream
		hr = S_FALSE;
		goto done;
	}
	this->m_rtLastFrame = rtStop;

    CHECK_HR(hr = pSample->GetPointer(&frameData));
	frameDataCount = pSample->GetSize();

	//Ak je nastavena bitmapa, pouzi tu, inak nastav sum
	if (this->m_pFilter->m_params->m_bitmapData == NULL)
	{
		for (DWORD i = 0; i < frameDataCount; i++)
			frameData[i] = (BYTE)(rand() % 256);
	}
	else
	{
		if (this->m_mt.subtype == MEDIASUBTYPE_RGB32)
		{
			//Na vystup ide RGB32 typ
			if (this->m_pFilter->m_params->m_bitmapInfo.biBitCount == 32)
			{
				CopyMemory(frameData, this->m_pFilter->m_params->m_bitmapData, frameDataCount);
			}
			else
			{
				BITMAPINFOHEADER dstBmi = this->m_pFilter->m_params->m_bitmapInfo;
				dstBmi.biBitCount = 32;

				RGBtoRGB(this->m_pFilter->m_params->m_bitmapInfo, this->m_pFilter->m_params->m_bitmapData, frameData, dstBmi);
			}
		}
		else if (this->m_mt.subtype == MEDIASUBTYPE_RGB24)
		{
			//Na vystup ide RGB24 typ
			if (this->m_pFilter->m_params->m_bitmapInfo.biBitCount == 24)
			{
				CopyMemory(frameData, this->m_pFilter->m_params->m_bitmapData, frameDataCount);
			}
			else
			{
				BITMAPINFOHEADER dstBmi = this->m_pFilter->m_params->m_bitmapInfo;
				dstBmi.biBitCount = 24;

				RGBtoRGB(this->m_pFilter->m_params->m_bitmapInfo, this->m_pFilter->m_params->m_bitmapData, frameData, dstBmi);
			}
		}
		else if (this->m_mt.subtype == MEDIASUBTYPE_YUY2)
		{
			//Na vystup ide YUY2 typ
			RGBtoYUY2(this->m_pFilter->m_params->m_bitmapInfo, this->m_pFilter->m_params->m_bitmapData, frameData);
		}
		else if (this->m_mt.subtype == MEDIASUBTYPE_YV12)
		{
			//Na vystup ide YV12 typ
			RGBtoYV12(this->m_pFilter->m_params->m_bitmapInfo, this->m_pFilter->m_params->m_bitmapData, frameData);
		}
	}

    CHECK_HR(hr = pSample->SetSyncPoint(TRUE));
	CHECK_HR(hr = pSample->SetActualDataLength(frameDataCount));
	
done:
    return hr;
}

HRESULT StaticSourceVideoPin::ReconnectWithChangesSync(void)
{
	HRESULT hr = S_OK;
	CMediaType mediaType;

	CHECK_HR(hr = mediaType.Set(this->m_mt));
	CHECK_HR(hr = ApplyParametersToMT(&mediaType));

	CHECK_HR(hr = this->m_pFilter->ReconnectPinSync(this, &mediaType));

done:
	return hr;
}

HRESULT StaticSourceVideoPin::ApplyParametersToMT(CMediaType * pmt)
{
	VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *)pmt->pbFormat;

	LONG width = this->m_pFilter->m_params->m_bitmapInfo.biWidth;
	LONG height = this->m_pFilter->m_params->m_bitmapInfo.biHeight;
	RECT rect = { 0, 0, width, height };

	pvi->rcTarget = rect;
	pvi->rcSource = rect;
	pvi->bmiHeader.biWidth = width;
	pvi->bmiHeader.biHeight = height;
	pvi->bmiHeader.biClrImportant = 0;
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->AvgTimePerFrame = this->m_pFilter->m_params->m_rtFrameLength;

	if (pmt->subtype == MEDIASUBTYPE_RGB32)
	{
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 32;
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	}
	else if (pmt->subtype == MEDIASUBTYPE_RGB24)
	{
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 24;
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	}
	else if (pmt->subtype == MEDIASUBTYPE_YUY2)
	{
		pvi->bmiHeader.biCompression = MAKEFOURCC('Y','U','Y','2');
		pvi->bmiHeader.biBitCount = 16;
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biSizeImage = 2 * ((width * height) >> 1) + (width * height);
	}
	else if (pmt->subtype == MEDIASUBTYPE_YV12)
	{
		pvi->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
		pvi->bmiHeader.biBitCount = 12;
		pvi->bmiHeader.biPlanes = 3;
		pvi->bmiHeader.biSizeImage = 2 * ((width * height) >> 2) + (width * height);
	}
	else
	{
		return E_FAIL;
	}

	pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

	return S_OK;
}