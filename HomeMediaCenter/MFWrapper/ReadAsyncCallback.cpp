#include "StdAfx.h"
#include "ReadAsyncCallback.h"
#include "MFAsyncObject.h"


ReadAsyncCallback::ReadAsyncCallback(BYTE * pb, ULONG cb, MFStream * mfStream) :
	m_pb(pb), m_cb(cb), m_MFStream(mfStream)
{
}

ReadAsyncCallback::~ReadAsyncCallback(void)
{
}

HRESULT ReadAsyncCallback::Invoke(IMFAsyncResult * pResult)
{
	HRESULT hr = S_OK;
    IUnknown * pState = NULL;
	IUnknown * pUnk = NULL;
    IMFAsyncResult * pCallerResult = NULL;
	ULONG readed;

	CHECK_HR(hr = pResult->GetState(&pState));
	CHECK_HR(hr = pState->QueryInterface(IID_PPV_ARGS(&pCallerResult)));
	CHECK_HR(hr = pCallerResult->GetObject(&pUnk));

	CHECK_HR(hr = this->m_MFStream->Read(this->m_pb, this->m_cb, &readed));

	((MFAsyncObject<ULONG>*)pUnk)->SetParam(readed);

done:

	if (pCallerResult)
    {
        pCallerResult->SetStatus(hr);
        MFInvokeCallback(pCallerResult);
    }

	SAFE_RELEASE(pState);
	SAFE_RELEASE(pCallerResult);
	SAFE_RELEASE(pUnk);


	return S_OK;
}
