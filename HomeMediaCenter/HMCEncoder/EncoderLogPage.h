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

#if !defined(ENCODERLOGPAGE_HMCENCODER_INCLUDED)
#define ENCODERLOGPAGE_HMCENCODER_INCLUDED

class EncoderLogPage : public CBasePropertyPage
{
	public:		EncoderLogPage(LPUNKNOWN punk, HRESULT * phr);
				virtual ~EncoderLogPage(void);
					
				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//CBasePropertyPage 
				HRESULT OnActivate(void);
				HRESULT OnDeactivate(void);
				INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:	static void LogCallback(void * avcl, int level, const char * fmt, va_list vl);
				static HWND pageHandle;
				static CCritSec * handleLock;

				int m_maxWidth;
};

#endif //ENCODERLOGPAGE_HMCENCODER_INCLUDED