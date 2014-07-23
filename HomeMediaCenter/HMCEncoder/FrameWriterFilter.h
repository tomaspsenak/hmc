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

#if !defined(FRAMEWRITERFILTER_HMCENCODER_INCLUDED)
#define FRAMEWRITERFILTER_HMCENCODER_INCLUDED

#include "AVEncoder.h"
#include "FrameWriterParameters.h"

class FrameWriterFilter : public CBaseRenderer, public ISpecifyPropertyPages, public IFileSinkFilter
{
	friend class FrameWriterParameters;

	public:		FrameWriterFilter(LPUNKNOWN punk, HRESULT * phr);
				virtual ~FrameWriterFilter(void);
				
				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//ISpecifyPropertyPages
				STDMETHODIMP GetPages(CAUUID * pPages);

				//IFileSinkFilter
				STDMETHODIMP GetCurFile(LPOLESTR * ppszFileName, AM_MEDIA_TYPE * pmt);
				STDMETHODIMP SetFileName(LPCOLESTR pszFileName, const AM_MEDIA_TYPE * pmt);

				//CBaseRenderer
				STDMETHODIMP Run(REFERENCE_TIME tStart);
				STDMETHODIMP Stop(void);
				HRESULT CheckMediaType(const CMediaType * pmt);
				HRESULT DoRenderSample(IMediaSample * pMediaSample);

				static const long OutBufferSize = 65536;

	private:	static int WritePackets(void * opaque, uint8_t * buf, int buf_size);
				static int ReadPackets(void * opaque, uint8_t * buf, int buf_size);
				static int64_t Seek(void * opaque, int64_t offset, int whence);

				FrameWriterParameters * m_params;
				AVEncoder * m_encoder;
				BOOL m_firstSample;

				HANDLE m_hFile;
				LPOLESTR m_fileName;
};

#endif //FRAMEWRITERFILTER_HMCENCODER_INCLUDED