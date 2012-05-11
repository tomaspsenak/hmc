#pragma once

#if !defined(SINKFILTERGRAPH_DSWRAPPER_INCLUDED)
#define SINKFILTERGRAPH_DSWRAPPER_INCLUDED

class SinkFilterGraph;
class SinkFilterGraphPin;

#include "SinkSourceFilter.h"

private class SinkFilterGraph : public CBaseFilter, public IAMFilterMiscFlags
{
	friend class SinkFilterGraphPin;

	public:		SinkFilterGraph(LPUNKNOWN pUnk, HRESULT * phr, IPin * sourcePin);
				~SinkFilterGraph(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static SinkFilterGraph * Create(IPin * sourcePin);
				void ReleaseAll(void);
				STDMETHODIMP InsertSourcePin(SinkSourceFilter * sourceFilter);

				//CBaseFilter
				int GetPinCount(void);
				CBasePin * GetPin(int n);

				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);

	private:	CCritSec m_critSection;
				IFilterGraph * m_filterGraph;
				IMediaControl * m_mediaControl;
				SinkFilterGraphPin * m_sinkPin;
};

private class SinkFilterGraphPin : public CBaseInputPin
{
	friend class SinkSourcePin;
	typedef CGenericList<SinkSourcePin> CSinkSourcePinList;

	public:		SinkFilterGraphPin(TCHAR * pObjectName, CBaseFilter * pFilter, CCritSec * pLock, HRESULT * phr, 
					LPCWSTR pName, IPin * sourcePin);
				~SinkFilterGraphPin(void);

				//CBasePin
				HRESULT CheckMediaType(const CMediaType * pmt);

				//IPin
				STDMETHODIMP BeginFlush(void);
				STDMETHODIMP EndFlush(void);
				STDMETHODIMP EndOfStream(void);

				//IMemInputPin
				STDMETHODIMP Receive(IMediaSample * pSample);
				STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);

	private:	void InitPinsList(void);
				POSITION AddPinsList(SinkSourcePin * sourcePin);
				void RemovePinsList(POSITION position);
		
				CSinkSourcePinList m_pinsList;
				IAMPushSource * m_pushSource;
				IPin * m_sourcePin;
				BOOL m_isFixedType;
};

#endif //SINKFILTERGRAPH_DSWRAPPER_INCLUDED

