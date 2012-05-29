#include "StdAfx.h"
#include "DesktopSourceFilter.h"

DesktopSourceFilter::DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr, UINT32 fps) 
	: CSource(L"DesktopSourceFilter", pUnk, CLSID_NULL), m_sourcePin(NULL), m_sourceAudioPin(NULL), m_rtLastFrame(0), m_isTimeSet(FALSE)
{
	this->m_sourcePin = new DesktopSourcePin(L"DesktopSourcePin", phr, this, L"Out", fps);
	if (this->m_sourcePin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_sourceAudioPin = new DesktopSourceAudioPin(L"DesktopSourceAudioPin", phr, this, L"Out");
	if (this->m_sourceAudioPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	//Nie je potrebne davat AddRef objektu DesktopSourcePin a DesktopSourceAudioPin, vsetko sa deleguje na DesktopSourceFilter
}

STDMETHODIMP DesktopSourceFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
		return GetInterface((IAMFilterMiscFlags*) this, ppv);

	return CSource::NonDelegatingQueryInterface(riid, ppv);
}

DesktopSourceFilter::~DesktopSourceFilter(void)
{
	if (this->m_sourcePin != NULL)
		delete this->m_sourcePin;
	if (this->m_sourceAudioPin != NULL)
		delete this->m_sourceAudioPin;
}

void DesktopSourceFilter::SyncPins(DWORD index, REFERENCE_TIME & rtSyncTime)
{
	CAutoLock cAutoLock(&this->m_syncSection);

	if (index == 1)
	{
		//Audio pin
		this->m_isTimeSet = TRUE;
		this->m_rtLastFrame = rtSyncTime;
	}
	else if (this->m_isTimeSet)
	{
		//Video pin a ak je nastaveny synchronizacny cas
		this->m_isTimeSet = FALSE;
		rtSyncTime = this->m_rtLastFrame;
	}
}

//************ IAMFilterMiscFlags ************

ULONG STDMETHODCALLTYPE DesktopSourceFilter::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}