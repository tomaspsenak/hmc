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

#if !defined(FRAMEWRITERPARAMETERS_HMCENCODER_INCLUDED)
#define FRAMEWRITERPARAMETERS_HMCENCODER_INCLUDED

#include "AVEncoder.h"

class FrameWriterFilter;

class FrameWriterParameters : public CUnknown, public IHMCFrameWriter
{
	friend class FrameWriterFilter;

	public:		FrameWriterParameters(FrameWriterFilter * filter);
				virtual ~FrameWriterParameters(void);

				DECLARE_IUNKNOWN

				STDMETHODIMP SetFormat(enum ImageFormat format);
				STDMETHODIMP GetFormat(enum ImageFormat * pFormat);

				STDMETHODIMP SetWidth(UINT32 width);
				STDMETHODIMP GetWidth(UINT32 * pWidth);

				STDMETHODIMP SetHeight(UINT32 height);
				STDMETHODIMP GetHeight(UINT32 * pHeight);

				STDMETHODIMP SetBitrate(int imageBitrate);
				STDMETHODIMP GetBitrate(int * pImageBitrate);

	private:	AVEncoderParameters m_params;
				FrameWriterFilter * m_filter;
				ImageFormat m_format;
};

#endif //FRAMEWRITERPARAMETERS_HMCENCODER_INCLUDED