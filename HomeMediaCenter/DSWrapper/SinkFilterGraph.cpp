#include "StdAfx.h"
#include "SinkFilterGraph.h"

SinkFilterGraph::SinkFilterGraph(LPUNKNOWN pUnk, HRESULT * phr, IPin * sourcePin)
	: CBaseFilter(L"SinkFilterGraph", pUnk, &m_critSection, CLSID_NULL, phr), m_filterGraph(NULL), m_mediaControl(NULL), m_sinkPin(NULL)
{
	HRESULT hr = S_OK;
	PIN_INFO pinInfo;
	pinInfo.pFilter = NULL;

	this->m_sinkPin = new SinkFilterGraphPin(L"SinkFilterGraphPin", this, &m_critSection, phr, L"Input", sourcePin);
	if (this->m_sinkPin == NULL)
	{
		CHECK_HR(hr = E_OUTOFMEMORY);
	}
	CHECK_HR(hr = *phr);
	
	CHECK_HR(hr = sourcePin->QueryPinInfo(&pinInfo));

	CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (void **)&this->m_filterGraph));
	CHECK_HR(hr = this->m_filterGraph->QueryInterface(IID_IMediaControl, (void **)&this->m_mediaControl));

	CHECK_SUCCEED(hr = this->m_filterGraph->AddFilter(pinInfo.pFilter, NULL));
	CHECK_SUCCEED(hr = this->m_filterGraph->AddFilter(this, NULL));
	CHECK_SUCCEED(hr = this->m_filterGraph->ConnectDirect(sourcePin, this->m_sinkPin, NULL));

	hr = S_OK;

done:

	SAFE_RELEASE(pinInfo.pFilter);
	*phr = hr;
}

SinkFilterGraph::~SinkFilterGraph(void)
{
	if (this->m_sinkPin != NULL)
		delete this->m_sinkPin;

	SAFE_RELEASE(this->m_filterGraph);
	SAFE_RELEASE(this->m_mediaControl);
}

void SinkFilterGraph::ReleaseAll(void)
{
	SAFE_RELEASE(this->m_filterGraph);
	SAFE_RELEASE(this->m_mediaControl);
	Release();
}

SinkFilterGraph * SinkFilterGraph::Create(IPin * sourcePin)
{
	HRESULT hr = S_OK;

	SinkFilterGraph * filter = new SinkFilterGraph(NULL, &hr, sourcePin);
	if (filter != NULL)
	{
		filter->AddRef();
		if (hr != S_OK)
		{
			SAFE_RELEASE(filter);
		}
	}

	return filter;
}

STDMETHODIMP SinkFilterGraph::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
		return GetInterface((IAMFilterMiscFlags*) this, ppv);

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP SinkFilterGraph::InsertSourcePin(SinkSourceFilter * sourceFilter)
{
	HRESULT hr = S_OK;

	SinkSourcePin * pin = new SinkSourcePin(L"SinkSourcePin", &hr, sourceFilter, L"Out", this->m_sinkPin);
	if (pin == NULL)
		return E_OUTOFMEMORY;
		
	pin->AddRef();
	SAFE_RELEASE(pin);

	return hr;
}

//*************** CBaseFilter ***************

int SinkFilterGraph::GetPinCount(void)
{
    return 1;
}

CBasePin * SinkFilterGraph::GetPin(int n)
{
    if(n == 0)
        return this->m_sinkPin;

    return NULL;
}

//************ IAMFilterMiscFlags ************

ULONG STDMETHODCALLTYPE SinkFilterGraph::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}


//********************************************
//************ SinkFilterGraphPin ************
//********************************************


SinkFilterGraphPin::SinkFilterGraphPin(TCHAR * pObjectName, CBaseFilter * pFilter, CCritSec * pLock, HRESULT * phr, LPCWSTR pName, IPin * sourcePin) :
	CBaseInputPin(pObjectName, pFilter, pLock, phr, pName), m_sourcePin(sourcePin), m_pinsList(NAME("SinkSourcePinList")), m_isFixedType(FALSE),
	m_pushSource(NULL)
{
	sourcePin->AddRef();
	InitPinsList();

	sourcePin->QueryInterface(IID_IAMPushSource, (void **)&this->m_pushSource);
}

SinkFilterGraphPin::~SinkFilterGraphPin(void)
{
	InitPinsList();
	SAFE_RELEASE(this->m_sourcePin);
	SAFE_RELEASE(this->m_pushSource);
}

//*************** CBasePin ***************

HRESULT SinkFilterGraphPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	return S_OK;
}

//***************** IPin *****************
STDMETHODIMP SinkFilterGraphPin::BeginFlush(void)
{
	return CBaseInputPin::BeginFlush();
}

STDMETHODIMP SinkFilterGraphPin::EndFlush(void)
{
	return CBaseInputPin::EndFlush();
}

STDMETHODIMP SinkFilterGraphPin::EndOfStream(void)
{
	return CBaseInputPin::EndOfStream();
}

//************* IMemInputPin *************
STDMETHODIMP SinkFilterGraphPin::GetAllocator(IMemAllocator ** ppAllocator)
{
	return VFW_E_NO_ALLOCATOR;
}

STDMETHODIMP SinkFilterGraphPin::Receive(IMediaSample * pSample)
{
	LPCRITICAL_SECTION pLock = (LPCRITICAL_SECTION)this->m_pLock;

	//Po zavolani IMediaControl Stop vznika deadlock (caka na ukoncenie tejto funkcie), 
	//preto kontroluje stav a v pripade ukoncenia opusti fuknciu Receive
	while (!TryEnterCriticalSection(pLock))
	{
		Sleep(90);
		if (this->m_pFilter->IsStopped())
			return E_FAIL;
	}

	POSITION pos = this->m_pinsList.GetHeadPosition();
	while(pos)
    {
        SinkSourcePin * pPin = this->m_pinsList.GetNext(pos);
        if(pPin != NULL)
            pPin->Receive(pSample);
    }

	LeaveCriticalSection(pLock);

	return S_OK;
}

void SinkFilterGraphPin::InitPinsList(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	POSITION pos = this->m_pinsList.GetHeadPosition();
    while(pos)
    {
        SinkSourcePin * pPin = this->m_pinsList.GetNext(pos);
        pPin->Release();
    }
    this->m_pinsList.RemoveAll();
}

POSITION SinkFilterGraphPin::AddPinsList(SinkSourcePin * sourcePin)
{
	CAutoLock cAutoLock(this->m_pLock);

	POSITION pos = this->m_pinsList.AddTail(sourcePin);
	if (pos != NULL)
	{
		sourcePin->AddRef();

		this->m_isFixedType = TRUE;

		if (this->m_pFilter->IsStopped())
			((SinkFilterGraph *)this->m_pFilter)->m_mediaControl->Run();
	}

	return pos;
}

void SinkFilterGraphPin::RemovePinsList(POSITION position)
{
	CAutoLock cAutoLock(this->m_pLock);

	SinkSourcePin * pPin = this->m_pinsList.Remove(position);
	if (pPin != NULL)
		pPin->Release();

	if (this->m_pinsList.GetCount() == 0)
	{
		((SinkFilterGraph *)this->m_pFilter)->m_mediaControl->Stop();

		this->m_isFixedType = FALSE;
	}
}