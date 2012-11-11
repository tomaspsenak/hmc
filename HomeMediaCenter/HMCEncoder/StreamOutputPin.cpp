#include "StdAfx.h"
#include "StreamOutputPin.h"

StreamOutputPin::StreamOutputPin(HMCFilter * pFilter, CCritSec * pLock, HRESULT * phr) :
	CBaseOutputPin(L"Stream Output", pFilter, pLock, phr, L"Stream Output"), m_pFilter(pFilter), m_offset(0), m_stream(NULL)
{
	if (this->m_pFilter == NULL)
		*phr = E_FAIL;
}

StreamOutputPin::~StreamOutputPin(void)
{
	SAFE_RELEASE(this->m_stream);
}

//*************** IPin ***************\\

STDMETHODIMP StreamOutputPin::EndOfStream(void)
{
	CAutoLock cAutoLock(this->m_pLock);

	this->m_pFilter->m_encoder->Stop(TRUE);

	DeliverEndOfStream();

	this->m_pFilter->NotifyEvent(EC_COMPLETE, S_OK, NULL);

	return S_OK;
}

//************* CBasePin *************\\

HRESULT StreamOutputPin::GetMediaType(int iPosition, CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Stream);
	pMediaType->SetSubtype(&MEDIASUBTYPE_NULL);
	pMediaType->SetSampleSize(HMCFilter::OutBufferSize);

	return S_OK;
}

HRESULT StreamOutputPin::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Stream)
		return S_OK;

	return S_FALSE;
}

//********** CBaseOutputPin **********\\

HRESULT StreamOutputPin::Active(void)
{
    return CBaseOutputPin::Active();
}

HRESULT StreamOutputPin::Inactive(void)
{
	return CBaseOutputPin::Inactive();
}

HRESULT StreamOutputPin::BreakConnect(void)
{
	SAFE_RELEASE(this->m_stream);

	return CBaseOutputPin::BreakConnect();
}

HRESULT StreamOutputPin::CompleteConnect(IPin * pReceivePin)
{
	pReceivePin->QueryInterface(IID_IStream, (void **)&this->m_stream);

	return CBaseOutputPin::CompleteConnect(pReceivePin);
}

HRESULT StreamOutputPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * ppropInputRequest)
{
	CheckPointer(pAlloc, E_POINTER);
    CheckPointer(ppropInputRequest, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;

	CAutoLock cAutoLock(this->m_pLock);

    ppropInputRequest->cBuffers = 1;
	ppropInputRequest->cbAlign  = 1;
    ppropInputRequest->cbBuffer = HMCFilter::OutBufferSize;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(ppropInputRequest, &aProp));

    if(aProp.cbBuffer < ppropInputRequest->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

int StreamOutputPin::WritePackets(void * opaque, uint8_t * buf, int buf_size)
{
	HRESULT hr = S_OK;
	BYTE * pBuffer = NULL;
	IMediaSample * pSample = NULL;

	StreamOutputPin * pThis = (StreamOutputPin *)opaque;
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

int StreamOutputPin::ReadPackets(void * opaque, uint8_t * buf, int buf_size)
{
	StreamOutputPin * pThis = (StreamOutputPin *)opaque;

	return 0;
}

int64_t StreamOutputPin::Seek(void * opaque, int64_t offset, int whence)
{
	StreamOutputPin * pThis = (StreamOutputPin *)opaque;

	pThis->m_offset = offset;
	
	return offset;
}