#include "StdAfx.h"
#include "DesktopSourceFilter.h"

DesktopSourceFilter::DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr) 
	: CSource(L"HMCDesktopSourceFilter", pUnk, CLSID_HMCDesktopSource), m_sourceVideoPin(NULL), m_sourceAudioPin(NULL), m_params(NULL)
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