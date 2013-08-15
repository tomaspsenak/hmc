#pragma once

#if !defined(WMPREFILTER_DSWRAPPER_INCLUDED)
#define WMPREFILTER_DSWRAPPER_INCLUDED

#include "PreFilter.h"

//WMVideoPreFilter nastavi biPlanes na 1 - ine hodnoty nie su podporovane cez WMAsfWriter (bug)
private class WMVideoPreFilter : public PreFilter
{
	public:		WMVideoPreFilter(LPUNKNOWN punk, HRESULT * phr) : PreFilter(punk, phr) { }
				virtual ~WMVideoPreFilter(void) { }

				//CTransformFilter
				HRESULT CheckInputType(const CMediaType * mtIn);
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
};

//WMAudioPreFilter nastavi wFormatTag na WAVE_FORMAT_PCM
private class WMAudioPreFilter : public PreFilter
{
	public:		WMAudioPreFilter(LPUNKNOWN punk, HRESULT * phr) : PreFilter(punk, phr) { }
				virtual ~WMAudioPreFilter(void) { }

				//CTransformFilter
				HRESULT CheckInputType(const CMediaType * mtIn);
				HRESULT GetMediaType(int iPosition, CMediaType * pMediaType);
};

#endif //WMPREFILTER_DSWRAPPER_INCLUDED
