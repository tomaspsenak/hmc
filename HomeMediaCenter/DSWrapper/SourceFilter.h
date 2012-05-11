#pragma once

#if !defined(SOURCEFILTER_DSWRAPPER_INCLUDED)
#define SOURCEFILTER_DSWRAPPER_INCLUDED

class SourcePin;

private class SourceFilter : public CSource
{
	public:		SourceFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream ^ stream);
				~SourceFilter(void);

	private:	SourcePin * m_sourcePin;
};

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

#endif //SOURCEFILTER_DSWRAPPER_INCLUDED