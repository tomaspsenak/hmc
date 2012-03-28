#include "StdAfx.h"
#include "WriteAsyncCallback.h"
#include "MFAsyncObject.h"


WriteAsyncCallback::WriteAsyncCallback(const BYTE * pb, ULONG cb, MFStream * mfStream) :
	m_pb(pb), m_cb(cb), m_MFStream(mfStream)
{
}


WriteAsyncCallback::~WriteAsyncCallback(void)
{
}

HRESULT WriteAsyncCallback::Invoke(IMFAsyncResult * pResult)
{
	HRESULT hr = S_OK;
    IUnknown * pState = NULL;
	IUnknown * pUnk = NULL;
    IMFAsyncResult * pCallerResult = NULL;
	ULONG written;

	CHECK_HR(hr = pResult->GetState(&pState));
	CHECK_HR(hr = pState->QueryInterface(IID_PPV_ARGS(&pCallerResult)));
	CHECK_HR(hr = pCallerResult->GetObject(&pUnk));

	CHECK_HR(hr = this->m_MFStream->Write(this->m_pb, this->m_cb, &written));

	((MFAsyncObject<ULONG>*)pUnk)->SetParam(written);

done:

	if (pCallerResult)
    {
        pCallerResult->SetStatus(hr);
        MFInvokeCallback(pCallerResult);
    }

	SAFE_RELEASE(pState);
	SAFE_RELEASE(pCallerResult);
	SAFE_RELEASE(pUnk);

	return hr;
}
