// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include <Unknwn.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Mferror.h>
#include <shlwapi.h>
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

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(p) { delete [] (p); (p) = NULL; }
#endif

#ifndef GETMIN
template <class T> T GetMin(T x, T y)
{
    return (x < y) ? x : y;
}
#define GETMIN
#endif
