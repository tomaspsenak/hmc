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

#if !defined(ASYNCINPUTPIN_HMCENCODER_INCLUDED)
#define ASYNCINPUTPIN_HMCENCODER_INCLUDED

class HMCFilter;

class AsyncInputPin : public CBaseInputPin, public CAMThread
{
	public:		AsyncInputPin(LPCTSTR pObjectName, HMCFilter * pFilter, CCritSec * pLock, HRESULT * phr);
				virtual ~AsyncInputPin(void);

				virtual STDMETHODIMP ReceiveAsync(BYTE * buffer, long length) = 0;
				virtual STDMETHODIMP EndOfStreamAsync(void) = 0;

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

	protected:	HMCFilter * m_pFilter;

	private:	BYTE * m_deliverBuffer;
				long m_deliverBufferLen;
				IMediaSample * m_tempSample;
		
				enum Command { CMD_STOP, CMD_RECEIVE, CMD_EOS };
};

#endif //ASYNCINPUTPIN_HMCENCODER_INCLUDED

