#include "StdAfx.h"
#include "SinkSourceFilter.h"

SinkSourceFilter::SinkSourceFilter(LPUNKNOWN pUnk, HRESULT * phr)
	: CSource(L"SinkSourceFilter", pUnk, CLSID_NULL)
{
}


SinkSourceFilter::~SinkSourceFilter(void)
{
}

STDMETHODIMP SinkSourceFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
		return GetInterface((IAMFilterMiscFlags*) this, ppv);

	return CSource::NonDelegatingQueryInterface(riid, ppv);
}

SinkSourceFilter * SinkSourceFilter::Create(void)
{
	HRESULT hr = S_OK;

	SinkSourceFilter * filter = new SinkSourceFilter(NULL, &hr);
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

//************ IAMFilterMiscFlags ************

ULONG STDMETHODCALLTYPE SinkSourceFilter::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}


//********************************************
//************** SinkSourcePin ***************
//********************************************


SinkSourcePin::SinkSourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, SinkFilterGraphPin * sinkPin)
	: CSourceStream(pObjectName, phr, pms, pName), m_sinkPin(sinkPin), m_position(NULL), m_buffer(NULL), m_syncObj(gcnew System::Object())
{
	sinkPin->AddRef();
}


SinkSourcePin::~SinkSourcePin(void)
{
	if (this->m_buffer != NULL)
		delete [] this->m_buffer;

	SAFE_RELEASE(this->m_sinkPin);
}

STDMETHODIMP SinkSourcePin::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMPushSource)
	{
		CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
		if (this->m_sinkPin->m_pushSource != NULL)
			return GetInterface((IAMPushSource*) this, ppv);
	}

	return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP SinkSourcePin::EnumMediaTypes(IEnumMediaTypes ** ppEnum)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);

	if (this->m_sinkPin->m_Connected == NULL)
		return VFW_E_NOT_CONNECTED;

	//Ak uz je nastaveny typ, vrati prave ten (cez GetMediaType). Inak vrati zoznam
	//podporovanych typov zdrojoveho pinu
	if (this->m_sinkPin->m_isFixedType)
		return CSourceStream::EnumMediaTypes(ppEnum);
	else
		return this->m_sinkPin->m_Connected->EnumMediaTypes(ppEnum);
}

HRESULT SinkSourcePin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);

	if (this->m_sinkPin->m_Connected == NULL)
		return VFW_E_NOT_CONNECTED;

	return pmt->Set(this->m_sinkPin->m_mt);
}

HRESULT SinkSourcePin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);

	if (this->m_sinkPin->m_Connected == NULL)
		return VFW_E_NOT_CONNECTED;

	if (this->m_sinkPin->m_isFixedType)
		return pMediaType->MatchesPartial(&this->m_sinkPin->m_mt) ? S_OK : E_FAIL;
	else
		return this->m_sinkPin->m_Connected->QueryAccept(pMediaType);
}

HRESULT SinkSourcePin::CompleteConnect(IPin * pReceivePin)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);

	if (this->m_sinkPin->m_Connected == NULL)
		return VFW_E_NOT_CONNECTED;

	if (this->m_sinkPin->m_isFixedType)
	{
		if (this->m_mt != this->m_sinkPin->m_mt)
			return E_FAIL;
	}
	else if (this->m_mt != this->m_sinkPin->m_mt)
	{
		if (this->m_sinkPin->m_Connected->QueryAccept(&this->m_mt) != S_OK)
			return E_FAIL;

		if (this->m_sinkPin->m_pFilter->ReconnectPin(this->m_sinkPin->m_Connected, &this->m_mt) != S_OK)
			return E_FAIL;
	}

	if (CSourceStream::CompleteConnect(pReceivePin) != S_OK)
		return E_FAIL;

	this->m_sinkPin->m_isFixedType = TRUE;

	return S_OK;
}

HRESULT SinkSourcePin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);

    if (this->m_sinkPin->m_Connected == NULL)
        return VFW_E_NOT_CONNECTED;

    HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES actual;

	CHECK_HR(hr = this->m_sinkPin->m_pAllocator->GetProperties(pProperties));
    CHECK_HR(hr = pAlloc->SetProperties(pProperties, &actual));

    if (pProperties->cBuffers > actual.cBuffers || pProperties->cbBuffer > actual.cbBuffer)
		CHECK_HR(hr =  E_FAIL);

	if (this->m_buffer != NULL)
		delete [] this->m_buffer;
	this->m_buffer = new BYTE[actual.cbBuffer];
	if (this->m_buffer == NULL)
		CHECK_HR(hr = E_OUTOFMEMORY);

