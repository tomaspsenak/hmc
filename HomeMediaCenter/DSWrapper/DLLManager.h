#pragma once

#if !defined(DLLMANAGER_DSWRAPPER_INCLUDED)
#define DLLMANAGER_DSWRAPPER_INCLUDED

struct DLLManager
{
	typedef HRESULT (STDAPICALLTYPE * FN_DLLGETCLASSOBJECT)(REFCLSID clsid, REFIID iid, void ** ppv);

	public:		DLLManager(void) : m_webmmux(LoadLibrary(L".\\webmmux.dll")), m_vp8encoder(LoadLibrary(L".\\vp8encoder.dll")), 
					m_hmcencoder(LoadLibrary(L".\\HMCEncoder.dll")), m_vorbisencoder(LoadLibrary(L".\\dsfVorbisEncoder.dll")) { }

				~DLLManager(void)
				{
					FreeLibrary(this->m_webmmux);
					FreeLibrary(this->m_vp8encoder);
					FreeLibrary(this->m_vorbisencoder);
					FreeLibrary(this->m_hmcencoder);
				}

				HRESULT CreateWebmmux(REFCLSID clsid, IUnknown ** ppUnk)
				{
					if (!this->m_webmmux)
						return E_FAIL;
					
					return CreateObjectFromPath(this->m_webmmux, clsid, ppUnk);
				}

				HRESULT CreateVP8Encoder(REFCLSID clsid, IUnknown ** ppUnk)
				{
					if (!this->m_vp8encoder)
						return E_FAIL;
					
					return CreateObjectFromPath(this->m_vp8encoder, clsid, ppUnk);
				}

				HRESULT CreateHMCEncoder(REFCLSID clsid, IUnknown ** ppUnk)
				{
					if (!this->m_hmcencoder)
						return E_FAIL;
					
					return CreateObjectFromPath(this->m_hmcencoder, clsid, ppUnk);
				}

				HRESULT CreateVorbisEncoder(REFCLSID clsid, IUnknown ** ppUnk)
				{
					if (!this->m_vorbisencoder)
						return E_FAIL;
					
					return CreateObjectFromPath(this->m_vorbisencoder, clsid, ppUnk);
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
		
				HMODULE m_webmmux;
				HMODULE m_vp8encoder;
				HMODULE m_vorbisencoder;
				HMODULE m_hmcencoder;
};

#endif //DLLMANAGER_DSWRAPPER_INCLUDED