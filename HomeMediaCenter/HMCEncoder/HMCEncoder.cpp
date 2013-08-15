#include "StdAfx.h"
#include "HMCFilter.h"

STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

STDAPI CreateHMCFilter(void ** ppv)
{
	HRESULT hr = S_OK;

	CUnknown * unk = HMCFilter::CreateInstance(NULL, &hr);
	if (unk == NULL)
		return E_FAIL;

	unk->NonDelegatingAddRef();

	*ppv = unk;
	return hr;
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

const AMOVIESETUP_MEDIATYPE videoTypes[] =  
{
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_YV12 }
};

const AMOVIESETUP_MEDIATYPE audioTypes[] =  
{
    { &MEDIATYPE_Audio, &MEDIASUBTYPE_PCM }
};

const AMOVIESETUP_MEDIATYPE streamTypes[] =  
{
    { &MEDIATYPE_Stream, &MEDIASUBTYPE_NULL }
};

const AMOVIESETUP_PIN pins[] =
{
    { L"Video Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, 4, &videoTypes[0] },
    { L"Audio Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, 1, &audioTypes[0] },
	{ L"Stream Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, NULL, 1, &streamTypes[0] }
};

const AMOVIESETUP_FILTER filter =
{
    &CLSID_HMCEncoder, L"HMCFilter", MERIT_DO_NOT_USE, 3, pins
};                    

CFactoryTemplate g_Templates[] =
{
    { L"HMCFilter", &CLSID_HMCEncoder, HMCFilter::CreateInstance, NULL, &filter }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);