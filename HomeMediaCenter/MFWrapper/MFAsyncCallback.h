#pragma once

#if !defined(MFASYNCCALLBACK_MFWRAPPER_INCLUDED)
#define MFASYNCCALLBACK_MFWRAPPER_INCLUDED

private class MFAsyncCallback : public IMFAsyncCallback
{
	public:		MFAsyncCallback(void);
				virtual ~MFAsyncCallback(void);

				//IUnknown
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv);
				STDMETHODIMP_(ULONG) AddRef();
				STDMETHODIMP_(ULONG) Release();

				//IMFAsyncCallback
				HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
				virtual HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pResult) = 0;

	private:	long m_cRef;
};

#endif //MFASYNCCALLBACK_MFWRAPPER_INCLUDED

