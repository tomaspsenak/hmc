// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

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
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <afxwin.h>
#include <dshow.h>
#include <streams.h>

#ifndef __HMCEncoder_h_h__
#include "HMCEncoder_h.h"
#endif

#define __STDC_CONSTANT_MACROS
#include <stdint.h>

extern "C" {
	#include "libavformat\avio.h"
	#include "libavutil\audio_fifo.h"
	#include "libavcodec\avcodec.h"
	#include "libavformat\avformat.h"
	#include "libswscale\swscale.h"
	#include "libswresample\swresample.h"
	#include "libavutil\opt.h"
};

#ifndef CHECK_HR
#define CHECK_HR(val) { if ( (val) != S_OK ) { goto done; } }
#endif

#ifndef SAFE_RELEASE
template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
#define SAFE_RELEASE(p) { SafeRelease(&(p)); }
#endif 

#ifndef CHECK_SUCCEED
#define CHECK_SUCCEED(val) { if ( (val) < 0 ) { goto done; } }
#endif

#ifndef SAFE_ADDREF
#define SAFE_ADDREF(val) { if ( (val) != NULL ) { val->AddRef(); } }
#endif