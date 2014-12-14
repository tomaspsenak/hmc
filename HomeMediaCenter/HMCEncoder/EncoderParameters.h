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

#if !defined(ENCODERPARAMETERS_HMCENCODER_INCLUDED)
#define ENCODERPARAMETERS_HMCENCODER_INCLUDED

#include "AVEncoder.h"

class EncoderFilter;

class EncoderParameters : public CUnknown, public IHMCEncoder
{
	friend class EncoderFilter;

	public:		EncoderParameters(EncoderFilter * filter);
				virtual ~EncoderParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetContainer(enum Container container);
				STDMETHODIMP GetContainer(enum Container * pContainer);
				STDMETHODIMP SetStreamable(BOOL streamable);
				STDMETHODIMP GetStreamable(BOOL * pStreamable);
				STDMETHODIMP SetAudioCBR(int audioBitrate);
				STDMETHODIMP SetAudioVBR(int audioBitrate, int audioBitrateMax, int audioBitrateMin, int audioQuality);
				STDMETHODIMP GetAudioBitrate(BOOL * pIsConstant, int * pAudioBitrate, int * pAudioBitrateMax, int * pAudioBitrateMin, int * pAudioQuality);
				STDMETHODIMP SetVideoCBR(int videoBitrate);
				STDMETHODIMP SetVideoVBR(int videoBitrate, int videoBitrateMax, int videoBitrateMin, int videoQuality);
				STDMETHODIMP GetVideoBitrate(BOOL * pIsConstant, int * pVideoBitrate, int * pVideoBitrateMax, int * pVideoBitrateMin, int * pVideoQuality);
				STDMETHODIMP SetVideoInterlace(BOOL interlaced);
				STDMETHODIMP GetVideoInterlace(BOOL * interlaced);
				STDMETHODIMP SetWidth(UINT32 width);
				STDMETHODIMP GetWidth(UINT32 * pWidth);
				STDMETHODIMP SetHeight(UINT32 height);
				STDMETHODIMP GetHeight(UINT32 * pHeight);
				STDMETHODIMP SetVideoQuality(int videoQuality);
				STDMETHODIMP GetVideoQuality(int * pVideoQuality);
				STDMETHODIMP SetVideoBFrames(BOOL bFrames);
				STDMETHODIMP GetVideoBFrames(BOOL * bFrames);
				STDMETHODIMP SetVideoGopSize(int gopSize);
				STDMETHODIMP GetVideoGopSize(int * gopSize);

	private:	AVEncoderParameters m_params;
				EncoderFilter * m_filter;
				Container m_container;
};

#endif //ENCODERPARAMETERS_HMCENCODER_INCLUDED

