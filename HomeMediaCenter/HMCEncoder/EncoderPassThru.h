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

#if !defined(ENCODERPASSTHRU_HMCENCODER_INCLUDED)
#define ENCODERPASSTHRU_HMCENCODER_INCLUDED

class EncoderPassThru : public CPosPassThru
{
	public:		EncoderPassThru(const TCHAR * pName, LPUNKNOWN pUnk, HRESULT * phr, IPin * audioPin, IPin * videoPin);
				virtual ~EncoderPassThru(void);

				HRESULT GetPeer(IMediaPosition ** ppMP);
				HRESULT GetPeerSeeking(IMediaSeeking ** ppMS);

				STDMETHODIMP GetCurrentPosition(LONGLONG * pCurrent);

	protected:	IPin * m_audioPin;
};

#endif //ENCODERPASSTHRU_HMCENCODER_INCLUDED
