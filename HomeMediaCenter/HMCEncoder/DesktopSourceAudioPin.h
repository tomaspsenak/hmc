#pragma once

/*
     Copyright (C) 2014 Tomáš Pšenák
     This program is free software; you can redistribute it and/or modify 
     it under the terms of the GNU General Public License version 2 as 
     published by the Free Software Foundation.
 
     This program is distributed in the hope that it will be useful, but 
     WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
     or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
     for more details.
 
     You should have received a copy of the GNU General Public License along 
     with this program; if not, write to the Free Software Foundation, Inc., 
     51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#if !defined(DESKTOPSOURCEAUDIOPIN_HMCENCODER_INCLUDED)
#define DESKTOPSOURCEAUDIOPIN_HMCENCODER_INCLUDED

#include <Audioclient.h>
#include <mmdeviceapi.h>

class DesktopSourceFilter;

class DesktopSourceAudioPin  : public CSourceStream
{
	public:		DesktopSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, DesktopSourceFilter * filter, LPCWSTR pName);
				~DesktopSourceAudioPin(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

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

	private:	static DWORD WINAPI PlaySilenceFunction(LPVOID pContext);
		
				DWORD m_sleepTime;
				REFERENCE_TIME m_rtLastFrame;
				const REFERENCE_TIME m_rtFrameLength;
				IMMDevice * m_device;
				IAudioClient * m_audioClient;
				IAudioCaptureClient * m_captureClient;
				CCritSec m_cSharedState;
				WORD m_blockAlign;
				BYTE * m_buffer;
				DWORD m_cBufferData;
				DesktopSourceFilter * m_pFilter;

				HANDLE m_silenceStartedEvent;
				HANDLE m_silenceStopEvent;
				HANDLE m_silenceThread;
				HRESULT m_silenceHr;
};

#endif //DESKTOPSOURCEAUDIOPIN_HMCENCODER_INCLUDED

