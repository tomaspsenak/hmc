#include "StdAfx.h"
#include "DesktopSourceParameters.h"
#include "DesktopSourceFilter.h"

DesktopSourceParameters::DesktopSourceParameters(DesktopSourceFilter * filter) : CUnknown(L"DesktopSourceParameters", filter->GetOwner()), 
	m_fps(10), m_rtFrameLength(UNITS / 10), m_filter(filter), m_width(0), m_height(0), m_videoQuality(100), m_videoStretchMode(HALFTONE),
	m_captureCursor(TRUE), m_keepAspectRatio(FALSE), m_captureActiveWindow(FALSE)
{
}

DesktopSourceParameters::~DesktopSourceParameters(void)
{
}

HRESULT DesktopSourceParameters::SetFrameRate(UINT32 fps)
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

HRESULT DesktopSourceParameters::GetFrameRate(UINT32 * pFps)
{
	CheckPointer(pFps, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pFps = this->m_fps;

	return S_OK;
}

HRESULT DesktopSourceParameters::SetWidth(UINT32 width)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (this->m_width == width)
		return S_OK;

	if (width % 8 != 0)
		return E_FAIL;

	this->m_width = width;

	if (this->m_filter->m_sourceVideoPin->IsConnected())
		return this->m_filter->m_sourceVideoPin->ReconnectWithChangesSync();

	return S_OK;
}

HRESULT DesktopSourceParameters::GetWidth(UINT32 * pWidth)
{
	CheckPointer(pWidth, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pWidth = this->m_width;

	return S_OK;
}

HRESULT DesktopSourceParameters::SetHeight(UINT32 height)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (this->m_height == height)
		return S_OK;

	if (height % 8 != 0)
		return E_FAIL;

	this->m_height = height;

	if (this->m_filter->m_sourceVideoPin->IsConnected())
		return this->m_filter->m_sourceVideoPin->ReconnectWithChangesSync();

	return S_OK;
}

HRESULT DesktopSourceParameters::GetHeight(UINT32 * pHeight)
{
	CheckPointer(pHeight, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pHeight = this->m_height;

	return S_OK;
}

HRESULT DesktopSourceParameters::SetVideoQuality(BYTE quality)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_videoQuality = quality;
	if (quality < 33)
		this->m_videoStretchMode = BLACKONWHITE;
	else if (quality < 66)
		this->m_videoStretchMode = COLORONCOLOR;
	else
		this->m_videoStretchMode = HALFTONE;

	return S_OK;
}

HRESULT DesktopSourceParameters::GetVideoQuality(BYTE * pQuality)
{
	CheckPointer(pQuality, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pQuality = this->m_videoQuality;

	return S_OK;
}

HRESULT DesktopSourceParameters::SetCaptureCursor(BOOL captureCursor)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);
	
	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_captureCursor = captureCursor;

	return S_OK;
}

HRESULT DesktopSourceParameters::GetCaptureCursor(BOOL * pCaptureCursor)
{
	CheckPointer(pCaptureCursor, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pCaptureCursor = this->m_captureCursor;

	return S_OK;
}

STDMETHODIMP DesktopSourceParameters::SetAspectRatio(BOOL keepAspectRatio)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);
	
	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_keepAspectRatio = keepAspectRatio;

	return S_OK;
}

STDMETHODIMP DesktopSourceParameters::GetAspectRatio(BOOL * pKeepAspectRatio)
{
	CheckPointer(pKeepAspectRatio, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pKeepAspectRatio = this->m_keepAspectRatio;

	return S_OK;
}

STDMETHODIMP DesktopSourceParameters::SetCaptureWindow(BOOL captureActiveWindow)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);
	
	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_captureActiveWindow = captureActiveWindow;

	return S_OK;
}

STDMETHODIMP DesktopSourceParameters::GetCaptureWindow(BOOL * pCaptureActiveWindow)
{
	CheckPointer(pCaptureActiveWindow, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pCaptureActiveWindow = this->m_captureActiveWindow;

	return S_OK;
}