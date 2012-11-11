#include "StdAfx.h"
#include "AudioInputPin.h"

AudioInputPin::AudioInputPin(HMCFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	AsyncInputPin(L"Audio Input", pFilter, pLock, phr)
{
}

AudioInputPin::~AudioInputPin(void)
{
}

//************* CBasePin *************\\

HRESULT AudioInputPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Audio)
	{
		if (pmt->subtype == MEDIASUBTYPE_PCM)
		{
			if (pmt->formattype == FORMAT_WaveFormatEx)
			{
				WAVEFORMATEX * wave = (WAVEFORMATEX *)pmt->pbFormat;
				if (wave->wFormatTag == 1)
					return S_OK;
			}
		}
	}

	return S_FALSE;
}

//************ AsyncInputPin ************

STDMETHODIMP AudioInputPin::EndOfStreamAsync(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_audioEOS = true;
	if (this->m_pFilter->m_videoEOS && this->m_pFilter->m_audioEOS)
		this->m_pFilter->m_streamPin->EndOfStream();

	return S_OK;
}

STDMETHODIMP AudioInputPin::ReceiveAsync(BYTE * buffer, long length)
{
	return this->m_pFilter->m_encoder->EncodeAudio(buffer, length);
}