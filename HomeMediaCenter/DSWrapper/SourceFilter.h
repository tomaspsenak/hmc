#pragma once

#if !defined(SOURCEFILTER_DSWRAPPER_INCLUDED)
#define SOURCEFILTER_DSWRAPPER_INCLUDED

private class SourceFilter : public CSource
{
	public:		SourceFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream ^ stream);
				~SourceFilter(void);
};

#endif //SOURCEFILTER_DSWRAPPER_INCLUDED