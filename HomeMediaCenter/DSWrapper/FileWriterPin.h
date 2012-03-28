#pragma once

#if !defined(FILEWRITERPIN_DSWRAPPER_INCLUDED)
#define FILEWRITERPIN_DSWRAPPER_INCLUDED

#include <strmif.h>

private class FileWriterPin : public CBaseInputPin, public IStream
{
	public:		FileWriterPin(TCHAR * pObjectName, CBaseFilter * pFilter, CCritSec * pLock, HRESULT * phr, 
					LPCWSTR pName, System::IO::Stream^ outputStream);
				~FileWriterPin(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
				
				//CBasePin
				HRESULT CheckMediaType(const CMediaType * pmt);

				//IPin
				STDMETHODIMP BeginFlush(void);
				STDMETHODIMP EndFlush(void);
				STDMETHODIMP EndOfStream(void);

				//IMemInputPin
				STDMETHODIMP Receive(IMediaSample *pSample);
				STDMETHODIMP GetAllocator(IMemAllocator **ppAllocator);

				//IStream
				STDMETHODIMP Read(void * pv, ULONG cb, ULONG * pcbRead);
				STDMETHODIMP Write(void const * pv, ULONG cb, ULONG * pcbWritten);
				STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition);
				STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize);
				STDMETHODIMP CopyTo(IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten);
				STDMETHODIMP Commit(DWORD grfCommitFlags);
				STDMETHODIMP Revert();
				STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
				STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
				STDMETHODIMP Stat(STATSTG * pstatstg, DWORD grfStatFlag);
				STDMETHODIMP Clone(IStream ** ppstm);

	private:	gcroot<System::IO::Stream^> m_outputStream;
				LONGLONG m_position;
				CCritSec m_readWriteSect;
};

#endif //FILEWRITERPIN_DSWRAPPER_INCLUDED

