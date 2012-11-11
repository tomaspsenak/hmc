#pragma once

/*
     Copyright (C) 2012 Tom� P�en�k
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

#if !defined(HMCPASSTHRU_HMCENCODER_INCLUDED)
#define HMCPASSTHRU_HMCENCODER_INCLUDED

class HMCPassThru : public CPosPassThru
{
	public:		HMCPassThru(const TCHAR * pName, LPUNKNOWN pUnk, HRESULT * phr, IPin * audioPin, IPin * videoPin);
				virtual ~HMCPassThru(void);

				HRESULT GetPeer(IMediaPosition ** ppMP);
				HRESULT GetPeerSeeking(IMediaSeeking ** ppMS);

	protected:	IPin * m_audioPin;
};

#endif //HMCPASSTHRU_HMCENCODER_INCLUDED
