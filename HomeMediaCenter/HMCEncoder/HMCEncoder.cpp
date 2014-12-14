#include "StdAfx.h"
#include "EncoderFilter.h"
#include "EncoderPage.h"
#include "FrameRateFilter.h"
#include "FrameRatePage.h"
#include "DesktopSourceFilter.h"
#include "DesktopSourcePage.h"
#include "FrameWriterFilter.h"
#include "FrameWriterPage.h"
#include "EncoderLogPage.h"
#include "DesktopSourceSilenceGenerator.h"

STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	//Inicializacia silence generatora - automaticka inicializacia nie je bezpecna pri multithread
	DesktopSourceSilenceGenerator::Init();

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

const AMOVIESETUP_FILTER encoderFilter =
{
    &CLSID_HMCEncoder, L"HMCEncoderFilter", MERIT_DO_NOT_USE, 3, pins
};

const AMOVIESETUP_PIN frameRatePins[] =
{
    { L"Video Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL },
	{ L"Video Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL }
};

const AMOVIESETUP_FILTER frameRateFilter =
{
    &CLSID_HMCFrameRate, L"HMCFrameRateFilter", MERIT_DO_NOT_USE, 2, frameRatePins
};

const AMOVIESETUP_PIN desktopSourcePins[] =
{
    { L"Video Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL },
	{ L"Audio Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL }
};

const AMOVIESETUP_FILTER desktopSourceFilter =
{
    &CLSID_HMCDesktopSource, L"HMCDesktopSourceFilter", MERIT_DO_NOT_USE, 2, desktopSourcePins
};

const AMOVIESETUP_PIN frameWriterPins[] =
{
    { L"Video Input", TRUE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL }
};

const AMOVIESETUP_FILTER frameWriterFilter =
{
    &CLSID_HMCFrameWriter, L"HMCFrameWriterFilter", MERIT_DO_NOT_USE, 1, frameWriterPins
};

CFactoryTemplate g_Templates[] =
{
    { L"HMCEncoderFilter", &CLSID_HMCEncoder, EncoderFilter::CreateInstance, NULL, &encoderFilter },
	{ L"HMCEncoderFilterPage", &CLSID_HMCEncoderPage, EncoderPage::CreateInstance },
	{ L"HMCEncoderFilterLogPage", &CLSID_HMCEncoderLogPage, EncoderLogPage::CreateInstance },
	{ L"HMCFrameRateFilter", &CLSID_HMCFrameRate, FrameRateFilter::CreateInstance, NULL, &frameRateFilter },
	{ L"HMCFrameRateFilterPage", &CLSID_HMCFrameRatePage, FrameRatePage::CreateInstance },
	{ L"HMCDesktopSourceFilter", &CLSID_HMCDesktopSource, DesktopSourceFilter::CreateInstance, NULL, &desktopSourceFilter },
	{ L"HMCDesktopSourceFilterPage", &CLSID_HMCDesktopSourcePage, DesktopSourcePage::CreateInstance },
	{ L"HMCFrameWriterFilter", &CLSID_HMCFrameWriter, FrameWriterFilter::CreateInstance, NULL, &frameWriterFilter },
	{ L"HMCFrameWriterFilterPage", &CLSID_HMCFrameWriterPage, FrameWriterPage::CreateInstance }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);