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

#if !defined(ENCODERSTREAMOUTPUTPIN_HMCENCODER_INCLUDED)
#define ENCODERSTREAMOUTPUTPIN_HMCENCODER_INCLUDED

#include "EncoderFilter.h"

class EncoderStreamOutputPin : public CBaseOutputPin
{
	friend class EncoderFilter;

	public:		EncoderStreamOutputPin(EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr);
				virtual ~EncoderStreamOutputPin(void);

				//IPin
				STDMETHODIMP EndOfStream(void);

				//CBasePin
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
				HRESULT CheckMediaType(const CMediaType * pmt);

				//CBaseOutputPin
				HRESULT Active(void);
				HRESULT Inactive(void);
				HRESULT BreakConnect(void);
				HRESULT CompleteConnect(IPin * pReceivePin);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * ppropInputRequest);

	private:	static int WritePackets(void * opaque, uint8_t * buf, int buf_size);
				static int ReadPackets(void * opaque, uint8_t * buf, int buf_size);
				static int64_t Seek(void * opaque, int64_t offset, int whence);
		
				REFERENCE_TIME m_offset;
				EncoderFilter * m_pFilter;
				IStream * m_stream;
};

#endif //ENCODERSTREAMOUTPUTPIN_HMCENCODER_INCLUDED

