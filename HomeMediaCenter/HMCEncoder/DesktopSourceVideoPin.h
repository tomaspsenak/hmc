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

#if !defined(DESKTOPSOURCEVIDEOPIN_HMCENCODER_INCLUDED)
#define DESKTOPSOURCEVIDEOPIN_HMCENCODER_INCLUDED

class DesktopSourceFilter;

class DesktopSourceVideoPin : public CSourceStream
{
	public:		DesktopSourceVideoPin(TCHAR * pObjectName, HRESULT * phr, DesktopSourceFilter * filter, LPCWSTR pName);
				virtual ~DesktopSourceVideoPin(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT SetMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q)
				{
					return E_FAIL;
				}

				HRESULT ReconnectWithChangesSync(void);

	protected:	HRESULT DoBufferProcessingLoop(void);
				HRESULT ApplyParametersToMT(CMediaType * pmt);

	private:	static HRESULT GetWidthHeight(const AM_MEDIA_TYPE * type, LONG * width, LONG * height); 
				static HRESULT GetBitmapInfo(const AM_MEDIA_TYPE * type, BITMAPINFOHEADER * bitmapInfo);
				
				HBITMAP CopyScreenToBitmap(BYTE * pData, BITMAPINFO * pHeader, int stretchMode, BOOL captureCursor, BOOL keepAspectRatio);
		
				REFERENCE_TIME m_rtLastFrame;
				DesktopSourceFilter * m_pFilter;
				BITMAPINFO m_bitmapInfo;
				BITMAPINFO m_rgbBitmapInfo;
				BYTE * m_rgbBuffer;
};

#endif //DESKTOPSOURCEVIDEOPIN_HMCENCODER_INCLUDED

