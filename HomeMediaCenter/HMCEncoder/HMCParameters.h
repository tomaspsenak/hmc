#pragma once

/*
     Copyright (C) 2012 Tomáš Pšenák
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

#if !defined(HMCPARAMETERS_HMCENCODER_INCLUDED)
#define HMCPARAMETERS_HMCENCODER_INCLUDED

class HMCParameters : public CUnknown, public IHMCEncoder
{
	friend class AVEncoder;

	public:		HMCParameters(LPUNKNOWN pUnk);
				virtual ~HMCParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetContainer(enum Container container);
				STDMETHODIMP GetContainer(enum Container * pContainer);
				STDMETHODIMP SetStreamable(BOOL streamable);
				STDMETHODIMP GetStreamable(BOOL * pStreamable);
				STDMETHODIMP SetAudioCBR(int audioBitrate);
				STDMETHODIMP SetAudioVBR(int audioBitrate, int audioBitrateMax, int audioBitrateMin, int audioQuality);
				STDMETHODIMP SetVideoCBR(int videoBitrate);
				STDMETHODIMP SetVideoVBR(int videoBitrate, int videoBitrateMax, int videoBitrateMin, int videoQuality);
				STDMETHODIMP SetVideoInterlace(BOOL interlaced);
				STDMETHODIMP GetVideoInterlace(BOOL * interlaced);
				STDMETHODIMP SetVideoBFrames(BOOL bFrames);
				STDMETHODIMP GetVideoBFrames(BOOL * bFrames);
				STDMETHODIMP SetVideoGopSize(int gopSize);
				STDMETHODIMP GetVideoGopSize(int * gopSize);

	private:	enum Container m_container;
				char m_containerStr[10];
				AVCodecID m_audioCodec;
				AVCodecID m_videoCodec;
				BOOL m_streamable;

				int m_audioBitrate;
				int m_audioBitrateMax;
				int m_audioBitrateMin;
				int m_audioSamplerate;
				int m_audioChannels;
				int m_audioQuality;
				enum BitrateMode m_audioMode;

				BOOL m_videoBFrames;
				int m_videoGopSize;
				int m_videoBitrate;
				int m_videoBitrateMax;
				int m_videoBitrateMin;
				int m_videoQuality;
				int m_videoBufferSize;
				enum BitrateMode m_videoMode;
				BOOL m_videoInterlaced;
};

#endif //HMCPARAMETERS_HMCENCODER_INCLUDED

