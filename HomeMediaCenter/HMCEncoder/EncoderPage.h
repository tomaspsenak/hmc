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

#if !defined(ENCODERPAGE_HMCENCODER_INCLUDED)
#define ENCODERPAGE_HMCENCODER_INCLUDED

class EncoderPage : public CBasePropertyPage
{
	public:		EncoderPage(LPUNKNOWN punk, HRESULT * phr);
				virtual ~EncoderPage(void);
					
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
				void SetVBitrateStaticEnable(BOOL enable);
				void SetABitrateStaticEnable(BOOL enable);

				Container m_oldContainer;
				BOOL m_oldStreamable;
				BOOL m_oldInterlaced;

				BOOL m_oldVideoConstant;
				int m_oldVideoBitrate;
				int m_oldVideoBitrateMax;
				int m_oldVideoBitrateMin;
				int m_oldVideoQuality;
				UINT32 m_oldWidth;
				UINT32 m_oldHeight;

				BOOL m_oldAudioConstant;
				int m_oldAudioBitrate;
				int m_oldAudioBitrateMax;
				int m_oldAudioBitrateMin;
				int m_oldAudioQuality;

				IHMCEncoder * m_encoderParams;
};

#endif //ENCODERPAGE_HMCENCODER_INCLUDED
