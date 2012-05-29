// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <dshow.h>
#include <streams.h>
#include <vcclr.h>

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

