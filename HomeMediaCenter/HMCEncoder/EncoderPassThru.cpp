#include "StdAfx.h"
#include "EncoderPassThru.h"
#include "EncoderAsyncInputPin.h"

EncoderPassThru::EncoderPassThru(const TCHAR * pName, LPUNKNOWN pUnk, HRESULT * phr, IPin * audioPin, IPin * videoPin) : 
	CPosPassThru(pName, pUnk, phr, videoPin), m_audioPin(audioPin)
{
}

EncoderPassThru::~EncoderPassThru(void)
{
}

HRESULT EncoderPassThru::GetPeer(IMediaPosition ** ppMP)
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

HRESULT EncoderPassThru::GetPeerSeeking(IMediaSeeking ** ppMS)
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

HRESULT EncoderPassThru::GetCurrentPosition(LONGLONG * pCurrent)
{
	if (((EncoderAsyncInputPin*)this->m_pPin)->GetCurrentPosition(pCurrent) == S_OK)
		return S_OK;

	return ((EncoderAsyncInputPin*)this->m_audioPin)->GetCurrentPosition(pCurrent);
}
