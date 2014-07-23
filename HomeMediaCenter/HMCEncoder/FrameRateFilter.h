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

#if !defined(FRAMERATEFILTER_HMCENCODER_INCLUDED)
#define FRAMERATEFILTER_HMCENCODER_INCLUDED

#include "FrameRateParameters.h"

class FrameRateFilter;

class FrameRateInputPin : public CTransInPlaceInputPin
{
	public:		FrameRateInputPin(LPCTSTR pObjectName, FrameRateFilter * pFilter, HRESULT * phr, LPCWSTR pName);
				virtual ~FrameRateInputPin(void);

				HRESULT CheckMediaType(const CMediaType * pmt);

	private:	FrameRateFilter * m_pFilter;
};

class FrameRateOutputPin : public CTransInPlaceOutputPin
{
	public:		FrameRateOutputPin(LPCTSTR pObjectName, FrameRateFilter * pFilter, HRESULT * phr, LPCWSTR pName);
				virtual ~FrameRateOutputPin(void);
		
				STDMETHODIMP EnumMediaTypes(IEnumMediaTypes ** ppEnum);
};

class FrameRateFilter : public CTransInPlaceFilter, public ISpecifyPropertyPages
{
	friend class FrameRateInputPin;
	friend class FrameRateOutputPin;
	friend class FrameRateParameters;

	public:		FrameRateFilter(LPUNKNOWN punk, HRESULT * phr);
				virtual ~FrameRateFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

				//CTransInPlaceFilter
				CBasePin * GetPin(int n);
				HRESULT Transform(IMediaSample * pSample);

				//CTransformFilter
				HRESULT StopStreaming(void);
				HRESULT CheckInputType(const CMediaType * mtIn);
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
				HRESULT CompleteConnect(PIN_DIRECTION direction, IPin * pReceivePin);
				HRESULT Receive(IMediaSample * pSample);

	private:	HRESULT ReconnectPinSync(CBasePin * pin, CBasePin * mediaTypePin, REFERENCE_TIME avgTimePerFrame);
		
				UINT32 m_fpsModSum;
				REFERENCE_TIME m_rtOriginFrameLength;
				REFERENCE_TIME m_rtLastFrame;
				FrameRateParameters * m_params;
};

#endif //FRAMERATEFILTER_HMCENCODER_INCLUDED