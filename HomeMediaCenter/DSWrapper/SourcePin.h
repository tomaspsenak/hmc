#pragma once

#if !defined(SOURCEPIN_DSWRAPPER_INCLUDED)
#define SOURCEPIN_DSWRAPPER_INCLUDED

private class SourcePin : public CSourceStream
{
	public:		SourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, System::IO::Stream ^ stream);
				~SourcePin(void);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT SetMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q)
				{
					return E_FAIL;
				}

	private:	gcroot<System::IO::Stream^> m_inputStream;
				CCritSec m_readWriteSect;
};

#endif //SOURCEPIN_DSWRAPPER_INCLUDED
