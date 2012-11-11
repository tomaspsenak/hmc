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

#if !defined(HMCFILTER_HMCENCODER_INCLUDED)
#define HMCFILTER_HMCENCODER_INCLUDED

#include "VideoInputPin.h"
#include "AudioInputPin.h"
#include "StreamOutputPin.h"
#include "HMCParameters.h"
#include "AVEncoder.h"
#include "HMCPassThru.h"

class AVEncoder;

class HMCFilter : public CBaseFilter
{
	friend class VideoInputPin;
	friend class AudioInputPin;
	friend class StreamOutputPin;

	public:		HMCFilter(LPUNKNOWN pUnk, HRESULT * phr);
				virtual ~HMCFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//CBaseFilter
				int GetPinCount(void);
				CBasePin * GetPin(int n);
				STDMETHODIMP Run(REFERENCE_TIME tStart);
				STDMETHODIMP Stop(void);

				static const long OutBufferSize = 65536;

	private:	CCritSec m_critSection;
				VideoInputPin * m_videoPin;
				AudioInputPin * m_audioPin;
				StreamOutputPin * m_streamPin;
				HMCParameters * m_params;
				AVEncoder * m_encoder;
				HMCPassThru * m_passThru;

				bool m_audioEOS;
				bool m_videoEOS;
};

#endif //HMCFILTER_HMCENCODER_INCLUDED

