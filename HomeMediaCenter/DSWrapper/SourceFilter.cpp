#include "StdAfx.h"
#include "SourceFilter.h"

SourceFilter::SourceFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream ^ stream)
	: CSource(L"SourceFilter", pUnk, CLSID_NULL)
{
	this->m_sourcePin = new SourcePin(L"SourcePin", phr, this, L"Out", stream);
	if (this->m_sourcePin == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
}


SourceFilter::~SourceFilter(void)
{
	if (this->m_sourcePin != NULL)
	{
		delete this->m_sourcePin;
	}
}


//********************************************
//**************** SourcePin *****************
//********************************************


SourcePin::SourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, System::IO::Stream ^ stream)
	: CSourceStream(pObjectName, phr, pms, pName), m_inputStream(stream)
{
}


SourcePin::~SourcePin(void)
{
}

HRESULT SourcePin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	pmt->SetType(&MEDIATYPE_Stream);
	pmt->SetSubtype(&MEDIASUBTYPE_MPEG2_TRANSPORT);

	return S_OK;
}

HRESULT SourcePin::SetMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);
	if (pMediaType->majortype != MEDIATYPE_Stream)
		return E_FAIL;
	if (pMediaType->subtype != MEDIASUBTYPE_MPEG2_TRANSPORT)
		return E_FAIL;

	return S_OK;
}

HRESULT SourcePin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);
	if (pMediaType->majortype != MEDIATYPE_Stream)
		return E_FAIL;
	if (pMediaType->subtype != MEDIASUBTYPE_MPEG2_TRANSPORT)
		return E_FAIL;

	return S_OK;
}

HRESULT SourcePin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;

    CAutoLock cAutoLock(m_pFilter->pStateLock());

    pProperties->cBuffers = 1;
	//Buffer podla velkosti UDP buffera
    pProperties->cbBuffer = 8192;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(pProperties, &aProp));

    if(aProp.cbBuffer < pProperties->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

HRESULT SourcePin::FillBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	BYTE * sampleData;
    DWORD sampleDataCount;
	ULONG readed = 0;

	CAutoLock cAutoLockShared(&m_readWriteSect);

	CHECK_HR(hr = pSample->GetPointer(&sampleData));
    sampleDataCount = pSample->GetSize();

	try
	{
		System::IntPtr intPtr = System::IntPtr(sampleData);
		array<System::Byte>^ buffer = gcnew array<System::Byte>(sampleDataCount);

		readed = this->m_inputStream->Read(buffer, 0, sampleDataCount);

		System::Runtime::InteropServices::Marshal::Copy(buffer, 0, intPtr, sampleDataCount);
	}
	catch (System::Exception^)
	{
		CHECK_HR(hr = E_FAIL);
	}

	pSample->SetActualDataLength(readed);

done:
	return hr;
}

