#pragma once

#if !defined(FILEWRITERFILTER_DSWRAPPER_INCLUDED)
#define FILEWRITERFILTER_DSWRAPPER_INCLUDED

#include "FileWriterPin.h"

private class FileWriterFilter : public CBaseFilter, public IAMFilterMiscFlags
{
	public:		FileWriterFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream^ outputStream);
				~FileWriterFilter(void);

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

				void SetInputSubtype(GUID inputSubtype) { this->m_inputSubtype = inputSubtype; }
				GUID GetInputSubtype(void) { return this->m_inputSubtype; }

				//CBaseFilter
				int GetPinCount(void);
				CBasePin * GetPin(int n);

				//IAMFilterMiscFlags
				ULONG STDMETHODCALLTYPE GetMiscFlags(void);

	private:	CCritSec m_critSection;
				FileWriterPin * m_writerPin;
				GUID m_inputSubtype;
};

#endif //FILEWRITERFILTER_DSWRAPPER_INCLUDED

