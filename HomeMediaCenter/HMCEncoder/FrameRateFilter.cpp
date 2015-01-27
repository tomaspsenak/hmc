#include "StdAfx.h"
#include "FrameRateFilter.h"
#include <dvdmedia.h>

//********************************************
//************ FrameRateInputPin *************
//********************************************

FrameRateInputPin::FrameRateInputPin(LPCTSTR pObjectName, FrameRateFilter * pFilter, HRESULT * phr, LPCWSTR pName) :
	CTransInPlaceInputPin(pObjectName, pFilter, phr, pName), m_pFilter(pFilter)
{
}

FrameRateInputPin::~FrameRateInputPin(void)
{
}

STDMETHODIMP FrameRateInputPin::NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly)
{
	CAutoLock lock(this->m_pLock);

	if (this->m_pFilter->m_pOutput->IsConnected() && this->m_pFilter->m_pInput->IsConnected())
	{
		REFERENCE_TIME inFrameLength, outFrameLength;

		if (this->m_mt.formattype == FORMAT_VideoInfo)
			inFrameLength = ((VIDEOINFOHEADER *)this->m_mt.pbFormat)->AvgTimePerFrame;
		else if (this->m_mt.formattype == FORMAT_VideoInfo2)
			inFrameLength = ((VIDEOINFOHEADER2 *)this->m_mt.pbFormat)->AvgTimePerFrame;
		else
			return CTransInPlaceInputPin::NotifyAllocator(pAllocator, bReadOnly);

		if (this->m_pFilter->m_pOutput->CurrentMediaType().formattype == FORMAT_VideoInfo)
			outFrameLength = ((VIDEOINFOHEADER *)this->m_pFilter->m_pOutput->CurrentMediaType().pbFormat)->AvgTimePerFrame;
		else if (this->m_pFilter->m_pOutput->CurrentMediaType().formattype == FORMAT_VideoInfo2)
			outFrameLength = ((VIDEOINFOHEADER2 *)this->m_pFilter->m_pOutput->CurrentMediaType().pbFormat)->AvgTimePerFrame;
		else
			return CTransInPlaceInputPin::NotifyAllocator(pAllocator, bReadOnly);

		//Ak je na vystupe vacsi pocet snimkov (kratsia dlzka), pouzije sa zvlast vlakno no ich posielanie
		//Zabezpeci to nastavenie allocatora ako readonly, potom sa pouzije zvlast allocator na vstupe a vystupe
		if (outFrameLength < inFrameLength)
			return CTransInPlaceInputPin::NotifyAllocator(pAllocator, TRUE);
	}

	return CTransInPlaceInputPin::NotifyAllocator(pAllocator, bReadOnly);
}

