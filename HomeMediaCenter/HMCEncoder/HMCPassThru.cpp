#include "StdAfx.h"
#include "HMCPassThru.h"
#include "AsyncInputPin.h"

HMCPassThru::HMCPassThru(const TCHAR * pName, LPUNKNOWN pUnk, HRESULT * phr, IPin * audioPin, IPin * videoPin) : 
	CPosPassThru(pName, pUnk, phr, videoPin), m_audioPin(audioPin)
{
}

HMCPassThru::~HMCPassThru(void)
{
}

HRESULT HMCPassThru::GetPeer(IMediaPosition ** ppMP)
{
    *ppMP = NULL;

    IPin * pConnected;
    HRESULT hr = this->m_pPin->ConnectedTo(&pConnected);
    if (FAILED(hr))
	{
		hr = this->m_audioPin->ConnectedTo(&pConnected);
		if (FAILED(hr))
			return E_NOTIMPL;
    }

    IMediaPosition * pMP;
    hr = pConnected->QueryInterface(IID_IMediaPosition, (void **)&pMP);
    pConnected->Release();
    if (FAILED(hr))
		return E_NOTIMPL;

    *ppMP = pMP;
    return S_OK;
}

HRESULT HMCPassThru::GetPeerSeeking(IMediaSeeking ** ppMS)
{
    *ppMS = NULL;

    IPin * pConnected;
    HRESULT hr = this->m_pPin->ConnectedTo(&pConnected);
    if (FAILED(hr))
	{
		hr = this->m_audioPin->ConnectedTo(&pConnected);
		if (FAILED(hr))
			return E_NOTIMPL;
    }

    IMediaSeeking * pMS;
    hr = pConnected->QueryInterface(IID_IMediaSeeking, (void **) &pMS);
    pConnected->Release();
    if (FAILED(hr))
		return E_NOTIMPL;

    *ppMS = pMS;
    return S_OK;
}

HRESULT HMCPassThru::GetCurrentPosition(LONGLONG * pCurrent)
{
	if (((AsyncInputPin*)this->m_pPin)->GetCurrentPosition(pCurrent) == S_OK)
		return S_OK;

	return ((AsyncInputPin*)this->m_audioPin)->GetCurrentPosition(pCurrent);
}
