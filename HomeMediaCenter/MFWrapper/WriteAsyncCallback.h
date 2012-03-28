#pragma once

#if !defined(WRITEASYNCCALLBACK_MFWRAPPER_INCLUDED)
#define WRITEASYNCCALLBACK_MFWRAPPER_INCLUDED

#include "MFAsyncCallback.h"
#include "MFStream.h"

private class WriteAsyncCallback : public MFAsyncCallback
{
	public:		WriteAsyncCallback(const BYTE * pb, ULONG cb, MFStream * mfStream);
				virtual ~WriteAsyncCallback(void);
				HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult * pResult);
	private:	const BYTE * m_pb;
				ULONG m_cb;
				MFStream * m_MFStream;
};

#endif //WRITEASYNCCALLBACK_MFWRAPPER_INCLUDED

