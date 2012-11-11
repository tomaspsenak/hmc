#include "StdAfx.h"
#include "HMCFilter.h"

HMCFilter::HMCFilter(LPUNKNOWN pUnk, HRESULT * phr) : CBaseFilter(L"HMCFilter", pUnk, &m_critSection, CLSID_HMCEncoder), m_passThru(NULL),
	m_videoPin(NULL), m_audioPin(NULL), m_streamPin(NULL), m_encoder(NULL), m_params(NULL), m_audioEOS(false), m_videoEOS(false)
{
	this->m_videoPin = new VideoInputPin(this, &this->m_critSection, phr);
	if (this->m_videoPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_audioPin = new AudioInputPin(this, &this->m_critSection, phr);
	if (this->m_audioPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_streamPin = new StreamOutputPin(this, &this->m_critSection, phr);
	if (this->m_streamPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_encoder = new AVEncoder();
	if (this->m_encoder == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	this->m_params = new HMCParameters(GetOwner());
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	this->m_passThru = new HMCPassThru(L"HMCPassThru COM Object", GetOwner(), phr, this->m_audioPin, this->m_videoPin);
	if (this->m_passThru == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}


HMCFilter::~HMCFilter(void)
{
	if (this->m_videoPin != NULL)
		delete this->m_videoPin;
	if (this->m_audioPin != NULL)
		delete this->m_audioPin;
	if (this->m_streamPin != NULL)
		delete this->m_streamPin;
	if (this->m_encoder != NULL)
		delete this->m_encoder;
	if (this->m_params != NULL)
		delete this->m_params;
	if (this->m_passThru != NULL)
		delete this->m_passThru;
}

STDMETHODIMP HMCFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCEncoder)
	{
		return GetInterface(this->m_params, ppv);
	}
	else if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking) 
    {
		return this->m_passThru->NonDelegatingQueryInterface(riid, ppv);
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

//*************** CBaseFilter ***************\\

int HMCFilter::GetPinCount(void)
{
	return 3;
}

CBasePin * HMCFilter::GetPin(int n)
{
	if (n == 0)
		return this->m_videoPin;
	else if (n == 1)
		return this->m_audioPin;
	else if (n == 2)
		return this->m_streamPin;

    return NULL;
}

HRESULT HMCFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cAutoLock(this->m_pLock);

	HRESULT hr = S_OK;
	bool hasAudio, hasVideo;
	AM_MEDIA_TYPE amt, vmt;
	ZeroMemory(&amt, sizeof(AM_MEDIA_TYPE));
	ZeroMemory(&vmt, sizeof(AM_MEDIA_TYPE));

	if (this->m_audioPin->ConnectionMediaType(&amt) == S_OK)
	{
		hasAudio = true;
		this->m_audioEOS = false;
	}
	else
	{
		hasAudio = false;
		this->m_audioEOS = true;
	}

	if (this->m_videoPin->ConnectionMediaType(&vmt) == S_OK)
	{
		hasVideo = true;
		this->m_videoEOS = false;
	}
	else
	{
		hasVideo = false;
		this->m_videoEOS = true;
	}

	CHECK_HR(hr = this->m_encoder->Start(this->m_streamPin, this->m_params, (hasAudio) ? &amt : NULL, (hasVideo) ? &vmt : NULL));

	CHECK_HR(hr = CBaseFilter::Run(tStart));

done:

	FreeMediaType(amt);
	FreeMediaType(vmt);

	return hr;
}

HRESULT HMCFilter::Stop(void)
{
	this->m_encoder->Stop(FALSE);

	return CBaseFilter::Stop();
}

//*************** HMCFilter ****************\\

CUnknown * WINAPI HMCFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    HMCFilter * filter = new HMCFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}