#include "StdAfx.h"
#include "FileWriterFilter.h"

FileWriterFilter::FileWriterFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream^ outputStream, GUID inputSubtype)  
	:  CBaseFilter(L"FileWriterFilter", pUnk, &m_critSection, CLSID_NULL, phr), m_inputSubtype(inputSubtype)
{
	this->m_writerPin = new FileWriterPin(L"FileWriterPin", this, &m_critSection, phr, L"Input", outputStream);
	if (this->m_writerPin == NULL)
		*phr = E_OUTOFMEMORY;
	//Nie je potrebne davat AddRef objektu FileWriterPin, vsetko sa deleguje na FileWriterFilter
}

FileWriterFilter::~FileWriterFilter(void)
{
	if (this->m_writerPin != NULL)
		delete this->m_writerPin;
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


//********************************************
//************** FileWriterPin ***************
//********************************************


FileWriterPin::FileWriterPin(TCHAR * pObjectName, CBaseFilter * pFilter, CCritSec * pLock, HRESULT * phr, LPCWSTR pName, System::IO::Stream^ outputStream) :
	m_outputStream(outputStream), CBaseInputPin(pObjectName, pFilter, pLock, phr, pName)
{
	try
	{
		//Zisti aktualnu poziciu v streame - napr. file stream
		this->m_position = outputStream->Position;
	}
	catch (System::Exception^)
	{
		//Neda sa zistit aktualna pozicia - napr. network stream
		this->m_position = 0;
	}
}

FileWriterPin::~FileWriterPin(void)
{
}

STDMETHODIMP FileWriterPin::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IStream)
		return GetInterface((IStream*) this, ppv);

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv);
}

//************* CBasePin *************\\

HRESULT FileWriterPin::CheckMediaType(const CMediaType * pmt)
{
	GUID subtype = ((FileWriterFilter *)m_pFilter)->m_inputSubtype;

	if (pmt->majortype == MEDIATYPE_Stream && pmt->subtype == subtype) 
		return S_OK;

	return S_FALSE;
}

//*************** IPin ***************\\

STDMETHODIMP FileWriterPin::BeginFlush(void)
{
	return CBaseInputPin::BeginFlush();
}

STDMETHODIMP FileWriterPin::EndFlush(void)
{
	return CBaseInputPin::EndFlush();
}

STDMETHODIMP FileWriterPin::EndOfStream(void)
{
	try 
	{
		CAutoLock readWriteLock(&this->m_readWriteSect);
		this->m_outputStream->Flush(); 
	}
	catch (System::Exception^) { }
	//Kazdy stream musi notifikovat o okonceni
	this->m_pFilter->NotifyEvent(EC_COMPLETE, S_OK, NULL);

	return S_OK;
}

//************ IMemInputPin ************

STDMETHODIMP FileWriterPin::GetAllocator(IMemAllocator ** ppAllocator)
{
	//Vstupny filter nemusi poskytovat allocator - povodny nepracoval s WebM
	return VFW_E_NO_ALLOCATOR;
}

STDMETHODIMP FileWriterPin::Receive(IMediaSample * pSample)
{
	HRESULT hr = S_OK;
	LONG size = 0;
	ULONG written = 0;
	BYTE * buf = NULL;

	size = pSample->GetActualDataLength();
	CHECK_HR(hr = pSample->GetPointer(&buf));

	CHECK_HR(hr = Write(buf, size, &written));

done: return hr;
}

//*************** IStream ***************\\

STDMETHODIMP FileWriterPin::Read(void * pv, ULONG cb, ULONG * pcbRead)
{
	//Synchronizovanie citania a zapisu - po skonceni platnosti sa odomkne
	CAutoLock readWriteLock(&this->m_readWriteSect);

	try
	{
		System::IntPtr intPtr = System::IntPtr(pv);
		array<System::Byte>^ buffer = gcnew array<System::Byte>(cb);
		ULONG readed = this->m_outputStream->Read(buffer, 0, cb);
		this->m_position += readed;

		System::Runtime::InteropServices::Marshal::Copy(buffer, 0, intPtr, readed);
		*pcbRead = readed;
	}
	catch (System::Exception^)
	{
		this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP FileWriterPin::Write(void const * pv, ULONG cb, ULONG * pcbWritten)
{
	CAutoLock readWriteLock(&this->m_readWriteSect);

	BYTE * pvb = (BYTE *)pv;

	try
	{
		System::IntPtr intPtr = System::IntPtr(pvb);
		array<System::Byte>^ buffer = gcnew array<System::Byte>(cb);
		System::Runtime::InteropServices::Marshal::Copy(intPtr, buffer, 0, cb);

		this->m_outputStream->Write(buffer, 0, cb);
		this->m_position += cb;
		*pcbWritten = cb;
	}
	catch (System::Exception^)
	{
		this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP FileWriterPin::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition)
{
	CAutoLock readWriteLock(&this->m_readWriteSect);

	LONGLONG pos = dlibMove.QuadPart;

	try
	{
		switch (dwOrigin)
		{
			case STREAM_SEEK_SET:
				if (this->m_position != pos)
				{
					this->m_outputStream->Seek(pos, System::IO::SeekOrigin::Begin);
					this->m_position = pos; 
				}
				break;
			case STREAM_SEEK_CUR:
				if (pos != 0)
				{
					this->m_position += pos; 
					this->m_outputStream->Seek(this->m_position, System::IO::SeekOrigin::Begin);
				}
				break;
			case STREAM_SEEK_END:
				this->m_position = this->m_outputStream->Length + pos; 
				this->m_outputStream->Seek(this->m_position, System::IO::SeekOrigin::Begin);
				break;
		}
	}
	catch (System::Exception^)
	{
		this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
		return E_FAIL;
	}

	plibNewPosition->QuadPart = this->m_position;
	return S_OK;
}

STDMETHODIMP FileWriterPin::SetSize(ULARGE_INTEGER libNewSize)
{
	CAutoLock readWriteLock(&this->m_readWriteSect);

	LONGLONG size = libNewSize.QuadPart;

	try
	{
		this->m_outputStream->SetLength(size);
	}
	catch (System::Exception^)
	{
		this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
		return E_NOTIMPL;
	}

	return S_OK;
}

STDMETHODIMP FileWriterPin::CopyTo(IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten)
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::Commit(DWORD grfCommitFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::Revert()
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::Stat(STATSTG * pstatstg, DWORD grfStatFlag)
{
	return E_NOTIMPL;
}

STDMETHODIMP FileWriterPin::Clone(IStream ** ppstm)
{
	return E_NOTIMPL;
}
