#include "StdAfx.h"
#include "FramerateFilter.h"
#include <dvdmedia.h>

FramerateOutputPin::FramerateOutputPin(LPCTSTR pObjectName, CTransInPlaceFilter * pFilter, HRESULT * phr, LPCWSTR pName) :
	CTransInPlaceOutputPin(pObjectName, pFilter, phr, pName)
{
}

FramerateOutputPin::~FramerateOutputPin(void)
{
}

//********************************************
//************* FramerateFilter **************
//********************************************

FramerateFilter::FramerateFilter(LPUNKNOWN punk, HRESULT * phr, UINT32 fps) : CTransInPlaceFilter(L"FramerateFilter", punk, CLSID_NULL, phr), 
	m_fps(fps), m_fpsMod(UNITS % fps), m_rtFrameLength(UNITS / fps), m_rtOriginFrameLength(0), m_rtLastFrame(0), m_fpsModSum(0)
{
	this->m_pInput = new CTransInPlaceInputPin(L"TransInPlace input pin", this, phr, L"Input");
	if (this->m_pInput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_pOutput = new FramerateOutputPin(L"TransInPlace output pin", this, phr, L"Output");
	if (this->m_pOutput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}

FramerateFilter::~FramerateFilter(void)
{
}

//*********** CTransInPlaceFilter ***********\\

CBasePin * FramerateFilter::GetPin(int n)
{
    if (n == 0)
        return this->m_pInput;
    else if (n == 1)
        return this->m_pOutput;
    else
        return NULL;
}

HRESULT FramerateFilter::Transform(IMediaSample * pSample)
{
	return E_NOTIMPL;
}

//************* CTransformFilter *************\\

HRESULT FramerateFilter::CheckInputType(const CMediaType * mtIn)
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

HRESULT FramerateFilter::GetMediaType(int iPosition, CMediaType * pMediaType)
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

				if (pMediaType->formattype == FORMAT_VideoInfo)
				{
					VIDEOINFOHEADER * header = (VIDEOINFOHEADER *)pMediaType->pbFormat;
					this->m_rtOriginFrameLength = header->AvgTimePerFrame;
					header->AvgTimePerFrame = this->m_rtFrameLength;
				}
				else
				{
					VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
					this->m_rtOriginFrameLength = header->AvgTimePerFrame;
					header->AvgTimePerFrame = this->m_rtFrameLength;
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

HRESULT FramerateFilter::CompleteConnect(PIN_DIRECTION direction, IPin * pReceivePin)
{
	HRESULT hr = S_OK;
	IPin * conPin = NULL;

	AM_MEDIA_TYPE pmt;
	pmt.cbFormat = 0;
	pmt.pUnk = NULL;

	if (direction == PINDIR_OUTPUT)
	{
		//Ak je pripojeny vystup, nastavi sa vstup podla neho
		CHECK_HR(hr = this->m_pOutput->ConnectionMediaType(&pmt));
		
		conPin = this->m_pInput->GetConnected();
		if (conPin == NULL)
			CHECK_HR(hr = E_FAIL);

		if (pmt.formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER * header = (VIDEOINFOHEADER *)pmt.pbFormat;
			header->AvgTimePerFrame = this->m_rtOriginFrameLength;
		}
		else if (pmt.formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)pmt.pbFormat;
			header->AvgTimePerFrame = this->m_rtOriginFrameLength;
		}
		else
		{
			CHECK_HR(hr = E_FAIL);
		}

		hr = conPin->QueryAccept(&pmt);
		if (hr == S_OK)
		{
			CHECK_HR(hr = ReconnectPin(conPin, &pmt));
		}
		else
		{
			CHECK_HR(hr = E_FAIL);
		}
	}

done:

	FreeMediaType(pmt);

	return hr;
}

HRESULT FramerateFilter::Receive(IMediaSample * pSample)
{
	//Spracovava sa iba media stream
    AM_SAMPLE2_PROPERTIES * const pProps = this->m_pInput->SampleProps();
    if (pProps->dwStreamId != AM_STREAM_MEDIA)
	{
        return this->m_pOutput->Deliver(pSample);
    }
	CheckPointer(pSample, E_POINTER);

    HRESULT hr = S_OK;
	IMediaSample * pOut = NULL;
	REFERENCE_TIME rtOldStart, rtOldStop;
	BOOL diffAllocators = UsingDifferentAllocators();

	CHECK_HR(hr = pSample->GetTime(&rtOldStart, &rtOldStop));

	//Snimka sa bude opakovat, pokial nedosiahne povodnu casovu peciatku
	while (this->m_rtLastFrame + this->m_rtFrameLength <= rtOldStop)
	{
		REFERENCE_TIME rtStart = this->m_rtLastFrame;
		REFERENCE_TIME rtStop  = rtStart + this->m_rtFrameLength;

		//Do uvahy sa bere aj desatinna cast - ak presiahne 1 a viac
		this->m_fpsModSum += m_fpsMod;
		rtStop += (this->m_fpsModSum / m_fps);
		this->m_fpsModSum %= m_fps;

		if (diffAllocators)
		{
			pOut = Copy(pSample);
			if (pOut == NULL)
				CHECK_HR(hr = E_UNEXPECTED);
		}
		else
		{
			pOut = pSample;
		}

		CHECK_HR(hr = pOut->SetTime(&rtStart, &rtStop));
		this->m_rtLastFrame = rtStop;

		CHECK_HR(hr = this->m_pOutput->Deliver(pOut));

		if (diffAllocators)
			SAFE_RELEASE(pOut);
	}

done:

	if (diffAllocators)
			SAFE_RELEASE(pOut);

	return hr;
}