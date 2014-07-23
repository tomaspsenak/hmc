#include "StdAfx.h"
#include "EncoderVideoInputPin.h"
#include <dvdmedia.h>

EncoderVideoInputPin::EncoderVideoInputPin(EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	EncoderAsyncInputPin(L"Video Input", pFilter, pLock, phr)
{
}

EncoderVideoInputPin::~EncoderVideoInputPin(void)
{
}

//************* CBasePin *************\\

HRESULT EncoderVideoInputPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Video)
	{
		if (pmt->subtype == MEDIASUBTYPE_YUY2 || pmt->subtype == MEDIASUBTYPE_UYVY || pmt->subtype == MEDIASUBTYPE_RGB24 || 
			pmt->subtype == MEDIASUBTYPE_YV12)
		{
			if (pmt->formattype == FORMAT_VideoInfo)
			{
				if (pmt->subtype == MEDIASUBTYPE_YV12)
				{
					VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *)pmt->pbFormat;
					if (vih->bmiHeader.biPlanes == 3 && vih->bmiHeader.biCompression == MAKEFOURCC('Y','V','1','2'))
						return S_OK;
				}
				else
				{
					return S_OK;
				}
			}
			else if (pmt->formattype == FORMAT_VideoInfo2)
			{
				if (pmt->subtype == MEDIASUBTYPE_YV12)
				{
					VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *)pmt->pbFormat;
					if (vih->bmiHeader.biPlanes == 3 && vih->bmiHeader.biCompression == MAKEFOURCC('Y','V','1','2'))
						return S_OK;
				}
				else
				{
					return S_OK;
				}
			}
		}
	}

	return VFW_E_NO_ACCEPTABLE_TYPES;
}

//******* EncoderAsyncInputPin ********\\

STDMETHODIMP EncoderVideoInputPin::EndOfStreamAsync(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_videoEOS = true;
	if (this->m_pFilter->m_videoEOS && this->m_pFilter->m_audioEOS)
		this->m_pFilter->m_streamPin->EndOfStream();

	return S_OK;
}

STDMETHODIMP EncoderVideoInputPin::ReceiveAsync(BYTE * buffer, long length)
{
	return this->m_pFilter->m_encoder->EncodeVideo(buffer, length);
}