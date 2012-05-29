#pragma once

#if !defined(DESKTOPSILENCEGENERATOR_DSWRAPPER_INCLUDED)
#define DESKTOPSILENCEGENERATOR_DSWRAPPER_INCLUDED

#include <mmdeviceapi.h>

struct PlaySilenceArgs
{
    IMMDevice * device;
    HANDLE startedEvent;
    HANDLE stopEvent;
	HANDLE thread;
    HRESULT hr;

	PlaySilenceArgs() : startedEvent(NULL), stopEvent(NULL), thread(NULL) { }
};

DWORD WINAPI PlaySilenceFunction(LPVOID pContext);

#endif //DESKTOPSILENCEGENERATOR_DSWRAPPER_INCLUDED