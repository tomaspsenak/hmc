#include "StdAfx.h"
#include "FrameRateParameters.h"
#include "FrameRateFilter.h"

FrameRateParameters::FrameRateParameters(FrameRateFilter * filter) : CUnknown(L"FrameRateParameters", filter->GetOwner()), 
	m_fps(25), m_fpsMod(UNITS % 25), m_rtFrameLength(UNITS / 25), m_filter(filter)
{
}

FrameRateParameters::~FrameRateParameters(void)
{
}

HRESULT FrameRateParameters::SetFrameRate(UINT32 fps)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (this->m_fps == fps)
		return S_OK;

	if (fps < 1)
		return E_FAIL;

	this->m_fps = fps;
	this->m_fpsMod = UNITS % fps;
	this->m_rtFrameLength = UNITS / fps;

	if (this->m_filter->OutputPin()->IsConnected())
		return this->m_filter->ReconnectPinSync(this->m_filter->OutputPin(), this->m_filter->OutputPin(), this->m_rtFrameLength);

	return S_OK;
}

HRESULT FrameRateParameters::GetFrameRate(UINT32 * pFps)
{
	CheckPointer(pFps, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pFps = this->m_fps;

	return S_OK;
}
