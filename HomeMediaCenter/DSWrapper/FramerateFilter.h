#pragma once

#if !defined(FRAMERATEFILTER_DSWRAPPER_INCLUDED)
#define FRAMERATEFILTER_DSWRAPPER_INCLUDED

private class FramerateOutputPin : public CTransInPlaceOutputPin
{
	public:		FramerateOutputPin(LPCTSTR pObjectName, CTransInPlaceFilter * pFilter, HRESULT * phr, LPCWSTR pName);
				virtual ~FramerateOutputPin(void);
		
				STDMETHODIMP EnumMediaTypes(IEnumMediaTypes ** ppEnum)
				{
					return CTransformOutputPin::EnumMediaTypes(ppEnum);
				}
};

private class FramerateFilter : public CTransInPlaceFilter
{
	public:		FramerateFilter(LPUNKNOWN punk, HRESULT * phr, UINT32 fps);
				virtual ~FramerateFilter(void);

				//CTransInPlaceFilter
				CBasePin * GetPin(int n);
				HRESULT Transform(IMediaSample * pSample);

				//CTransformFilter
				HRESULT CheckInputType(const CMediaType * mtIn);
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
				HRESULT CompleteConnect(PIN_DIRECTION direction, IPin * pReceivePin);
				HRESULT Receive(IMediaSample * pSample);

	private:	UINT32 m_fpsModSum;
				const UINT32 m_fpsMod;
				const UINT32 m_fps;
				const REFERENCE_TIME m_rtFrameLength;
				REFERENCE_TIME m_rtOriginFrameLength;
				REFERENCE_TIME m_rtLastFrame;
};

#endif //FRAMERATEFILTER_DSWRAPPER_INCLUDED