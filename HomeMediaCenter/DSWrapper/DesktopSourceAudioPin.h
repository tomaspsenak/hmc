#pragma once

#if !defined(DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED)
#define DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED

#include <Audioclient.h>

private class DesktopSourceAudioPin  : public CSourceStream
{
	public:		DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName);
				~DesktopSourceAudioPin(void);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q)
				{
					return E_FAIL;
				}

				HRESULT OnThreadCreate(void);

				HRESULT OnThreadDestroy(void);

	private:	DWORD m_iFrameNumber;
				const REFERENCE_TIME m_rtFrameLength;
				IAudioClient * m_audioClient;
				IAudioCaptureClient * m_captureClient;
				WORD m_blockAlign;
				DWORD m_nextTick;
				CCritSec m_cSharedState;
				DWORD m_lastSync;
};

#endif //DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED

