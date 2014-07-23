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

#if !defined(DESKTOPSOURCEPARAMETERS_HMCENCODER_INCLUDED)
#define DESKTOPSOURCEPARAMETERS_HMCENCODER_INCLUDED

class DesktopSourceFilter;

class DesktopSourceParameters : public CUnknown, public IHMCDesktopSource
{
	friend class DesktopSourceVideoPin;

	public:		DesktopSourceParameters(DesktopSourceFilter * filter);
				virtual ~DesktopSourceParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetFrameRate(UINT32 fps);
				STDMETHODIMP GetFrameRate(UINT32 * pFps);

				STDMETHODIMP SetWidth(UINT32 width);
				STDMETHODIMP GetWidth(UINT32 * pWidth);

				STDMETHODIMP SetHeight(UINT32 height);
				STDMETHODIMP GetHeight(UINT32 * pHeight);

				STDMETHODIMP SetVideoQuality(BYTE quality);
				STDMETHODIMP GetVideoQuality(BYTE * pQuality);

				STDMETHODIMP SetCaptureCursor(BOOL captureCursor);
				STDMETHODIMP GetCaptureCursor(BOOL * pCaptureCursor);

				STDMETHODIMP SetAspectRatio(BOOL keepAspectRatio);
				STDMETHODIMP GetAspectRatio(BOOL * pKeepAspectRatio);

	private:	UINT32 m_fps;
				UINT32 m_width;
				UINT32 m_height;
				BYTE m_videoQuality;
				BOOL m_captureCursor;
				BOOL m_keepAspectRatio;

				int m_videoStretchMode;
				REFERENCE_TIME m_rtFrameLength;
		
				DesktopSourceFilter * m_filter;
};

#endif //DESKTOPSOURCEPARAMETERS_HMCENCODER_INCLUDED