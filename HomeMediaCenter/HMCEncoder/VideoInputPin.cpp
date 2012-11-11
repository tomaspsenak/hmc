#include "StdAfx.h"
#include "VideoInputPin.h"

VideoInputPin::VideoInputPin(HMCFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	AsyncInputPin(L"Video Input", pFilter, pLock, phr)
{
}

VideoInputPin::~VideoInputPin(void)
{
}

//************* CBasePin *************\\

HRESULT VideoInputPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Video)
	{
		if (pmt->subtype == MEDIASUBTYPE_YUY2 || pmt->subtype == MEDIASUBTYPE_UYVY || pmt->subtype == MEDIASUBTYPE_RGB24)
		{
			if (pmt->formattype == FORMAT_VideoInfo || pmt->formattype == FORMAT_VideoInfo2)
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

//************ AsyncInputPin ************

STDMETHODIMP VideoInputPin::EndOfStreamAsync(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_videoEOS = true;
	if (this->m_pFilter->m_videoEOS && this->m_pFilter->m_audioEOS)
		this->m_pFilter->m_streamPin->EndOfStream();

	return S_OK;
}

STDMETHODIMP VideoInputPin::ReceiveAsync(BYTE * buffer, long length)
{
	return this->m_pFilter->m_encoder->EncodeVideo(buffer, length);
}