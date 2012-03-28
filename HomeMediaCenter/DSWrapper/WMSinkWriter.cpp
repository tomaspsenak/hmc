#include "StdAfx.h"
#include "WMSinkWriter.h"


WMSinkWriter::WMSinkWriter(System::IO::Stream ^ outputStream) : m_nRefCount(1), m_outputStream(outputStream)
{
}

WMSinkWriter::~WMSinkWriter(void)
{
}

STDMETHODIMP WMSinkWriter::QueryInterface(REFIID riid, void ** ppv)
{
	if (ppv == NULL) 
    {
        return E_POINTER;
    }
    else if (riid == __uuidof(IWMWriterSink) || riid == IID_IUnknown) 
    {
        *ppv = static_cast<IWMWriterSink *>(this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP WMSinkWriter::AllocateDataUnit(DWORD cbDataUnit, INSSBuffer ** ppDataUnit)
{
	HRESULT hr = S_OK;
	INSSBuffer * buffer = NULL;

	buffer = new CNSSBuffer(&hr, cbDataUnit);
	if (buffer == NULL)
		return E_OUTOFMEMORY;

	if (hr != S_OK)
	{
		buffer->Release();
		return hr;
	}

	*ppDataUnit = buffer;

	return S_OK;
}

STDMETHODIMP WMSinkWriter::IsRealTime(BOOL * pfRealTime)
{
	*pfRealTime = TRUE;
	return S_OK;
}

STDMETHODIMP WMSinkWriter::OnDataUnit(INSSBuffer * pDataUnit)
{
	HRESULT hr = S_OK;
	BYTE * buffer;
	DWORD count;

	CHECK_HR(hr = pDataUnit->GetBufferAndLength(&buffer, &count));

	CHECK_HR(hr = Write(buffer, count));

done:
	return hr;
}

STDMETHODIMP WMSinkWriter::OnEndWriting()
{
	try
	{
		this->m_outputStream->Flush();
	}
	catch (System::Exception^) { }

	return S_OK;
}

STDMETHODIMP WMSinkWriter::OnHeader(INSSBuffer * pHeader)
{
	HRESULT hr = S_OK;
	BYTE * buffer;
	DWORD count;

	CHECK_HR(hr = pHeader->GetBufferAndLength(&buffer, &count));

	CHECK_HR(hr = Write(buffer, count));

done:
	return hr;
}

STDMETHODIMP WMSinkWriter::Write(BYTE * pv, ULONG cb)
{
	CAutoLock readWriteLock(&this->m_cSharedState);

	try
	{
		System::IntPtr intPtr = System::IntPtr(pv);
		array<System::Byte>^ buffer = gcnew array<System::Byte>(cb);
		System::Runtime::InteropServices::Marshal::Copy(intPtr, buffer, 0, cb);
		this->m_outputStream->Write(buffer, 0, cb);
	}
	catch (System::Exception^) 
	{
		return E_FAIL;
	}

	return S_OK;
}

//*************** WMSinkWriter ***************\\

CNSSBuffer::CNSSBuffer(HRESULT * phr, DWORD cbMaxLen) : m_nRefCount(1), m_cbMaxLength(cbMaxLen), m_cbLength(0)
{
	this->m_pbData = new BYTE[cbMaxLen];
	if (this->m_pbData == NULL) 
		*phr = E_OUTOFMEMORY;
}

CNSSBuffer::~CNSSBuffer(void)
{
	if (this->m_pbData != NULL)
        delete [] this->m_pbData;
	
	this->m_pbData = NULL;
}

STDMETHODIMP CNSSBuffer::QueryInterface(REFIID riid, void ** ppv)
{
	if (ppv == NULL) 
    {
        return E_POINTER;
    }
    else if (riid == __uuidof(INSSBuffer) || riid == IID_IUnknown) 
    {
        *ppv = static_cast<INSSBuffer *>(this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP CNSSBuffer::GetBuffer(BYTE ** ppdwBuffer)
{
	if (ppdwBuffer == NULL) 
        return E_POINTER;
    *ppdwBuffer = this->m_pbData;

    return S_OK;
}

STDMETHODIMP CNSSBuffer::GetBufferAndLength(BYTE ** ppdwBuffer, DWORD * pdwLength)
{
	if (ppdwBuffer == NULL && pdwLength == NULL) 
        return E_POINTER;
    if (ppdwBuffer) 
        *ppdwBuffer = this->m_pbData;
    if (pdwLength) 
        *pdwLength = this->m_cbLength;

    return S_OK;
}

STDMETHODIMP CNSSBuffer::GetLength(DWORD * pdwLength)
{
	if (pdwLength == NULL) 
        return E_POINTER;
    *pdwLength = this->m_cbLength;

    return S_OK;
}

STDMETHODIMP CNSSBuffer::GetMaxLength(DWORD * pdwLength)
{
	if (pdwLength == NULL) 
        return E_POINTER;
    *pdwLength = this->m_cbMaxLength;

    return S_OK;
}

STDMETHODIMP CNSSBuffer::SetLength(DWORD dwLength)
{
	if (dwLength > this->m_cbMaxLength) 
        return E_INVALIDARG;
    this->m_cbLength = dwLength;

    return S_OK;
}