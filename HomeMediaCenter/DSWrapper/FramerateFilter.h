#pragma once

#if !defined(FRAMERATEFILTER_DSWRAPPER_INCLUDED)
#define FRAMERATEFILTER_DSWRAPPER_INCLUDED

private class FramerateFilter : public CTransformFilter
{
	public:		FramerateFilter(LPUNKNOWN punk, HRESULT * phr, UINT32 fps);
				~FramerateFilter(void);

				//CTransformFilter
				HRESULT CheckInputType(const CMediaType * mtIn);
				HRESULT CheckTransform(const CMediaType * mtIn, const CMediaType * mtOut);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties);
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
				HRESULT CompleteConnect(PIN_DIRECTION direction, IPin * pReceivePin);
				HRESULT Receive(IMediaSample * pSample);

	private:	HRESULT InitFrameArray(REFERENCE_TIME frameLength);

				BYTE * m_frameArray;
				UINT32 m_originFps;
				const UINT32 m_fps;
				DWORD m_iFrameNumber;
				DWORD m_iTransformFrame;
				const REFERENCE_TIME m_rtFrameLength;
				REFERENCE_TIME m_rtOriginFrameLength;
};

#endif //FRAMERATEFILTER_DSWRAPPER_INCLUDED