HRESULT FrameRateInputPin::CheckMediaType(const CMediaType * pmt)
{
	HRESULT hr = this->m_pTIPFilter->CheckInputType(pmt);
	if (hr != S_OK)
		return hr;

	CTransformOutputPin * output = this->m_pFilter->m_pOutput;
	CMediaType & pmtOut = output->CurrentMediaType();

	//Ak je vystupny pin pripojeny, akceptuje iba nastaveny typ vystupneho pinu
	//Preto sa porovna AM_MEDIA_TYPE okrem AvgTimePerFrame
	if (output->IsConnected())
	{
		if (pmtOut.majortype != pmt->majortype)
			return VFW_E_NO_ACCEPTABLE_TYPES;

		if (pmtOut.subtype != pmt->subtype)
			return VFW_E_NO_ACCEPTABLE_TYPES;

		if (pmtOut.formattype != pmt->formattype)
			return VFW_E_NO_ACCEPTABLE_TYPES;

		if (pmtOut.formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER * vihOut = (VIDEOINFOHEADER *)pmtOut.pbFormat;
			VIDEOINFOHEADER * vihIn = (VIDEOINFOHEADER *)pmt->pbFormat;

			if (memcmp(&vihOut->rcSource, &vihIn->rcSource, sizeof(RECT)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;

			if (memcmp(&vihOut->rcTarget, &vihIn->rcTarget, sizeof(RECT)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;

			if (memcmp(&vihOut->bmiHeader, &vihIn->bmiHeader, sizeof(BITMAPINFOHEADER)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;
		}
		else if (pmtOut.formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2 * vihOut = (VIDEOINFOHEADER2 *)pmtOut.pbFormat;
			VIDEOINFOHEADER2 * vihIn = (VIDEOINFOHEADER2 *)pmt->pbFormat;

			if (memcmp(&vihOut->rcSource, &vihIn->rcSource, sizeof(RECT)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;

			if (memcmp(&vihOut->rcTarget, &vihIn->rcTarget, sizeof(RECT)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;

			if (memcmp(&vihOut->bmiHeader, &vihIn->bmiHeader, sizeof(BITMAPINFOHEADER)) != 0)
				return VFW_E_NO_ACCEPTABLE_TYPES;
		}
		else
		{
			return VFW_E_NO_ACCEPTABLE_TYPES;
		}
	}

	return S_OK;
}

//********************************************
//*********** FrameRateOutputPin *************
//********************************************

FrameRateOutputPin::FrameRateOutputPin(LPCTSTR pObjectName, FrameRateFilter * pFilter, HRESULT * phr, LPCWSTR pName) :
	CTransInPlaceOutputPin(pObjectName, pFilter, phr, pName)
{
}

FrameRateOutputPin::~FrameRateOutputPin(void)
{
}

STDMETHODIMP FrameRateOutputPin::EnumMediaTypes(IEnumMediaTypes ** ppEnum)
{
	//Nedelegovat EnumMediaTypes na vstupny pin - pouzit GetMediaType filtra
	//z dovodu menenia AvgTimePerFrame v AM_MEDIA_TYPE
	return CTransformOutputPin::EnumMediaTypes(ppEnum);
}

//********************************************
//************* FrameRateFilter **************
//********************************************

FrameRateFilter::FrameRateFilter(LPUNKNOWN punk, HRESULT * phr) : CTransInPlaceFilter(L"HMCFrameRateFilter", punk, CLSID_HMCFrameRate, phr), 
	m_rtOriginFrameLength(0), m_rtLastFrame(0), m_fpsModSum(0), m_params(NULL), m_threadSample(NULL)
{
	this->m_pInput = new FrameRateInputPin(L"TransInPlace input pin", this, phr, L"Input");
	if (this->m_pInput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_pOutput = new FrameRateOutputPin(L"TransInPlace output pin", this, phr, L"Output");
	if (this->m_pOutput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_params = new FrameRateParameters(this);
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}

FrameRateFilter::~FrameRateFilter(void)
{
	if (this->m_params != NULL)
		delete this->m_params;
}

STDMETHODIMP FrameRateFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCFrameRate)
	{
		return GetInterface(this->m_params, ppv);
	}
	else if (riid == IID_ISpecifyPropertyPages)
	{
		return GetInterface((ISpecifyPropertyPages*) this, ppv);
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

//********** ISpecifyPropertyPages **********\\

STDMETHODIMP FrameRateFilter::GetPages(CAUUID * pPages)
{
	CheckPointer(pPages, E_POINTER);

	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL)
	{
		pPages->cElems = 0;
		return E_OUTOFMEMORY;
	}

	pPages->cElems = 1;
	pPages->pElems[0] = CLSID_HMCFrameRatePage;

	return S_OK;
}

//*********** CTransInPlaceFilter ***********\\

CBasePin * FrameRateFilter::GetPin(int n)
{
    if (n == 0)
        return this->m_pInput;
    else if (n == 1)
        return this->m_pOutput;
    else
        return NULL;
}

HRESULT FrameRateFilter::Transform(IMediaSample * pSample)
{
	return E_NOTIMPL;
}

//************* CTransformFilter *************\\

HRESULT FrameRateFilter::StartStreaming(void)
{
	//State lock (m_pLock / m_csFilter) je zamknuty
	//Spustenie vlakna
    if (!Create())
        return E_FAIL;

	return S_OK;
}

HRESULT FrameRateFilter::StopStreaming(void)
{
	//State lock (m_pLock / m_csFilter) je zamknuty
	this->m_fpsModSum = 0;
	this->m_rtLastFrame = 0;

	if (ThreadExists())
	{
		HRESULT hr = CallWorker(CMD_STOP);
		if (FAILED(hr))
			return hr;

		//Pockat na ukoncenie vlakna
		Close();
    }

	return S_OK;
}

HRESULT FrameRateFilter::EndOfStream(void)
{
	//Receive lock (m_csReceive) je zamknuty
	if (UsingDifferentAllocators())
	{
		//Pri roznych alokatoroch sa pouziva zvlast vlakno na posielanie sampla / udalosti
		//Funkcia Stop zamkyna m_csFilter a m_csReceive, preto nehrozi ze CallWorker nebude reagovat
		//Treba state lock (m_csFilter) aby sa neukoncovalo vlakno (ale stale ThreadExists) a CallWorker by uz nedostal odpoved
		CallWorker(CMD_EOS);
	}
	else
	{
		return CTransInPlaceFilter::EndOfStream();
	}

	return S_OK;
}

HRESULT FrameRateFilter::CheckInputType(const CMediaType * mtIn)
{
	CheckPointer(mtIn, E_POINTER);

	BITMAPINFOHEADER * bmi;

	if (!mtIn->bFixedSizeSamples)
		return E_FAIL;

	if (mtIn->majortype != MEDIATYPE_Video)
		return E_FAIL; 

	if (mtIn->formattype == FORMAT_VideoInfo)
		bmi = &((VIDEOINFOHEADER *)mtIn->pbFormat)->bmiHeader;
	else if (mtIn->formattype == FORMAT_VideoInfo2)
		bmi = &((VIDEOINFOHEADER2 *)mtIn->pbFormat)->bmiHeader;
	else
		return E_FAIL;

	//Neakceptovat prevrateny obraz - napr. pri EVR
	if (bmi->biHeight < 0)
		return E_FAIL;

	//Na zaciatku povoli kazdy video format na vstupe
	//Po pripojeni vystupu sa prepoji na novy format
	return S_OK;
}

HRESULT FrameRateFilter::GetMediaType(int iPosition, CMediaType * pMediaType)
{
	HRESULT hr = S_OK;
	int actPos = 0;

	AM_MEDIA_TYPE * mediaType = NULL;
	IEnumMediaTypes * enumTypes = NULL;

	IPin * conPin = this->m_pInput->GetConnected();
	if (conPin == NULL)
		CHECK_HR(hr = VFW_E_NOT_CONNECTED);

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
					header->AvgTimePerFrame = this->m_params->m_rtFrameLength;
				}
				else
				{
					VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
					this->m_rtOriginFrameLength = header->AvgTimePerFrame;
					header->AvgTimePerFrame = this->m_params->m_rtFrameLength;
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

HRESULT FrameRateFilter::CompleteConnect(PIN_DIRECTION direction, IPin * pReceivePin)
{
	if (direction == PINDIR_OUTPUT)
		return ReconnectPinSync(this->m_pInput, this->m_pOutput, this->m_rtOriginFrameLength);

	return S_OK;
}

HRESULT FrameRateFilter::Receive(IMediaSample * pSample)
{
	//Spracovava sa iba media stream
    AM_SAMPLE2_PROPERTIES * const pProps = this->m_pInput->SampleProps();
    if (pProps->dwStreamId != AM_STREAM_MEDIA)
	{
        return this->m_pOutput->Deliver(pSample);
    }
	CheckPointer(pSample, E_POINTER);

    HRESULT hr = S_OK;

	if (UsingDifferentAllocators())
	{
		//Pri roznych alokatoroch sa pouziva zvlast vlakno na posielanie sampla / udalosti
		ASSERT(this->m_threadSample == NULL);

		this->m_threadSample = Copy(pSample);
		if (this->m_threadSample == NULL)
			CHECK_HR(hr = E_OUTOFMEMORY);

		{
			//Treba state lock aby sa neukoncovalo vlakno (ale stale ThreadExists) a CallWorker by uz nedostal odpoved
			CAutoLock lock(this->m_pLock);
			hr = CallWorker(CMD_RECEIVE);
		}

		SAFE_RELEASE(this->m_threadSample);
	}
	else
	{
		//Priamo odosli sample
		hr = SendSample(pSample);
	}

done:

	return hr;
}

//************ CAMThread ************\\

DWORD FrameRateFilter::ThreadProc(void)
{
	Command cmd;

    do 
	{
		cmd = (Command)GetRequest();

		switch (cmd)
		{
			case CMD_RECEIVE:
			{
				IMediaSample * pOut = this->m_threadSample;
				SAFE_ADDREF(pOut);

				Reply(S_OK);

				//Ak sa pri spracovani sampla vyskytne chyba, je vyvolana udalost s chybovym hlasenim
				if (FAILED(SendSample(pOut)))
					this->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);

				SAFE_RELEASE(pOut);

				break;
			}
			case CMD_EOS:
			{
				Reply(S_OK);
				
				//Zamkni state lock (m_pLock / m_csFilter) aby sa nerozpojil pin pocas EndOfStream
				CAutoLock lock(this->m_pLock);
				CTransInPlaceFilter::EndOfStream();

				break;
			}
			case CMD_STOP:
				Reply(S_OK);
				break;
			default:
				Reply(E_NOTIMPL);
				break;
		}
    } while (cmd != CMD_STOP);

	return 0;
}

//************ FrameRateFilter *************\\

CUnknown * WINAPI FrameRateFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    FrameRateFilter * filter = new FrameRateFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}

HRESULT FrameRateFilter::ReconnectPinSync(CBasePin * pin, CBasePin * mediaTypePin, REFERENCE_TIME avgTimePerFrame)
{
	CMediaType newMt;
	HRESULT hr = S_OK;

	//Ak je pripojeny vystup, nastavi sa vstup podla neho
	CHECK_HR(hr = mediaTypePin->ConnectionMediaType(&newMt));

	if (newMt.formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * header = (VIDEOINFOHEADER *)newMt.pbFormat;
		header->AvgTimePerFrame = avgTimePerFrame;
	}
	else if (newMt.formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)newMt.pbFormat;
		header->AvgTimePerFrame = avgTimePerFrame;
	}
	else
	{
		hr = E_FAIL;
		goto done;
	}

	//ReconnectPin je asynchronny, preto je pouzita nahrada - Disconnect a ConnectDirect
	//ReconnectPin sposoboval problemy ak pripojeny filter (napr. EVR) viackrat sucasne
	//volal CompleteConnect a tym aj asynchronny ReconnectPin
	CBaseFilter::ReconnectPinSync(pin, &newMt);

done:

	//FreeMediaType(newMt) netreba volat - vola sa v destruktore CMediaType
	return hr;
}

HRESULT FrameRateFilter::SendSample(IMediaSample * pSample)
{
	HRESULT hr = S_OK;
	REFERENCE_TIME rtOldStart, rtOldStop;
	
	CHECK_HR(hr = pSample->GetTime(&rtOldStart, &rtOldStop));

	//Snimka sa bude opakovat, pokial nedosiahne povodnu casovu peciatku
	while (this->m_rtLastFrame + this->m_params->m_rtFrameLength <= rtOldStop)
	{
		REFERENCE_TIME rtStart = this->m_rtLastFrame;
		REFERENCE_TIME rtStop  = rtStart + this->m_params->m_rtFrameLength;

		//Do uvahy sa bere aj desatinna cast - ak presiahne 1 a viac
		this->m_fpsModSum += this->m_params->m_fpsMod;
		rtStop += (this->m_fpsModSum / this->m_params->m_fps);
		this->m_fpsModSum %= this->m_params->m_fps;

		CHECK_HR(hr = pSample->SetTime(&rtStart, &rtStop));
		this->m_rtLastFrame = rtStop;

		CHECK_HR(hr = this->m_pOutput->Deliver(pSample));
	}

done:

	return hr;
}