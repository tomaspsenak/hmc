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

#if !defined(AVENCODER_HMCENCODER_INCLUDED)
#define AVENCODER_HMCENCODER_INCLUDED

#include "StreamOutputPin.h"
#include "HMCParameters.h"

class StreamOutputPin;

class AVEncoder
{
	public:		AVEncoder(void);
				virtual ~AVEncoder(void);

				HRESULT Start(StreamOutputPin * streamPin, HMCParameters * params, AM_MEDIA_TYPE * audioMT, AM_MEDIA_TYPE * videoMT);
				HRESULT Stop(BOOL isEOS);

				HRESULT EncodeAudio(BYTE * buffer, long length);
				HRESULT EncodeVideo(BYTE * buffer, long length);

	private:	AVStream * AddAudio(HMCParameters * params, AM_MEDIA_TYPE * audioMT);
				AVStream * AddVideo(HMCParameters * params, AM_MEDIA_TYPE * videoMT);
				HRESULT FreeContext(void);
				static AVFrame * CreateFrame(PixelFormat pix_fmt, int width, int height);
				static HRESULT CheckStream(AVStream* & stream, CCritSec & streamLock);
				static int FreeInterleavePackets(AVFormatContext * s);
		
				AVFormatContext * m_formatContext;
				AVStream * m_audioStream;
				AVStream * m_videoStream;
				BOOL m_isStopped;

				ReSampleContext * m_audioResample;
				int m_audioInChannels;
				uint8_t * m_audioInBuf;
				uint8_t * m_audioOutBuf;
				int m_audioOutBufSize;
				AVFifoBuffer * m_audioFIFO;
				AVFrame * m_audioFrame;

				PixelFormat m_pictureFormat;
				SwsContext * m_pictureContext;
				AVFrame * m_pictureInFrame;
				AVFrame * m_pictureOutFrame;
				int m_videoOutBufSize;
				uint8_t * m_videoOutBuf;

				CCritSec m_audioLock;
				CCritSec m_videoLock;
				CCritSec m_mainLock;
};

#endif //AVENCODER_HMCENCODER_INCLUDED

