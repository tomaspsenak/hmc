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

#if !defined(ENCODERAUDIOINPUTPIN_HMCENCODER_INCLUDED)
#define ENCODERAUDIOINPUTPIN_HMCENCODER_INCLUDED

#include "EncoderFilter.h"
#include "EncoderAsyncInputPin.h"

class EncoderAudioInputPin : public EncoderAsyncInputPin
{
	public:		EncoderAudioInputPin(EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr);
				virtual ~EncoderAudioInputPin(void);
				
				//CBasePin
				HRESULT CheckMediaType(const CMediaType * pmt);

				//EncoderAsyncInputPin
				STDMETHODIMP EndOfStreamAsync(void);
				STDMETHODIMP ReceiveAsync(BYTE * buffer, long length);
};

#endif //ENCODERAUDIOINPUTPIN_HMCENCODER_INCLUDED

