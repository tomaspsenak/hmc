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

#if !defined(STATICSOURCEPINS_HMCENCODER_INCLUDED)
#define STATICSOURCEPINS_HMCENCODER_INCLUDED

class StaticSourceFilter;

class StaticSourceAudioPin  : public CSourceStream
{
	public:		StaticSourceAudioPin(TCHAR * pObjectName, HRESULT * phr, StaticSourceFilter * filter, LPCWSTR pName);
				virtual ~StaticSourceAudioPin(void);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				HRESULT Active(void) { m_rtLastFrame = 0; m_isStopping = FALSE; return CSourceStream::Active(); }
				HRESULT Inactive(void) { m_isStopping = TRUE; return CSourceStream::Inactive();  }

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q) { return E_FAIL; }

				REFERENCE_TIME GetLastFrameTime(void) { return m_rtLastFrame; }

	private:	StaticSourceFilter * m_pFilter;
				REFERENCE_TIME m_rtLastFrame;
				BOOL m_isStopping;
};

class StaticSourceVideoPin  : public CSourceStream
{
	public:		StaticSourceVideoPin(TCHAR * pObjectName, HRESULT * phr, StaticSourceFilter * filter, LPCWSTR pName);
				virtual ~StaticSourceVideoPin(void);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				HRESULT Active(void) { m_rtLastFrame = 0; return CSourceStream::Active(); }
				HRESULT Inactive(void) { return CSourceStream::Inactive();  }

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q) { return E_FAIL; }

				REFERENCE_TIME GetLastFrameTime(void) { return m_rtLastFrame; }
				HRESULT ReconnectWithChangesSync(void);

	private:	HRESULT ApplyParametersToMT(CMediaType * pmt);
		
				StaticSourceFilter * m_pFilter;
				REFERENCE_TIME m_rtLastFrame;
};

#endif //STATICSOURCEPINS_HMCENCODER_INCLUDED
