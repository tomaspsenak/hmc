#include "StdAfx.h"
#include "EncoderFilter.h"

EncoderFilter::EncoderFilter(LPUNKNOWN pUnk, HRESULT * phr) : CBaseFilter(L"HMCEncoderFilter", pUnk, &m_critSection, CLSID_HMCEncoder), 
	m_passThru(NULL), m_videoPin(NULL), m_audioPin(NULL), m_streamPin(NULL), m_encoder(NULL), m_params(NULL), m_audioEOS(false), m_videoEOS(false)
{
	this->m_videoPin = new EncoderVideoInputPin(this, &this->m_critSection, phr);
	if (this->m_videoPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_audioPin = new EncoderAudioInputPin(this, &this->m_critSection, phr);
	if (this->m_audioPin == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_streamPin = new EncoderStreamOutputPin(this, &this->m_critSection, phr);
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

	this->m_params = new EncoderParameters(this);
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	this->m_passThru = new EncoderPassThru(L"EncoderPassThru COM Object", GetOwner(), phr, this->m_audioPin, this->m_videoPin);
	if (this->m_passThru == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}


EncoderFilter::~EncoderFilter(void)
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

STDMETHODIMP EncoderFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCEncoder)
	{
		return GetInterface(this->m_params, ppv);
	}
	else if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking) 
    {
		return this->m_passThru->NonDelegatingQueryInterface(riid, ppv);
	}
	else if (riid == IID_ISpecifyPropertyPages)
	{
		return GetInterface((ISpecifyPropertyPages*) this, ppv);
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

//********** ISpecifyPropertyPages **********\\

STDMETHODIMP EncoderFilter::GetPages(CAUUID * pPages)
{
	CheckPointer(pPages, E_POINTER);

	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID) * 2);
	if (pPages->pElems == NULL)
	{
		pPages->cElems = 0;
		return E_OUTOFMEMORY;
	}

	pPages->cElems = 2;
	pPages->pElems[0] = CLSID_HMCEncoderPage;
	pPages->pElems[1] = CLSID_HMCEncoderLogPage;

	return S_OK;
}

//*************** CBaseFilter ***************\\

int EncoderFilter::GetPinCount(void)
{
	return 3;
}

CBasePin * EncoderFilter::GetPin(int n)
{
	if (n == 0)
		return this->m_videoPin;
	else if (n == 1)
		return this->m_audioPin;
	else if (n == 2)
		return this->m_streamPin;

    return NULL;
}

HRESULT EncoderFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cAutoLock(this->m_pLock);

	HRESULT hr = S_OK;
	bool hasAudio, hasVideo;
	AM_MEDIA_TYPE amt, vmt;
	ZeroMemory(&amt, sizeof(AM_MEDIA_TYPE));
	ZeroMemory(&vmt, sizeof(AM_MEDIA_TYPE));

	hasAudio = (this->m_audioPin->ConnectionMediaType(&amt) == S_OK);
	this->m_audioEOS = !hasAudio;

	hasVideo = (this->m_videoPin->ConnectionMediaType(&vmt) == S_OK);
	this->m_videoEOS = !hasVideo;

	CHECK_HR(hr = this->m_encoder->Start(&this->m_params->m_params, (hasAudio) ? &amt : NULL, (hasVideo) ? &vmt : NULL, OutBufferSize, 
		this->m_streamPin, EncoderStreamOutputPin::ReadPackets, EncoderStreamOutputPin::WritePackets, EncoderStreamOutputPin::Seek));

	CHECK_HR(hr = CBaseFilter::Run(tStart));

done:

	FreeMediaType(amt);
	FreeMediaType(vmt);

	return hr;
}

HRESULT EncoderFilter::Stop(void)
{
	this->m_encoder->Stop(FALSE);

	return CBaseFilter::Stop();
}

//*************** HMCFilter ****************\\

CUnknown * WINAPI EncoderFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    EncoderFilter * filter = new EncoderFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}