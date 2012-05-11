#pragma once

#if !defined(SINKSOURCEFILTER_DSWRAPPER_INCLUDED)
#define SINKSOURCEFILTER_DSWRAPPER_INCLUDED

class SinkSourceFilter;
class SinkSourcePin;

#include "SinkFilterGraph.h"

private class SinkSourceFilter : public CSource, public IAMFilterMiscFlags
{
	public:		SinkSourceFilter(LPUNKNOWN pUnk, HRESULT * phr);
				~SinkSourceFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static SinkSourceFilter * Create(void);

				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);
};

private class SinkSourcePin : public CSourceStream, public IAMPushSource
{
	public:		SinkSourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, SinkFilterGraphPin * sinkPin);
				~SinkSourcePin(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				STDMETHODIMP EnumMediaTypes(IEnumMediaTypes ** ppEnum);
				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT CompleteConnect(IPin * pReceivePin);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT Receive(IMediaSample * pSample);
				HRESULT FillBuffer(IMediaSample * pSample);
				HRESULT Active(void);
				HRESULT Inactive(void);

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q)
				{
					return E_FAIL;
				}

				//IAMPushSource
				STDMETHODIMP GetMaxStreamOffset(REFERENCE_TIME * prtMaxOffset);
				STDMETHODIMP GetPushSourceFlags(ULONG * pFlags);
				STDMETHODIMP GetStreamOffset(REFERENCE_TIME * prtOffset);
				STDMETHODIMP SetMaxStreamOffset(REFERENCE_TIME rtMaxOffset);
				STDMETHODIMP SetPushSourceFlags(ULONG Flags);
				STDMETHODIMP SetStreamOffset(REFERENCE_TIME rtOffset);
				STDMETHODIMP GetLatency(REFERENCE_TIME * prtLatency);

	private:	gcroot<System::Object^> m_syncObj;
				SinkFilterGraphPin * m_sinkPin;
				REFERENCE_TIME m_startTime;
				REFERENCE_TIME m_endTime;
				LONGLONG m_startMediaTime;
				LONGLONG m_endMediaTime;
				POSITION m_position;
				BYTE * m_buffer;

				BOOL m_isDiscontinuity;
				BOOL m_isPreroll;
				BOOL m_isSyncPoint;
};

#endif //SINKSOURCEFILTER_DSWRAPPER_INCLUDED

