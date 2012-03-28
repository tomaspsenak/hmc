#pragma once

#if !defined(MFASYNCOBJECT_MFWRAPPER_INCLUDED)
#define MFASYNCOBJECT_MFWRAPPER_INCLUDED

template<class T> private class MFAsyncObject : public IUnknown
{
	public:		MFAsyncObject(T param);
				virtual ~MFAsyncObject(void);

				//IUnknown
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv);
				STDMETHODIMP_(ULONG) AddRef();
				STDMETHODIMP_(ULONG) Release();

				//MFAsyncObject
				T GetParam(void);
				void SetParam(T param);

	private:	long m_cRef;
				T m_param;
};

template<class T> MFAsyncObject<T>::MFAsyncObject(T param) : m_cRef(1), m_param(param)
{
}

template<class T> MFAsyncObject<T>::~MFAsyncObject(void)
{
}

//*************** IUnknown ***************\\

template<class T> HRESULT MFAsyncObject<T>::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
    {
        QITABENT(MFAsyncObject, IUnknown),
        { 0 }
    };
	return QISearch(this, qit, riid, ppv);
}

template<class T> ULONG MFAsyncObject<T>::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

template<class T> ULONG MFAsyncObject<T>::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

//*************** MFAsyncObject ***************\\

template<class T> T MFAsyncObject<T>::GetParam(void)
{
	return this->m_param;
}

template<class T> void MFAsyncObject<T>::SetParam(T param)
{
	this->m_param = param;
}

#endif //MFASYNCOBJECT_MFWRAPPER_INCLUDED
