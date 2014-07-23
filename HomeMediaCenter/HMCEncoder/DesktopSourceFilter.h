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

#if !defined(DESKTOPSOURCEFILTER_HMCENCODER_INCLUDED)
#define DESKTOPSOURCEFILTER_HMCENCODER_INCLUDED

#include "DesktopSourceVideoPin.h"
#include "DesktopSourceAudioPin.h"
#include "DesktopSourceParameters.h"

class DesktopSourceFilter : public CSource, public IAMFilterMiscFlags, public ISpecifyPropertyPages
{
	friend class DesktopSourceVideoPin;
	friend class DesktopSourceAudioPin;
	friend class DesktopSourceParameters;

	public:		DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr);
				~DesktopSourceFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

				void SyncPins(DWORD index);
				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);

				//Maximalna dlzka cakania na synchronizaciu
				static const DWORD WaitToSync = 10000;

	private:	DesktopSourceVideoPin * m_sourceVideoPin;
				DesktopSourceAudioPin * m_sourceAudioPin;
				CCritSec m_syncSection;
				HANDLE m_syncEvent[2];
				BOOL m_signaled[2];

				DesktopSourceParameters * m_params;
};

#endif //DESKTOPSOURCEFILTER_HMCENCODER_INCLUDED
