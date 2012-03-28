#include "StdAfx.h"
#include "MFAsyncCallback.h"


MFAsyncCallback::MFAsyncCallback(void) : m_cRef(1)
{
}

MFAsyncCallback::~MFAsyncCallback(void)
{
}

//*************** IUnknown ***************\\

HRESULT MFAsyncCallback::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
    {
        QITABENT(MFAsyncCallback, IMFAsyncCallback),
        { 0 }
    };
	return QISearch(this, qit, riid, ppv);
}

ULONG MFAsyncCallback::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG MFAsyncCallback::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

//*************** IMFAsyncCallback ***************\\

HRESULT MFAsyncCallback::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
{
	// Implementation of this method is optional.
	return E_NOTIMPL; 
}