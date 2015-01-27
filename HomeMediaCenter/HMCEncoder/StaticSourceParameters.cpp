#include "StdAfx.h"
#include "StaticSourceParameters.h"
#include "StaticSourceFilter.h"

StaticSourceParameters::StaticSourceParameters(StaticSourceFilter * filter) : CUnknown(L"StaticSourceParameters", filter->GetOwner()), 
	m_fps(10), m_rtFrameLength(UNITS / 10), m_filter(filter), m_bitmapData(NULL)
{
	ZeroMemory(&this->m_bitmapInfo, sizeof(BITMAPINFOHEADER));
	this->m_bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
	this->m_bitmapInfo.biWidth = 640;
	this->m_bitmapInfo.biHeight = 480;
	this->m_bitmapInfo.biPlanes = 1;
	this->m_bitmapInfo.biBitCount = 24;
}

StaticSourceParameters::~StaticSourceParameters(void)
{
	if (this->m_bitmapData != NULL)
		free(this->m_bitmapData);
}

HRESULT StaticSourceParameters::SetFrameRate(UINT32 fps)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (this->m_fps == fps)
		return S_OK;

	if (fps < 1)
		return E_FAIL;

	this->m_fps = fps;
	this->m_rtFrameLength = UNITS / fps;

	if (this->m_filter->m_sourceVideoPin->IsConnected())
		return this->m_filter->m_sourceVideoPin->ReconnectWithChangesSync();

	return S_OK;
}

HRESULT StaticSourceParameters::GetFrameRate(UINT32 * pFps)
{
	CheckPointer(pFps, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pFps = this->m_fps;

	return S_OK;
}

HRESULT StaticSourceParameters::SetBitmapData(BYTE * pData, UINT32 dataCount)
{
	CheckPointer(pData, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (dataCount < sizeof(BITMAPFILEHEADER))
		return E_FAIL;

	BITMAPFILEHEADER * bmf = (BITMAPFILEHEADER *)pData;
	if (bmf->bfType != 'MB' || bmf->bfSize < dataCount)
		return E_FAIL;

	BITMAPINFOHEADER * bmi = (BITMAPINFOHEADER *)(pData + sizeof(BITMAPFILEHEADER));
	//Skontroloval biSize, nemusi sa potom doplnat v StaticSourceVideoPin::GetMediaType
	if (bmi->biSize != sizeof(BITMAPINFOHEADER))
		return E_FAIL;

	//Podporuje sa iba 24 a 32 bitova hlbka
	if (bmi->biBitCount != 24 && bmi->biBitCount != 32)
		return E_FAIL;

	CopyMemory(&this->m_bitmapInfo, bmi, sizeof(BITMAPINFOHEADER));

	DWORD dataLength = bmf->bfSize - bmf->bfOffBits;

	if (this->m_bitmapData != NULL)
		free(this->m_bitmapData);
	this->m_bitmapData = (BYTE *)malloc(dataLength);
	if (this->m_bitmapData == NULL)
		return E_OUTOFMEMORY;

	CopyMemory(this->m_bitmapData, pData + bmf->bfOffBits, dataLength);

	if (this->m_filter->m_sourceVideoPin->IsConnected())
		return this->m_filter->m_sourceVideoPin->ReconnectWithChangesSync();

	return S_OK;
}