#pragma once

#if !defined(MFSTREAM_MFWRAPPER_INCLUDED)
#define MFSTREAM_MFWRAPPER_INCLUDED

private class MFStream : public IMFByteStream
{
	public:		MFStream(HRESULT * hr, System::IO::Stream^ innerStream);
				virtual ~MFStream(void);

				//IUnknown
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv);
				STDMETHODIMP_(ULONG) AddRef();
				STDMETHODIMP_(ULONG) Release();

				//IMFByteStream
				HRESULT STDMETHODCALLTYPE BeginRead(BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState);
				HRESULT STDMETHODCALLTYPE BeginWrite(const BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState);
				HRESULT STDMETHODCALLTYPE Close();
				HRESULT STDMETHODCALLTYPE EndRead(IMFAsyncResult *pResult, ULONG *pcbRead);
				HRESULT STDMETHODCALLTYPE EndWrite(IMFAsyncResult *pResult, ULONG *pcbWritten);
				HRESULT STDMETHODCALLTYPE Flush();
				HRESULT STDMETHODCALLTYPE GetCapabilities(DWORD *pdwCapabilities);
				HRESULT STDMETHODCALLTYPE GetCurrentPosition(QWORD *pqwPosition);
				HRESULT STDMETHODCALLTYPE GetLength(QWORD *pqwLength);
				HRESULT STDMETHODCALLTYPE IsEndOfStream(BOOL *pfEndOfStream);
				HRESULT STDMETHODCALLTYPE Read(BYTE *pb, ULONG cb, ULONG *pcbRead);
				HRESULT STDMETHODCALLTYPE Seek(MFBYTESTREAM_SEEK_ORIGIN SeekOrigin, LONGLONG qwSeekOffset, DWORD dwSeekFlags, QWORD *pqwCurrentPosition);
				HRESULT STDMETHODCALLTYPE SetCurrentPosition(QWORD qwPosition);
				HRESULT STDMETHODCALLTYPE SetLength(QWORD qwLength);
				HRESULT STDMETHODCALLTYPE Write(const BYTE *pb, ULONG cb, ULONG *pcbWritten);

	private:	long m_cRef;
				DWORD m_workQueue;
				gcroot<System::IO::Stream^> m_innerStream;
				gcroot<array<System::Byte>^> m_managedBuffer;
				CRITICAL_SECTION m_criticalSection;
				IMFAttributes * m_attributes;
};

#endif //MFSTREAM_MFWRAPPER_INCLUDED

