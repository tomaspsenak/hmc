#pragma once

#if !defined(DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED)
#define DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED

#include <Audioclient.h>
#include <mmdeviceapi.h>
#include "DesktopSilenceGenerator.h"

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

	private:	REFERENCE_TIME m_rtLastFrame;
				const REFERENCE_TIME m_rtFrameLength;
				IMMDevice * m_device;
				IAudioClient * m_audioClient;
				IAudioCaptureClient * m_captureClient;
				PlaySilenceArgs m_silenceArgs;
				CCritSec m_cSharedState;
				WORD m_blockAlign;
				BYTE * m_buffer;
				DWORD m_cBufferData;
};

#endif //DESKTOPSOURCEAUDIOPIN_DSWRAPPER_INCLUDED

