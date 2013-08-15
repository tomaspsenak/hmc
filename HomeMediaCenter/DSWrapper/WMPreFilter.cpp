#include "StdAfx.h"
#include "WMPreFilter.h"
#include <dvdmedia.h>

//********************************************
//************ WMVideoPreFilter **************
//********************************************

HRESULT WMVideoPreFilter::CheckInputType(const CMediaType * mtIn)
{
	CheckPointer(mtIn, E_POINTER);

	if (!mtIn->bFixedSizeSamples)
		return E_FAIL;

	if (mtIn->majortype != MEDIATYPE_Video)
		return E_FAIL;

	if (mtIn->formattype != FORMAT_VideoInfo && mtIn->formattype != FORMAT_VideoInfo2)
        return E_FAIL;

	//Na zaciatku povoli kazdy video format na vstupe
	//Po pripojeni vystupu sa prepoji na novy format
	return S_OK;
}

HRESULT WMVideoPreFilter::GetMediaType(int iPosition, CMediaType * pMediaType)
{
	HRESULT hr = S_OK;
	int actPos = 0;

	AM_MEDIA_TYPE * mediaType = NULL;
	IEnumMediaTypes * enumTypes = NULL;

	IPin * conPin = this->m_pInput->GetConnected();
	if (conPin == NULL)
		CHECK_HR(hr = E_UNEXPECTED);

	CHECK_HR(hr = conPin->EnumMediaTypes(&enumTypes));
	CHECK_HR(hr = enumTypes->Reset());

	while (enumTypes->Next(1, &mediaType, NULL) == S_OK)
	{
		if (mediaType->formattype == FORMAT_VideoInfo || mediaType->formattype == FORMAT_VideoInfo2)
		{
			if (actPos == iPosition)
			{
				CHECK_HR(hr = CopyMediaType(pMediaType, mediaType));

				//Zmena biPlanes na 1 - vyzaduje sa
				if (pMediaType->formattype == FORMAT_VideoInfo)
				{
					VIDEOINFOHEADER * header = (VIDEOINFOHEADER *)pMediaType->pbFormat;
					header->bmiHeader.biPlanes = 1;
				}
				else
				{
					VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
					header->bmiHeader.biPlanes = 1;
				}
				goto done;
			}

			actPos++;
		}

		DeleteMediaType(mediaType);
		mediaType = NULL;
	}

	hr = VFW_S_NO_MORE_ITEMS;

done:

	DeleteMediaType(mediaType);
	SAFE_RELEASE(enumTypes);

	return hr;
}

//********************************************
//************ WMAudioPreFilter **************
//********************************************

HRESULT WMAudioPreFilter::CheckInputType(const CMediaType * mtIn)
{
	CheckPointer(mtIn, E_POINTER);

	if (mtIn->majortype != MEDIATYPE_Audio)
		return E_FAIL;

	if (mtIn->formattype != FORMAT_WaveFormatEx)
        return E_FAIL;

	WAVEFORMATEX * wave = (WAVEFORMATEX *)mtIn->pbFormat;
	if (wave->wFormatTag != WAVE_FORMAT_PCM)
		return E_FAIL;

	return S_OK;
}

HRESULT WMAudioPreFilter::GetMediaType(int iPosition, CMediaType * pMediaType)
{
	HRESULT hr = S_OK;
	int actPos = 0;

	AM_MEDIA_TYPE * mediaType = NULL;
	IEnumMediaTypes * enumTypes = NULL;

	IPin * conPin = this->m_pInput->GetConnected();
	if (conPin == NULL)
		CHECK_HR(hr = E_UNEXPECTED);

	CHECK_HR(hr = conPin->EnumMediaTypes(&enumTypes));
	CHECK_HR(hr = enumTypes->Reset());

	while (enumTypes->Next(1, &mediaType, NULL) == S_OK)
	{
		if (mediaType->formattype == FORMAT_WaveFormatEx)
		{
			WAVEFORMATEX * wave = (WAVEFORMATEX *)mediaType->pbFormat;
			if (wave->wFormatTag == WAVE_FORMAT_PCM)
			{
				if (actPos == iPosition)
				{
					CHECK_HR(hr = CopyMediaType(pMediaType, mediaType));

					goto done;
				}

				actPos++;
			}
		}

		DeleteMediaType(mediaType);
		mediaType = NULL;
	}

	hr = VFW_S_NO_MORE_ITEMS;

done:

	DeleteMediaType(mediaType);
	SAFE_RELEASE(enumTypes);

	return hr;
}