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

#if !defined(ENCODERFILTER_HMCENCODER_INCLUDED)
#define ENCODERFILTER_HMCENCODER_INCLUDED

#include "EncoderVideoInputPin.h"
#include "EncoderAudioInputPin.h"
#include "EncoderStreamOutputPin.h"
#include "EncoderParameters.h"
#include "AVEncoder.h"
#include "EncoderPassThru.h"

class EncoderFilter : public CBaseFilter, public ISpecifyPropertyPages
{
	friend class EncoderVideoInputPin;
	friend class EncoderAudioInputPin;
	friend class EncoderStreamOutputPin;
	friend class EncoderParameters;

	public:		EncoderFilter(LPUNKNOWN pUnk, HRESULT * phr);
				virtual ~EncoderFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

				//CBaseFilter
				int GetPinCount(void);
				CBasePin * GetPin(int n);
				STDMETHODIMP Run(REFERENCE_TIME tStart);
				STDMETHODIMP Stop(void);

				static const long OutBufferSize = 65536;

	private:	CCritSec m_critSection;
				EncoderVideoInputPin * m_videoPin;
				EncoderAudioInputPin * m_audioPin;
				EncoderStreamOutputPin * m_streamPin;
				EncoderParameters * m_params;
				AVEncoder * m_encoder;
				EncoderPassThru * m_passThru;

				bool m_audioEOS;
				bool m_videoEOS;
};

#endif //ENCODERFILTER_HMCENCODER_INCLUDED

