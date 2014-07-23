#include "StdAfx.h"
#include "EncoderAsyncInputPin.h"
#include "EncoderFilter.h"

EncoderAsyncInputPin::EncoderAsyncInputPin(LPCTSTR pObjectName, EncoderFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	CBaseInputPin(pObjectName, (CBaseFilter *)pFilter, pLock, phr, pObjectName), CAMThread(phr), m_pFilter(pFilter), m_tempSample(NULL),
	m_deliverBufferLen(0), m_deliverBuffer(NULL), m_time(-1)
{
	if (this->m_pFilter == NULL)
		*phr = E_FAIL;
}

EncoderAsyncInputPin::~EncoderAsyncInputPin(void)
{
	if (this->m_deliverBuffer)
	{
		free(this->m_deliverBuffer);
		this->m_deliverBuffer = NULL;
	}
}

HRESULT EncoderAsyncInputPin::GetCurrentPosition(LONGLONG * pCurrent)
{
	if (this->m_time < 0)
		return E_NOTIMPL;

	*pCurrent = this->m_time;
	return S_OK;
}

//********** CBaseOutputPin **********\\

HRESULT EncoderAsyncInputPin::Active(void)
{
	CAutoLock lock(this->m_pLock);

	//Zdroje uz su alokovane?
	if (this->CBaseInputPin::m_pFilter->IsActive())
		return S_FALSE;

    //Pin musi byt pripojeny
    if (!IsConnected())
        return E_FAIL;

	//Inicializuj memoryallocator
    HRESULT hr = CBaseInputPin::Active();
    if (FAILED(hr))
        return hr;

    //Spustenie vlakna
    if (!Create())
        return E_FAIL;

    return S_OK;
}

HRESULT EncoderAsyncInputPin::Inactive(void)
{
	CAutoLock lock(this->m_pLock);

	//Pin nebol pripojeny
    if (!IsConnected())
        return S_OK;

    // !!! need to do this before trying to stop the thread, because
    // we may be stuck waiting for our own allocator!!!
    HRESULT hr = CBaseInputPin::Inactive();
    if (FAILED(hr))
		return hr;

	//Treba uzamknut thread handle inak moze vzniknut race condition
	CAutoLock lock2(&this->m_AccessLock);
    if (ThreadExists())
	{
		hr = CallWorker(CMD_STOP);
		if (FAILED(hr))
			return hr;

		//Pockat na ukoncenie vlakna
		Close();
    }

    return S_OK;
}

//*************** IPin ***************\\

STDMETHODIMP EncoderAsyncInputPin::BeginFlush(void)
{
	return CBaseInputPin::BeginFlush();
}

STDMETHODIMP EncoderAsyncInputPin::EndFlush(void)
{
	return CBaseInputPin::EndFlush();
}

STDMETHODIMP EncoderAsyncInputPin::EndOfStream(void)
{
	return CallWorker(CMD_EOS);
}

//************ IMemInputPin ************

STDMETHODIMP EncoderAsyncInputPin::GetAllocator(IMemAllocator ** ppAllocator)
{
	//Vstupny filter nemusi poskytovat allocator - povodny nepracoval s WebM
	return VFW_E_NO_ALLOCATOR;
}

STDMETHODIMP EncoderAsyncInputPin::Receive(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);
	HRESULT hr;

	//m_tempSample by malo byt vzdy NULL, iba pri multithread nemusi byt
	if (this->m_tempSample)
		ASSERT("m_tempSample must be NULL");

	//Ignorovat sample so zapornym timeStart - napr. po seekovani treba zaciatok vynechat
	REFERENCE_TIME timeStart = 0, timeEnd;
	hr = pSample->GetTime(&timeStart, &timeEnd);
	if (hr == S_OK && timeStart < 0)
		return S_OK;
	this->m_time = timeStart;

	this->m_tempSample = pSample;
	hr = CallWorker(CMD_RECEIVE);
	this->m_tempSample = NULL;

	return hr;
}

//************ CAMThread ************\\

DWORD EncoderAsyncInputPin::ThreadProc(void)
{
	Command cmd;

    do 
	{
		cmd = (Command)GetRequest();

		switch (cmd)
		{
			case CMD_RECEIVE:
			{
				BYTE * buffer;
				long length = this->m_tempSample->GetActualDataLength();

				if (FAILED(this->m_tempSample->GetPointer(&buffer)))
				{
					Reply(E_FAIL);
					continue;
				}
				
				if (this->m_deliverBufferLen < length)
				{
					BYTE * tempBuf = (BYTE *)realloc(this->m_deliverBuffer, length);
					if (!tempBuf)
					{
						Reply(E_OUTOFMEMORY);
						continue;
					}
					this->m_deliverBuffer = tempBuf;
					this->m_deliverBufferLen = length;
				}

				memcpy(this->m_deliverBuffer, buffer, length);

				Reply(S_OK);

				//Ak sa pri spracovani sampla vyskytne chyba, je vyvolana udalost s chybovym hlasenim
				if (FAILED(ReceiveAsync(this->m_deliverBuffer, length)))
					this->m_pFilter->NotifyEvent(EC_ERRORABORT, E_FAIL, 0);
				
				break;
			}
			case CMD_EOS:
			{
				Reply(S_OK);
				EndOfStreamAsync();
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
