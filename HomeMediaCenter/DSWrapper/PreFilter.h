#pragma once

#if !defined(PREFILTER_DSWRAPPER_INCLUDED)
#define PREFILTER_DSWRAPPER_INCLUDED

private class PreOutputPin : public CTransInPlaceOutputPin
{
	public:		PreOutputPin(LPCTSTR pObjectName, CTransInPlaceFilter * pFilter, HRESULT * phr, LPCWSTR pName) :
					CTransInPlaceOutputPin(pObjectName, pFilter, phr, pName) { }
				virtual ~PreOutputPin(void) { }
		
				inline STDMETHODIMP EnumMediaTypes(IEnumMediaTypes ** ppEnum)
				{
					return CTransformOutputPin::EnumMediaTypes(ppEnum);
				}
};

private class PreInputPin : public CTransInPlaceInputPin
{
	public:		PreInputPin(LPCTSTR pObjectName, CTransInPlaceFilter * pFilter, HRESULT * phr, LPCWSTR pName) :
					CTransInPlaceInputPin(pObjectName, pFilter, phr, pName) { }
				virtual ~PreInputPin(void) { }

				STDMETHODIMP Receive(IMediaSample * pSample);
};

private class PreFilter : public CTransInPlaceFilter
{
	public:		PreFilter(LPUNKNOWN punk, HRESULT * phr);
				virtual ~PreFilter(void);

				//CTransInPlaceFilter
				CBasePin * GetPin(int n);
				inline HRESULT Transform(IMediaSample * pSample)
				{
					return S_OK;
				}
};

#endif //WMPREFILTER_DSWRAPPER_INCLUDED

