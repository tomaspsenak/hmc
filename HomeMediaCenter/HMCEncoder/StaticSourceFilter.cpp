#include "StdAfx.h"
#include "StaticSourceFilter.h"

StaticSourceFilter::StaticSourceFilter(LPUNKNOWN pUnk, HRESULT * phr) 
	: CSource(L"HMCStaticSourceFilter", pUnk, CLSID_HMCStaticSource), m_sourceVideoPin(NULL), m_sourceAudioPin(NULL), m_params(NULL),
	m_rtStop(0)
{
	this->m_sourceVideoPin = new StaticSourceVideoPin(L"StaticSourceVideoPin", phr, this, L"Video");
	if (this->m_sourceVideoPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_sourceAudioPin = new StaticSourceAudioPin(L"StaticSourceAudioPin", phr, this, L"Audio");
	if (this->m_sourceAudioPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_params = new StaticSourceParameters(this);
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	//Nie je potrebne davat AddRef objektu StaticSourcePin a StaticSourceAudioPin, vsetko sa deleguje na StaticSourceFilter
}

StaticSourceFilter::~StaticSourceFilter(void)
{
	if (this->m_sourceVideoPin != NULL)
		delete this->m_sourceVideoPin;
	if (this->m_sourceAudioPin != NULL)
		delete this->m_sourceAudioPin;
	if (this->m_params != NULL)
		delete this->m_params;
}

STDMETHODIMP StaticSourceFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
	{
		return GetInterface((IAMFilterMiscFlags*) this, ppv);
	}
	else if (riid == IID_IHMCStaticSource)
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

STDMETHODIMP StaticSourceFilter::GetPages(CAUUID * pPages)
{
	CheckPointer(pPages, E_POINTER);

	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL)
	{
		pPages->cElems = 0;
		return E_OUTOFMEMORY;
	}

	pPages->cElems = 1;
	pPages->pElems[0] = CLSID_HMCStaticSourcePage;

	return S_OK;
}

//************ IAMFilterMiscFlags **********\\

ULONG STDMETHODCALLTYPE StaticSourceFilter::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

//************* IMediaSeeking **************\\

STDMETHODIMP StaticSourceFilter::GetCapabilities(DWORD * pCapabilities)
{
	CheckPointer(pCapabilities, E_POINTER);
	*pCapabilities = 0;
	return S_OK;
}

STDMETHODIMP StaticSourceFilter::CheckCapabilities(DWORD * pCapabilities)
{
	CheckPointer(pCapabilities, E_POINTER);
	if(*pCapabilities == 0)
		return S_OK;
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::IsFormatSupported(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	return (*pFormat == TIME_FORMAT_MEDIA_TIME) ? S_OK : S_FALSE;
}

STDMETHODIMP StaticSourceFilter::QueryPreferredFormat(GUID * pFormat)
{
	return GetTimeFormat(pFormat);
}

STDMETHODIMP StaticSourceFilter::GetTimeFormat(GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}

STDMETHODIMP StaticSourceFilter::IsUsingTimeFormat(const GUID * pFormat)
{
	return IsFormatSupported(pFormat);
}

STDMETHODIMP StaticSourceFilter::SetTimeFormat(const GUID * pFormat)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetDuration(LONGLONG * pDuration)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetStopPosition(LONGLONG * pStop)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetCurrentPosition(LONGLONG * pCurrent)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags, LONGLONG * pStop, DWORD dwStopFlags)
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

STDMETHODIMP StaticSourceFilter::GetPositions(LONGLONG * pCurrent, LONGLONG * pStop)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::SetRate(double dRate)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetRate(double * pdRate)
{
	return E_NOTIMPL;
}

STDMETHODIMP StaticSourceFilter::GetPreroll(LONGLONG* pllPreroll)
{
	return E_NOTIMPL;
}

CUnknown * WINAPI StaticSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    StaticSourceFilter * filter = new StaticSourceFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}