done:
	return hr;
}

HRESULT SinkSourcePin::Active(void)
{
	if (this->m_position == NULL)
	{
		this->m_position = this->m_sinkPin->AddPinsList(this);
		if (this->m_position == NULL)
			return E_FAIL;
	}

	return CSourceStream::Active();
}

HRESULT SinkSourcePin::Inactive(void)
{
	HRESULT hr = CSourceStream::Inactive();

	if (this->m_position != NULL)
	{
		this->m_sinkPin->RemovePinsList(this->m_position);
		this->m_position = NULL;
	}

	return hr;
}

HRESULT SinkSourcePin::Receive(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	System::Threading::Monitor::Enter(this->m_syncObj);

	HRESULT hr = S_OK;
	BYTE * sampleData;
    DWORD sampleDataCount;

	CHECK_HR(hr = pSample->GetTime(&this->m_startTime, &this->m_endTime));
	CHECK_HR(hr = pSample->GetMediaTime(&this->m_startMediaTime, &this->m_endMediaTime));
	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    sampleDataCount = pSample->GetSize();

	this->m_isDiscontinuity = pSample->IsDiscontinuity();
	this->m_isPreroll = FALSE;
	this->m_isSyncPoint = pSample->IsSyncPoint();

	memcpy(this->m_buffer, sampleData, sampleDataCount);

	System::Threading::Monitor::Pulse(this->m_syncObj);

done:

	System::Threading::Monitor::Exit(this->m_syncObj);
	return S_OK;
}

HRESULT SinkSourcePin::FillBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	BYTE * sampleData;
    DWORD sampleDataCount;

	System::Threading::Monitor::Enter(this->m_syncObj);
	while (!System::Threading::Monitor::Wait(this->m_syncObj, 500))
	{
		if (this->m_pFilter->IsStopped())
		{
			this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
			CHECK_HR(hr = E_FAIL);
		}
	}

	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    sampleDataCount = pSample->GetSize();

	memcpy(sampleData, this->m_buffer, sampleDataCount);

	CHECK_HR(hr = pSample->SetActualDataLength(sampleDataCount));
	CHECK_HR(hr = pSample->SetTime(&this->m_startTime, &this->m_endTime));
	CHECK_HR(hr = pSample->SetMediaTime(&this->m_startMediaTime, &this->m_endMediaTime));

	pSample->SetDiscontinuity(this->m_isDiscontinuity);
	pSample->SetPreroll(this->m_isPreroll);
	pSample->SetSyncPoint(this->m_isSyncPoint);

done:

	System::Threading::Monitor::Exit(this->m_syncObj);
	return hr;
}

//************* IAMPushSource *************

STDMETHODIMP SinkSourcePin::GetMaxStreamOffset(REFERENCE_TIME * prtMaxOffset)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->GetMaxStreamOffset(prtMaxOffset);

	return E_NOTIMPL;
}

STDMETHODIMP SinkSourcePin::GetPushSourceFlags(ULONG * pFlags)
{
	*pFlags = AM_PUSHSOURCEREQS_USE_STREAM_CLOCK;

	return S_OK;
}

STDMETHODIMP SinkSourcePin::GetStreamOffset(REFERENCE_TIME * prtOffset)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->GetStreamOffset(prtOffset);

	return E_NOTIMPL;
}

STDMETHODIMP SinkSourcePin::SetMaxStreamOffset(REFERENCE_TIME rtMaxOffset)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->SetMaxStreamOffset(rtMaxOffset);

	return E_NOTIMPL;
}

STDMETHODIMP SinkSourcePin::SetPushSourceFlags(ULONG Flags)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->SetPushSourceFlags(Flags);

	return E_NOTIMPL;
}

STDMETHODIMP SinkSourcePin::SetStreamOffset(REFERENCE_TIME rtOffset)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->SetStreamOffset(rtOffset);

	return E_NOTIMPL;
}

STDMETHODIMP SinkSourcePin::GetLatency(REFERENCE_TIME * prtLatency)
{
	CAutoLock cAutoLock(this->m_sinkPin->m_pLock);
	if (this->m_sinkPin->m_pushSource != NULL)
		return this->m_sinkPin->m_pushSource->GetLatency(prtLatency);

	return E_NOTIMPL;
}