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

#if !defined(DESKTOPSOURCEPAGE_HMCENCODER_INCLUDED)
#define DESKTOPSOURCEPAGE_HMCENCODER_INCLUDED

class DesktopSourcePage : public CBasePropertyPage
{
	public:		DesktopSourcePage(LPUNKNOWN punk, HRESULT * phr);
				virtual ~DesktopSourcePage(void);

				static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * phr);

				//CBasePropertyPage 
				HRESULT OnConnect(IUnknown * pUnknown);
				HRESULT OnDisconnect(void);
				HRESULT OnActivate(void);
				HRESULT OnApplyChanges(void);
				INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:	void SetDirty(void);
				int GetValue(int dlgItem);
				void RefreshValues(void);
				BYTE GetSliderPercentage(int dlgItem);
				BYTE GetSliderPosition(int dlgItem);
				BYTE SetSliderPosition(int dlgItem, BYTE percentage);
			
				UINT32 m_oldFps;
				UINT32 m_oldWidth;
				UINT32 m_oldHeight;
				BOOL m_oldKeepAspectRatio;
				BOOL m_oldCaptureCursor;
				BYTE m_oldVideoQualityPosition;
			
				IHMCDesktopSource * m_desktopSourceParams;
};

#endif //DESKTOPSOURCEPAGE_HMCENCODER_INCLUDED