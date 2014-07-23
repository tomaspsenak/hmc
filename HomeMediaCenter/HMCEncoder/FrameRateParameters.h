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

#if !defined(FRAMERATEPARAMETERS_HMCENCODER_INCLUDED)
#define FRAMERATEPARAMETERS_HMCENCODER_INCLUDED

class FrameRateFilter;

class FrameRateParameters : public CUnknown, public IHMCFrameRate
{
	friend class FrameRateFilter;

	public:		FrameRateParameters(FrameRateFilter * filter);
				virtual ~FrameRateParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetFrameRate(UINT32 fps);
				STDMETHODIMP GetFrameRate(UINT32 * pFps);

	private:	UINT32 m_fps;
				UINT32 m_fpsMod;
				REFERENCE_TIME m_rtFrameLength;

				FrameRateFilter * m_filter;
};

#endif //FRAMERATEPARAMETERS_HMCENCODER_INCLUDED

