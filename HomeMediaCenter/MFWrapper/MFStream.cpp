#include "StdAfx.h"
#include "MFStream.h"
#include "ReadAsyncCallback.h"
#include "WriteAsyncCallback.h"
#include "MFAsyncObject.h"

MFStream::MFStream(HRESULT * hr, System::IO::Stream^ innerStream) : m_cRef(1), m_innerStream(innerStream)
{
	HRESULT p_hr;
	*hr = S_OK;

	this->m_managedBuffer = gcnew array<System::Byte>(65536);
	InitializeCriticalSection(&this->m_criticalSection);

	p_hr = MFAllocateWorkQueue(&this->m_workQueue);
	if (p_hr != S_OK)
		*hr = p_hr;

	p_hr = MFCreateAttributes(&this->m_attributes, 1);
	if (p_hr != S_OK)
		*hr = p_hr;

	try
	{
		//Niekedy je vyzadovany aj povodny nazov (MKV kontajner)
		System::String^ name = (System::String^)innerStream->GetType()->InvokeMember(L"Name", 
			System::Reflection::BindingFlags::GetProperty | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::Public, 
			nullptr, innerStream, gcnew array<System::Object^>(0));

		pin_ptr<const wchar_t> wch = PtrToStringChars(name);

		p_hr = this->m_attributes->SetString(MF_BYTESTREAM_ORIGIN_NAME, wch);
		if (p_hr != S_OK)
			*hr = p_hr;
	}
	catch (System::Exception^)
	{
	}
}


MFStream::~MFStream(void)
{
	MFUnlockWorkQueue(this->m_workQueue);
	this->m_workQueue = 0;
	DeleteCriticalSection(&this->m_criticalSection);
	SAFE_RELEASE(this->m_attributes);
}

//*************** IUnknown ***************\\

HRESULT MFStream::QueryInterface(REFIID riid, void** ppv)
{
	if (ppv == NULL)
        return E_POINTER;

	if (riid == __uuidof(IUnknown))
    {
        *ppv = this;
		this->AddRef();
    }
    else if (riid == __uuidof(IMFByteStream))
    {
        *ppv = static_cast<IMFByteStream*>(this);
		this->AddRef();
    }
	else if (riid == __uuidof(IMFAttributes))
	{
		*ppv = this->m_attributes;
		this->m_attributes->AddRef();
	}
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

	return S_OK;
}

ULONG MFStream::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG MFStream::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

//*************** IMFByteStream ***************\\

HRESULT MFStream::BeginRead(BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	HRESULT hr = S_OK;
	ReadAsyncCallback * readCallback = NULL;
	IMFAsyncResult * pResult = NULL;
	MFAsyncObject<ULONG> * asyncObject = NULL;

	asyncObject = new MFAsyncObject<ULONG>(0);
	readCallback = new ReadAsyncCallback(pb, cb, this);
	CHECK_HR(hr = MFCreateAsyncResult(asyncObject, pCallback, punkState, &pResult));

	CHECK_HR(hr = MFPutWorkItem(this->m_workQueue, readCallback, pResult));

done:

	SAFE_RELEASE(readCallback);
	SAFE_RELEASE(pResult);
	SAFE_RELEASE(asyncObject);

	return hr;
}

HRESULT MFStream::BeginWrite(const BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	HRESULT hr = S_OK;
	WriteAsyncCallback * writeCallback = NULL;
	IMFAsyncResult * pResult = NULL;
	MFAsyncObject<ULONG> * asyncObject = NULL;

	asyncObject = new MFAsyncObject<ULONG>(0);
	writeCallback = new WriteAsyncCallback(pb, cb, this);
	CHECK_HR(hr = MFCreateAsyncResult(asyncObject, pCallback, punkState, &pResult));

	CHECK_HR(hr = MFPutWorkItem(this->m_workQueue, writeCallback, pResult));

done:

	SAFE_RELEASE(writeCallback);
	SAFE_RELEASE(pResult);
	SAFE_RELEASE(asyncObject);

	return hr;
}

HRESULT MFStream::Close()
{
	return S_OK;
}

HRESULT MFStream::EndRead(IMFAsyncResult *pResult, ULONG *pcbRead)
{
	HRESULT hr = S_OK;
	ULONG value = 0;
	IUnknown * pUnk = NULL;

	CHECK_HR(hr = pResult->GetStatus());
	CHECK_HR(hr = pResult->GetObject(&pUnk));

	value = ((MFAsyncObject<ULONG>*)pUnk)->GetParam();
	*pcbRead = value;

done:
	
	SAFE_RELEASE(pUnk);

	return hr;
}

