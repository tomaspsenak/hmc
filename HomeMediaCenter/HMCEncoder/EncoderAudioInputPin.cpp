#include "StdAfx.h"
#include "EncoderAudioInputPin.h"

EncoderAudioInputPin::EncoderAudioInputPin(EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	EncoderAsyncInputPin(L"Audio Input", pFilter, pLock, phr)
{
}

EncoderAudioInputPin::~EncoderAudioInputPin(void)
{
}

//************* CBasePin *************\\

HRESULT EncoderAudioInputPin::CheckMediaType(const CMediaType * pmt)
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

	return VFW_E_NO_ACCEPTABLE_TYPES;
}

//******* EncoderAsyncInputPin ********\\

STDMETHODIMP EncoderAudioInputPin::EndOfStreamAsync(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_audioEOS = true;
	if (this->m_pFilter->m_videoEOS && this->m_pFilter->m_audioEOS)
		this->m_pFilter->m_streamPin->EndOfStream();

	return S_OK;
}

STDMETHODIMP EncoderAudioInputPin::ReceiveAsync(BYTE * buffer, long length)
{
	return this->m_pFilter->m_encoder->EncodeAudio(buffer, length);
}