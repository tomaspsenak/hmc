#pragma once

#if !defined(DESKTOPSOURCEFILTER_DSWRAPPER_INCLUDED)
#define DESKTOPSOURCEFILTER_DSWRAPPER_INCLUDED

#include "DesktopSourcePin.h"
#include "DesktopSourceAudioPin.h"

private class DesktopSourceFilter : public CSource, public IAMFilterMiscFlags
{
	public:		DesktopSourceFilter(LPUNKNOWN pUnk, HRESULT * phr, UINT32 fps);
				~DesktopSourceFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				void SyncPins(DWORD index, REFERENCE_TIME & rtSyncTime);
				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);

				//Synchronizacia audia a videa v milisekundach, nedavat male cislo
				static const DWORD SyncTime = 5000;

	private:	DesktopSourcePin * m_sourcePin;
				DesktopSourceAudioPin * m_sourceAudioPin;
				CCritSec m_syncSection;
				REFERENCE_TIME m_rtLastFrame;
				BOOL m_isTimeSet;
};

#endif //DESKTOPSOURCEFILTER_DSWRAPPER_INCLUDED
