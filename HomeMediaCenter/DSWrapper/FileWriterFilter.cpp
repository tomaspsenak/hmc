#include "StdAfx.h"
#include "FileWriterFilter.h"

FileWriterFilter::FileWriterFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream^ outputStream)  
	: CBaseFilter(L"FileWriterFilter", pUnk, &m_critSection, CLSID_NULL, phr), m_inputSubtype(GUID_NULL)
{
	this->m_writerPin = new FileWriterPin(L"FileWriterPin", this, &m_critSection, phr, L"Input", outputStream);
	if (this->m_writerPin == NULL)
		*phr = E_OUTOFMEMORY;
	else
		this->m_writerPin->AddRef();
}

FileWriterFilter::~FileWriterFilter(void)
{
	SAFE_RELEASE(this->m_writerPin);
}

STDMETHODIMP FileWriterFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
		return GetInterface((IAMFilterMiscFlags*) this, ppv);

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

//*************** CBaseFilter ***************\\

int FileWriterFilter::GetPinCount(void)
{
    return 1;
}

CBasePin * FileWriterFilter::GetPin(int n)
{
    if(n == 0)
        return this->m_writerPin;

    return NULL;
}

//************ IAMFilterMiscFlags ************

ULONG STDMETHODCALLTYPE FileWriterFilter::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}
