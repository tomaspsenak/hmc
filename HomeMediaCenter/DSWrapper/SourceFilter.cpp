#include "StdAfx.h"
#include "SourceFilter.h"
#include "SourcePin.h"

SourceFilter::SourceFilter(LPUNKNOWN pUnk, HRESULT * phr, System::IO::Stream ^ stream)
	: CSource(L"SourceFilter", pUnk, CLSID_NULL)
{
	SourcePin * sourcePin = new SourcePin(L"SourcePin", phr, this, L"Out", stream);
	if (sourcePin == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
}


SourceFilter::~SourceFilter(void)
{
}
