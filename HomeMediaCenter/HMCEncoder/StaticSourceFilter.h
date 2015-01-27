#pragma once

/*
     Copyright (C) 2015 Tomáš Pšenák
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

#if !defined(STATICSOURCEFILTER_HMCENCODER_INCLUDED)
#define STATICSOURCEFILTER_HMCENCODER_INCLUDED

#include "StaticSourceParameters.h"
#include "StaticSourcePins.h"

class StaticSourceFilter : public CSource, public IAMFilterMiscFlags, public ISpecifyPropertyPages, public IMediaSeeking
{
	friend class StaticSourceVideoPin;
	friend class StaticSourceAudioPin;
	friend class StaticSourceParameters;

	public:		StaticSourceFilter(LPUNKNOWN pUnk, HRESULT * phr);
				virtual ~StaticSourceFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

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

				//Hranicna hodnota pri synchronizacii audia s videom
				static const REFERENCE_TIME SyncThreshold = 10 * UNITS;

	private:	StaticSourceVideoPin * m_sourceVideoPin;
				StaticSourceAudioPin * m_sourceAudioPin;

				REFERENCE_TIME m_rtStop;

				StaticSourceParameters * m_params;
};

#endif //STATICSOURCEFILTER_HMCENCODER_INCLUDED
