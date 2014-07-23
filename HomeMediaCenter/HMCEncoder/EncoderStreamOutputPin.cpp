#include "StdAfx.h"
#include "EncoderStreamOutputPin.h"

EncoderStreamOutputPin::EncoderStreamOutputPin(EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr) :
	CBaseOutputPin(L"Stream Output", pFilter, pLock, phr, L"Stream Output"), m_pFilter(pFilter), m_offset(0), m_stream(NULL)
{
	if (this->m_pFilter == NULL)
		*phr = E_FAIL;
}

EncoderStreamOutputPin::~EncoderStreamOutputPin(void)
{
	SAFE_RELEASE(this->m_stream);
}

//*************** IPin ***************\\

STDMETHODIMP EncoderStreamOutputPin::EndOfStream(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_encoder->Stop(TRUE);

	DeliverEndOfStream();

	this->m_pFilter->NotifyEvent(EC_COMPLETE, S_OK, NULL);

	return S_OK;
}

//************* CBasePin *************\\

HRESULT EncoderStreamOutputPin::GetMediaType(int iPosition, CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Stream);
	pMediaType->SetSubtype(&MEDIASUBTYPE_NULL);
	pMediaType->SetSampleSize(EncoderFilter::OutBufferSize);

	return S_OK;
}

HRESULT EncoderStreamOutputPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Stream)
		return S_OK;

	return VFW_E_NO_ACCEPTABLE_TYPES;
}

//********** CBaseOutputPin **********\\

HRESULT EncoderStreamOutputPin::Active(void)
{
    return CBaseOutputPin::Active();
}

HRESULT EncoderStreamOutputPin::Inactive(void)
{
	return CBaseOutputPin::Inactive();
}

HRESULT EncoderStreamOutputPin::BreakConnect(void)
{
	SAFE_RELEASE(this->m_stream);

	return CBaseOutputPin::BreakConnect();
}

HRESULT EncoderStreamOutputPin::CompleteConnect(IPin * pReceivePin)
{
	this->m_offset = 0;
	pReceivePin->QueryInterface(IID_IStream, (void **)&this->m_stream);

	return CBaseOutputPin::CompleteConnect(pReceivePin);
}

HRESULT EncoderStreamOutputPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * ppropInputRequest)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(ppropInputRequest, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;

	CAutoLock cAutoLock(this->m_pLock);

    ppropInputRequest->cBuffers = 1;
	ppropInputRequest->cbAlign  = 1;
    ppropInputRequest->cbBuffer = EncoderFilter::OutBufferSize;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(ppropInputRequest, &aProp));

    if(aProp.cbBuffer < ppropInputRequest->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

int EncoderStreamOutputPin::WritePackets(void * opaque, uint8_t * buf, int buf_size)
{
	HRESULT hr = S_OK;
	BYTE * pBuffer = NULL;
	IMediaSample * pSample = NULL;

	EncoderStreamOutputPin * pThis = (EncoderStreamOutputPin *)opaque;
	REFERENCE_TIME newOffset = pThis->m_offset;

	hr = pThis->GetDeliveryBuffer(&pSample, NULL, NULL, 0);
	if (hr != S_OK)
	{
		//Pokusi sa zapisat buffer cez IStream - allocator je dealokovany
		if (pThis->m_stream)
		{
			ULONG written;
			LARGE_INTEGER move;
			move.QuadPart = pThis->m_offset;

			CHECK_HR(hr = pThis->m_stream->Seek(move, STREAM_SEEK_SET, NULL));
			CHECK_HR(hr = pThis->m_stream->Write(buf, buf_size, &written));

			pThis->m_offset += written;
		}

		goto done;
	}

	buf_size = min(pSample->GetSize(), buf_size);
	newOffset += buf_size;
	
	CHECK_HR(hr = pSample->GetPointer(&pBuffer));

	//Pozor ak sa velkost nezhoduje
	memcpy(pBuffer, buf, buf_size);

	CHECK_HR(hr = pSample->SetActualDataLength(buf_size));
	CHECK_HR(hr = pSample->SetTime(&pThis->m_offset, &newOffset));

	pThis->m_offset = newOffset;

	CHECK_HR(hr = pThis->Deliver(pSample));

done:
	SAFE_RELEASE(pSample);

	return buf_size;
}

int EncoderStreamOutputPin::ReadPackets(void * opaque, uint8_t * buf, int buf_size)
{
	EncoderStreamOutputPin * pThis = (EncoderStreamOutputPin *)opaque;

	return 0;
}

int64_t EncoderStreamOutputPin::Seek(void * opaque, int64_t offset, int whence)
{
	EncoderStreamOutputPin * pThis = (EncoderStreamOutputPin *)opaque;

	pThis->m_offset = offset;
	
	return offset;
}