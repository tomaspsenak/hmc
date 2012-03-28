#pragma once

#if !defined(READASYNCCALLBACK_MFWRAPPER_INCLUDED)
#define READASYNCCALLBACK_MFWRAPPER_INCLUDED

#include "MFAsyncCallback.h"
#include "MFStream.h"

private class ReadAsyncCallback : public MFAsyncCallback
{
	public:		ReadAsyncCallback(BYTE * pb, ULONG cb, MFStream * mfStream);
				virtual ~ReadAsyncCallback(void);
				HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult * pResult);
	private:	BYTE * m_pb;
				ULONG m_cb;
				MFStream * m_MFStream;
};

#endif //READASYNCCALLBACK_MFWRAPPER_INCLUDED

