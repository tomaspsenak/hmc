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

#if !defined(COLORCONVERTER_HMCENCODER_INCLUDED)
#define COLORCONVERTER_HMCENCODER_INCLUDED

void RGBtoYUY2(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer);

void RGBtoYV12(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer);

void RGBtoRGB(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer, BITMAPINFOHEADER & dstBmi);

#endif //COLORCONVERTER_HMCENCODER_INCLUDED