HRESULT MFStream::EndWrite(IMFAsyncResult *pResult, ULONG *pcbWritten)
{
	HRESULT hr = S_OK;
	ULONG value = 0;
	IUnknown * pUnk = NULL;

	CHECK_HR(hr = pResult->GetStatus());
	CHECK_HR(hr = pResult->GetObject(&pUnk));

	value = ((MFAsyncObject<ULONG>*)pUnk)->GetParam();
	*pcbWritten = value;

done:
	
	SAFE_RELEASE(pUnk);

	return hr;
}

HRESULT MFStream::Flush()
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		this->m_innerStream->Flush();
	}
	catch (System::Exception^)
	{
		hr = E_NOTIMPL;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::GetCapabilities(DWORD *pdwCapabilities)
{
	DWORD cap = 0x00000000;

	if (this->m_innerStream->CanRead)
		cap = cap | MFBYTESTREAM_IS_READABLE;
	if (this->m_innerStream->CanWrite)
		cap = cap | MFBYTESTREAM_IS_WRITABLE;
	if (this->m_innerStream->CanSeek)
		cap = cap | MFBYTESTREAM_IS_SEEKABLE;
	else
		cap = cap | MFBYTESTREAM_HAS_SLOW_SEEK;
	//cap = cap | MFBYTESTREAM_IS_PARTIALLY_DOWNLOADED;

	*pdwCapabilities = cap;

	return S_OK;
}

HRESULT MFStream::GetCurrentPosition(QWORD *pqwPosition)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		*pqwPosition = this->m_innerStream->Position;
	}
	catch (System::Exception^)
	{
		hr = E_NOTIMPL;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::GetLength(QWORD *pqwLength)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		*pqwLength = this->m_innerStream->Length;
	}
	catch (System::Exception^)
	{
		*pqwLength = -1;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::IsEndOfStream(BOOL *pfEndOfStream)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		if (this->m_innerStream->Position == this->m_innerStream->Length)
			*pfEndOfStream = TRUE;
		else
			*pfEndOfStream = FALSE;
	}
	catch (System::Exception^)
	{
		*pfEndOfStream = FALSE;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::Read(BYTE *pb, ULONG cb, ULONG *pcbRead)
{
	HRESULT hr = S_OK;
	ULONG count = GetMin<ULONG>(cb, this->m_managedBuffer->Length);
	ULONG readed = 0;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		readed = this->m_innerStream->Read(this->m_managedBuffer, 0, count);
	}
	catch (System::Exception^)
	{ 
		hr = E_ABORT;
	}

	for (ULONG i = 0; i < readed; i++)
			pb[i] = this->m_managedBuffer[i];
	*pcbRead = readed;

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::Seek(MFBYTESTREAM_SEEK_ORIGIN SeekOrigin, LONGLONG qwSeekOffset, DWORD dwSeekFlags, QWORD *pqwCurrentPosition)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		switch (SeekOrigin)
		{
			case msoBegin:
				this->m_innerStream->Seek(qwSeekOffset, System::IO::SeekOrigin::Begin);
				break;
			case msoCurrent:
				this->m_innerStream->Seek(qwSeekOffset, System::IO::SeekOrigin::Current);
				break;
		}
		*pqwCurrentPosition = this->m_innerStream->Position;
	}
	catch (System::Exception^)
	{
		hr = E_NOTIMPL;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::SetCurrentPosition(QWORD qwPosition)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		this->m_innerStream->Position = qwPosition;
	}
	catch (System::Exception^)
	{
		hr = E_NOTIMPL;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::SetLength(QWORD qwLength)
{
	HRESULT hr = S_OK;
	EnterCriticalSection(&this->m_criticalSection);

	try
	{
		this->m_innerStream->SetLength(qwLength);
	}
	catch (System::Exception^)
	{
		hr = E_NOTIMPL;
	}

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}

HRESULT MFStream::Write(const BYTE *pb, ULONG cb, ULONG *pcbWritten)
{
	HRESULT hr = S_OK;
	ULONG count = GetMin<ULONG>(cb, this->m_managedBuffer->Length);
	EnterCriticalSection(&this->m_criticalSection);

	for (ULONG i = 0; i < count; i++)
			this->m_managedBuffer[i] = pb[i];

	try
	{
		this->m_innerStream->Write(this->m_managedBuffer, 0, count);
		this->m_innerStream->Flush();
	}
	catch (System::Exception^)
	{ 
		hr = E_ABORT;
	}
	
	*pcbWritten = count;

	LeaveCriticalSection(&m_criticalSection);
	return hr;
}