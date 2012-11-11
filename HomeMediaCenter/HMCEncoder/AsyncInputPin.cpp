#include "StdAfx.h"
#include "AsyncInputPin.h"

AsyncInputPin::AsyncInputPin(LPCTSTR pObjectName, HMCFilter * pFilter, CCritSec * pLock, HRESULT * phr) : 
	CBaseInputPin(pObjectName, (CBaseFilter *)pFilter, pLock, phr, pObjectName), CAMThread(phr), m_pFilter(pFilter), m_tempSample(NULL),
	m_deliverBufferLen(0), m_deliverBuffer(NULL)
{
	if (this->m_pFilter == NULL)
		*phr = E_FAIL;
}

AsyncInputPin::~AsyncInputPin(void)
{
	if (this->m_deliverBuffer)
	{
		free(this->m_deliverBuffer);
		this->m_deliverBuffer = NULL;
	}
}

//********** CBaseOutputPin **********\\

HRESULT AsyncInputPin::Active(void)
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

HRESULT AsyncInputPin::Inactive(void)
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

STDMETHODIMP AsyncInputPin::BeginFlush(void)
{
	return CBaseInputPin::BeginFlush();
}

STDMETHODIMP AsyncInputPin::EndFlush(void)
{
	return CBaseInputPin::EndFlush();
}

STDMETHODIMP AsyncInputPin::EndOfStream(void)
{
	return CallWorker(CMD_EOS);
}

//************ IMemInputPin ************

STDMETHODIMP AsyncInputPin::GetAllocator(IMemAllocator ** ppAllocator)
{
	//Vstupny filter nemusi poskytovat allocator - povodny nepracoval s WebM
	return VFW_E_NO_ALLOCATOR;
}

STDMETHODIMP AsyncInputPin::Receive(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	//m_tempSample by malo byt vzdy NULL, iba pri multithread nemusi byt
	if (this->m_tempSample)
		ASSERT("m_tempSample must be NULL");

	this->m_tempSample = pSample;

	HRESULT hr = CallWorker(CMD_RECEIVE);
	this->m_tempSample = NULL;
	return hr;
}

//************ CAMThread ************\\

DWORD AsyncInputPin::ThreadProc(void)
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
				
				ReceiveAsync(this->m_deliverBuffer, length);
				
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
