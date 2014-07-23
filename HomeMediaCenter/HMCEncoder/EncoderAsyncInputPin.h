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

#if !defined(ENCODERASYNCINPUTPIN_HMCENCODER_INCLUDED)
#define ENCODERASYNCINPUTPIN_HMCENCODER_INCLUDED

class EncoderFilter;

class EncoderAsyncInputPin : public CBaseInputPin, public CAMThread
{
	public:		EncoderAsyncInputPin(LPCTSTR pObjectName, EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr);
				virtual ~EncoderAsyncInputPin(void);

				virtual STDMETHODIMP ReceiveAsync(BYTE * buffer, long length) = 0;
				virtual STDMETHODIMP EndOfStreamAsync(void) = 0;

				HRESULT GetCurrentPosition(LONGLONG * pCurrent);

				//CBaseInputPin
				HRESULT Active(void);
				HRESULT Inactive(void);

				//IPin
				STDMETHODIMP BeginFlush(void);
				STDMETHODIMP EndFlush(void);
				STDMETHODIMP EndOfStream(void);

				//IMemInputPin
				STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);
				STDMETHODIMP Receive(IMediaSample * pSample);

				//CAMThread
				DWORD ThreadProc(void);

	protected:	EncoderFilter * m_pFilter;

	private:	BYTE * m_deliverBuffer;
				long m_deliverBufferLen;
				IMediaSample * m_tempSample;
				REFERENCE_TIME m_time;
		
				enum Command { CMD_STOP, CMD_RECEIVE, CMD_EOS };
};

#endif //ENCODERASYNCINPUTPIN_HMCENCODER_INCLUDED

