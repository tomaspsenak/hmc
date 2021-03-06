#pragma once

/*
     Copyright (C) 2015 Tom� P�en�k
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

class DesktopSourceFilter : public CSource, public IAMFilterMiscFlags, public ISpecifyPropertyPages, public IMediaSeeking
{
	friend class DesktopSourceVideoPin;
	friend class DesktopSourceAudioPin;
	friend class DesktopSourceParameters;

	public:		DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr);
				virtual ~DesktopSourceFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

				void SyncPins(DWORD index);
				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);

				//IMediaSeeking
				STDMETHODIMP GetCapabilities(DWORD * pCapabilities);
				STDMETHODIMP CheckCapabilities(DWORD * pCapabilities);
				STDMETHODIMP IsFormatSupported(const GUID * pFormat);
				STDMETHODIMP QueryPreferredFormat(GUID * pFormat);
				STDMETHODIMP GetTimeFormat(GUID * pFormat);
				STDMETHODIMP IsUsingTimeFormat(const GUID * pFormat);
				STDMETHODIMP SetTimeFormat(const GUID * pFormat);
				STDMETHODIMP GetDuration(LONGLONG * pDuration);
				STDMETHODIMP GetStopPosition(LONGLONG * pStop);
				STDMETHODIMP GetCurrentPosition(LONGLONG * pCurrent);
				STDMETHODIMP ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat);
				STDMETHODIMP SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags, LONGLONG * pStop, DWORD dwStopFlags);
				STDMETHODIMP GetPositions(LONGLONG * pCurrent, LONGLONG * pStop);
				STDMETHODIMP GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest);
				STDMETHODIMP SetRate(double dRate);
				STDMETHODIMP GetRate(double * pdRate);
				STDMETHODIMP GetPreroll(LONGLONG* pllPreroll);

				//Maximalna dlzka cakania na synchronizaciu
				static const DWORD WaitToSync = 10000;

	private:	DesktopSourceVideoPin * m_sourceVideoPin;
				DesktopSourceAudioPin * m_sourceAudioPin;
				CCritSec m_syncSection;
				HANDLE m_syncEvent[2];
				BOOL m_signaled[2];

				REFERENCE_TIME m_rtStop;

				DesktopSourceParameters * m_params;
};

#endif //DESKTOPSOURCEFILTER_HMCENCODER_INCLUDED
