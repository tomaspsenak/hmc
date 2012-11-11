#include "StdAfx.h"
#include "FileWriterFilter.h"

FileWriterFilter::FileWriterFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream^ outputStream, GUID inputSubtype)  
	:  CBaseFilter(L"FileWriterFilter", pUnk, &m_critSection, CLSID_NULL, phr), m_inputSubtype(inputSubtype), m_passThru(NULL)
{
	this->m_writerPin = new FileWriterPin(L"FileWriterPin", this, &m_critSection, phr, L"Input", outputStream);
	if (this->m_writerPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
	
	if (*phr != S_OK)
		return;
	//Nie je potrebne davat AddRef objektu FileWriterPin, vsetko sa deleguje na FileWriterFilter

	this->m_passThru = new CPosPassThru(L"FWPassThru COM Object", GetOwner(), phr, this->m_writerPin);
	if (this->m_passThru == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}

FileWriterFilter::~FileWriterFilter(void)
{
	if (this->m_writerPin != NULL)
		delete this->m_writerPin;
	if (this->m_passThru != NULL)
		delete this->m_passThru;
}

STDMETHODIMP FileWriterFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IAMFilterMiscFlags)
	{
		return GetInterface((IAMFilterMiscFlags*) this, ppv);
	}
	else if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking) 
    {
		return this->m_passThru->NonDelegatingQueryInterface(riid, ppv);
	}

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
	m_outputStream(outputStream), CBaseInputPin(pObjectName, pFilter, pLock, phr, pName), CAMThread(phr), m_buffer(nullptr)
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

//********** CBaseOutputPin **********\\

HRESULT FileWriterPin::Active(void)
{
	CAutoLock lock(this->m_pLock);

	//Zdroje uz su alokovane?
    if (this->m_pFilter->IsActive())
		return S_FALSE;

    //Pin musi byt pripojeny
    if (!IsConnected())
        return E_FAIL;

	//Inicializuj memoryallocator
    HRESULT hr = CBaseInputPin::Active();
    if (FAILED(hr))
        return hr;

    //Spustenie vlakna
    if (!Create())
        return E_FAIL;

    return S_OK;
}

HRESULT FileWriterPin::Inactive(void)
{
	CAutoLock lock(this->m_pLock);

	//Pin nebol pripojeny
    if (!IsConnected())
        return S_OK;

    // !!! need to do this before trying to stop the thread, because
    // we may be stuck waiting for our own allocator!!!
    HRESULT hr = CBaseInputPin::Inactive();
    if (FAILED(hr))
		return hr;

	//Treba uzamknut thread handle inak moze vzniknut race condition
	CAutoLock lock2(&this->m_AccessLock);
    if (ThreadExists())
	{
		hr = CallWorker(CMD_STOP);
		if (FAILED(hr))
			return hr;

		//Pockat na ukoncenie vlakna
		Close();
    }

    return S_OK;
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
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	LONG size = 0;
	ULONG written = 0;
	BYTE * buf = NULL;
	REFERENCE_TIME startTime = 0, endTime = 0;

	size = pSample->GetActualDataLength();
	CHECK_HR(hr = pSample->GetPointer(&buf));
	CHECK_HR(hr = pSample->GetTime(&startTime, &endTime));

	{
		//readWriteLock nemoze byt pri funkcii Write - deadlock
		CAutoLock readWriteLock(&this->m_readWriteSect);

		if (this->m_position != startTime)
		{	
			LARGE_INTEGER dlibMove;
			dlibMove.QuadPart = startTime;
			CHECK_HR(hr = Seek(dlibMove, STREAM_SEEK_SET, NULL));
		}
	}

	CHECK_HR(hr = Write(buf, size, &written));

done: return hr;
}

//*************** IStream ***************\\

STDMETHODIMP FileWriterPin::Read(void * pv, ULONG cb, ULONG * pcbRead)
{
	CheckPointer(pv, E_POINTER);

	//Synchronizovanie citania a zapisu - po skonceni platnosti sa odomkne
	CAutoLock readWriteLock(&this->m_readWriteSect);

	try
	{
		array<System::Byte>^ buffer = gcnew array<System::Byte>(cb);
		ULONG readed = this->m_outputStream->Read(buffer, 0, cb);
		this->m_position += readed;

		System::Runtime::InteropServices::Marshal::Copy(buffer, 0, System::IntPtr(pv), readed);
		if (pcbRead)
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
	CheckPointer(pv, E_POINTER);

	if (this->m_buffer != nullptr)
		ASSERT("m_buffer must be NULL");

	this->m_buffer = gcnew array<System::Byte>(cb);
	System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void *)pv), this->m_buffer, 0, cb);
	if (pcbWritten)
		*pcbWritten = cb;

	//Ked vrati CallWorker hodnotu, vlakno uz bude mat m_readWriteSect
	HRESULT hr = CallWorker(CMD_WRITE);
	if (hr != S_OK)
	{
		//Ak sa nepodari zapisat buffer asynchronne, pokusi sa synchronne
		hr = Write(this->m_buffer);
	}
	this->m_buffer = nullptr;

	return hr;
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

	if (plibNewPosition)
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

//************ CAMThread ************\\

DWORD FileWriterPin::ThreadProc(void)
{
	Command cmd;

    do 
	{
		cmd = (Command)GetRequest();

		switch (cmd)
		{
			case CMD_WRITE:
			{
				CAutoLock readWriteLock(&this->m_readWriteSect);

				array<System::Byte>^ buffer = this->m_buffer;
				Reply(S_OK);

				Write(buffer);
				
				break;
			}
			case CMD_STOP:
				Reply(S_OK);
				break;
			default:
				Reply(E_NOTIMPL);
				break;
		}
    } while (cmd != CMD_STOP);

	return 0;
}

HRESULT FileWriterPin::Write(array<System::Byte> ^ buffer)
{
	try
	{
		this->m_outputStream->Write(buffer, 0, buffer->Length);
		this->m_position += buffer->Length;
	}
	catch (System::Exception^)
	{
		this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
		return E_FAIL;
	}

	return S_OK;
}
