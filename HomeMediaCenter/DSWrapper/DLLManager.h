#pragma once

#if !defined(DLLMANAGER_DSWRAPPER_INCLUDED)
#define DLLMANAGER_DSWRAPPER_INCLUDED

struct DLLManager
{
	typedef HRESULT (STDAPICALLTYPE * FN_DLLGETCLASSOBJECT)(REFCLSID clsid, REFIID iid, void ** ppv);

	public:		DLLManager(void) : m_hmcencoder(LoadLibrary(L".\\HMCEncoder.dll")) { }

				~DLLManager(void)
				{
					FreeLibrary(this->m_hmcencoder);
				}

				HRESULT CreateHMCEncoder(REFCLSID clsid, IUnknown ** ppUnk)
				{
					if (!this->m_hmcencoder)
						return E_FAIL;
					
					return CreateObjectFromPath(this->m_hmcencoder, clsid, ppUnk);
				}

				static DLLManager & GetManager(void)
				{
					return manager;
				}

	private:	HRESULT CreateObjectFromPath(HMODULE module, REFCLSID clsid, IUnknown ** ppUnk)
				{
					FN_DLLGETCLASSOBJECT fn = (FN_DLLGETCLASSOBJECT)GetProcAddress(module, "DllGetClassObject");
					if (fn == NULL)
						return HRESULT_FROM_WIN32(GetLastError());

					IUnknown * pUnk = NULL;
					HRESULT hr = fn(clsid, IID_IUnknown, (void **)(IUnknown **)&pUnk);
					if (SUCCEEDED(hr))
					{
						IClassFactory * pCF = (IClassFactory *)pUnk;
						if (pCF == NULL)
							hr = E_NOINTERFACE;
						else
							hr = pCF->CreateInstance(NULL, IID_IUnknown, (void **)ppUnk);
					}

					SAFE_RELEASE(pUnk);

					return hr;
				}
		
				HMODULE m_hmcencoder;

				static DLLManager manager;
};

#endif //DLLMANAGER_DSWRAPPER_INCLUDED