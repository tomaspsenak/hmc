#include "StdAfx.h"
#include "DesktopSourceFilter.h"

DesktopSourceFilter::DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr) 
	: CSource(L"HMCDesktopSourceFilter", pUnk, CLSID_HMCDesktopSource), m_sourceVideoPin(NULL), m_sourceAudioPin(NULL), m_params(NULL),
	m_rtStop(0)
{
	this->m_signaled[0] = FALSE;
	this->m_signaled[1] = FALSE;

	this->m_syncEvent[0] = CreateEvent(NULL, TRUE, FALSE /*nesignalovy stav*/, NULL); 
	this->m_syncEvent[1] = CreateEvent(NULL, TRUE, FALSE /*nesignalovy stav*/, NULL); 
	if (this->m_syncEvent[0] == NULL || this->m_syncEvent[1] == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	this->m_sourceVideoPin = new DesktopSourceVideoPin(L"DesktopSourceVideoPin", phr, this, L"Video");
	if (this->m_sourceVideoPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_sourceAudioPin = new DesktopSourceAudioPin(L"DesktopSourceAudioPin", phr, this, L"Audio");
	if (this->m_sourceAudioPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_params = new DesktopSourceParameters(this);
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	//Nie je potrebne davat AddRef objektu DesktopSourcePin a DesktopSourceAudioPin, vsetko sa deleguje na DesktopSourceFilter
}

DesktopSourceFilter::~DesktopSourceFilter(void)
{
	if (this->m_sourceVideoPin != NULL)
		delete this->m_sourceVideoPin;
	if (this->m_sourceAudioPin != NULL)
		delete this->m_sourceAudioPin;
	if (this->m_params != NULL)
		delete this->m_params;

	CloseHandle(this->m_syncEvent[0]);
	CloseHandle(this->m_syncEvent[1]);
}

STDMETHODIMP DesktopSourceFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
	{
		return GetInterface((IAMFilterMiscFlags*) this, ppv);
	}
	else if (riid == IID_IHMCDesktopSource)
	{
		return GetInterface(this->m_params, ppv);
	}
	else if (riid == IID_ISpecifyPropertyPages)
	{
		return GetInterface((ISpecifyPropertyPages*) this, ppv);
	}
	else if (riid == IID_IMediaSeeking)
	{
		return GetInterface((IMediaSeeking*) this, ppv);
	}

	return CSource::NonDelegatingQueryInterface(riid, ppv);
}

//********** ISpecifyPropertyPages **********\\

STDMETHODIMP DesktopSourceFilter::GetPages(CAUUID * pPages)
{
	CheckPointer(pPages, E_POINTER);

	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL)
	{
		pPages->cElems = 0;
		return E_OUTOFMEMORY;
	}

	pPages->cElems = 1;
	pPages->pElems[0] = CLSID_HMCDesktopSourcePage;

	return S_OK;
}

//************ IAMFilterMiscFlags **********\\

ULONG STDMETHODCALLTYPE DesktopSourceFilter::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

//************* IMediaSeeking **************\\

STDMETHODIMP DesktopSourceFilter::GetCapabilities(DWORD * pCapabilities)
{
	CheckPointer(pCapabilities, E_POINTER);
	*pCapabilities = 0;
	return S_OK;
}

STDMETHODIMP DesktopSourceFilter::CheckCapabilities(DWORD * pCapabilities)
{
	CheckPointer(pCapabilities, E_POINTER);
	if(*pCapabilities == 0)
		return S_OK;
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::IsFormatSupported(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	return (*pFormat == TIME_FORMAT_MEDIA_TIME) ? S_OK : S_FALSE;
}

STDMETHODIMP DesktopSourceFilter::QueryPreferredFormat(GUID * pFormat)
{
	return GetTimeFormat(pFormat);
}

STDMETHODIMP DesktopSourceFilter::GetTimeFormat(GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}

STDMETHODIMP DesktopSourceFilter::IsUsingTimeFormat(const GUID * pFormat)
{
	return IsFormatSupported(pFormat);
}

STDMETHODIMP DesktopSourceFilter::SetTimeFormat(const GUID * pFormat)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetDuration(LONGLONG * pDuration)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetStopPosition(LONGLONG * pStop)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetCurrentPosition(LONGLONG * pCurrent)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags, LONGLONG * pStop, DWORD dwStopFlags)
{
	if (pCurrent && (dwCurrentFlags & AM_SEEKING_PositioningBitsMask) != AM_SEEKING_NoPositioning)
	{
		if (*pCurrent != 0)
			return E_INVALIDARG;
	}

	if (pStop && (dwStopFlags & AM_SEEKING_PositioningBitsMask) != AM_SEEKING_NoPositioning)
	{
		switch (dwStopFlags & AM_SEEKING_PositioningBitsMask)
		{
			case AM_SEEKING_AbsolutePositioning: this->m_rtStop = *pStop; break;
			case AM_SEEKING_RelativePositioning: this->m_rtStop += *pStop; break;
			case AM_SEEKING_IncrementalPositioning: return E_INVALIDARG;
		}
	}

	return S_OK;
}

STDMETHODIMP DesktopSourceFilter::GetPositions(LONGLONG * pCurrent, LONGLONG * pStop)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::SetRate(double dRate)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetRate(double * pdRate)
{
	return E_NOTIMPL;
}

STDMETHODIMP DesktopSourceFilter::GetPreroll(LONGLONG* pllPreroll)
{
	return E_NOTIMPL;
}

//********** DesktopSourceFilter ***********\\

void DesktopSourceFilter::SyncPins(DWORD index)
{
	if (this->m_sourceVideoPin->IsConnected() && this->m_sourceAudioPin->IsConnected())
	{
		SetEvent(this->m_syncEvent[index]);

		//Caka, pokial audio aj video signalizuju, maximalne ale cas v konstante WaitToSync
		WaitForMultipleObjects(2, this->m_syncEvent, TRUE, WaitToSync);

		CAutoLock cAutoLock(&this->m_syncSection);

		this->m_signaled[index] = TRUE;
		if (this->m_signaled[0] && this->m_signaled[1])
		{
			//Ak audio aj video signalizovali, zmen do nesignaloveho stavu
			ResetEvent(this->m_syncEvent[0]);
			ResetEvent(this->m_syncEvent[1]);
			this->m_signaled[0] = FALSE;
			this->m_signaled[1] = FALSE;
		}
	}
}

CUnknown * WINAPI DesktopSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    DesktopSourceFilter * filter = new DesktopSourceFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}