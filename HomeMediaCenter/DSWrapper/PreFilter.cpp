#include "StdAfx.h"
#include "PreFilter.h"

STDMETHODIMP PreInputPin::Receive(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);
	HRESULT hr;

	//Ignorovat zmenu typu pocas behu grafu
	pSample->SetMediaType(NULL);

	//Ignorovat sample so zapornym timeStart - napr. po seekovani treba zaciatok vynechat
	REFERENCE_TIME timeStart = 0, timeEnd;
	hr = pSample->GetTime(&timeStart, &timeEnd);
	if (hr == S_OK && timeStart < 0)
		return S_OK;

	return CTransInPlaceInputPin::Receive(pSample);
}

//********************************************
//*************** PreFilter ******************
//********************************************

PreFilter::PreFilter(LPUNKNOWN punk, HRESULT * phr) :  CTransInPlaceFilter(L"PreFilter", punk, CLSID_NULL, phr)
{
	this->m_pInput = new PreInputPin(L"TransInPlace input pin", this, phr, L"Input");
	if (this->m_pInput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	if (*phr != S_OK)
		return;

	this->m_pOutput = new PreOutputPin(L"TransInPlace output pin", this, phr, L"Output");
	if (this->m_pOutput == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}


PreFilter::~PreFilter(void)
{
}

//*********** CTransInPlaceFilter ***********\\

CBasePin * PreFilter::GetPin(int n)
{
    if (n == 0)
        return this->m_pInput;
    else if (n == 1)
        return this->m_pOutput;
    else
        return NULL;
}
