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

#if !defined(STATICSOURCEPARAMETERS_HMCENCODER_INCLUDED)
#define STATICSOURCEPARAMETERS_HMCENCODER_INCLUDED

class StaticSourceFilter;

class StaticSourceParameters : public CUnknown, public IHMCStaticSource
{
	friend class StaticSourceVideoPin;
	friend class StaticSourceAudioPin;

	public:		StaticSourceParameters(StaticSourceFilter * filter);
				virtual ~StaticSourceParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetFrameRate(UINT32 fps);
				STDMETHODIMP GetFrameRate(UINT32 * pFps);
				STDMETHODIMP SetBitmapData(BYTE * pData, UINT32 dataCount);

	private:	UINT32 m_fps;
				REFERENCE_TIME m_rtFrameLength;
				BITMAPINFOHEADER m_bitmapInfo;
				BYTE * m_bitmapData;
		
				StaticSourceFilter * m_filter;
};

#endif //STATICSOURCEPARAMETERS_HMCENCODER_INCLUDED
