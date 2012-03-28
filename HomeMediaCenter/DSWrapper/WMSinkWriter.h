#pragma once

#if !defined(WMSINKWRITER_DSWRAPPER_INCLUDED)
#define WMSINKWRITER_DSWRAPPER_INCLUDED

#include <wmsdk.h>

private class WMSinkWriter : public IWMWriterSink
{
	public:		WMSinkWriter(System::IO::Stream ^ outputStream);
				~WMSinkWriter(void);

				STDMETHODIMP_(ULONG) AddRef()
				{
					return InterlockedIncrement(&m_nRefCount);
				}
  
				STDMETHODIMP_(ULONG) Release()
				{
					LONG lRef = InterlockedDecrement(&m_nRefCount);
					if (lRef == 0) 
						delete this;
					return lRef;  
				}

				STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);

				STDMETHODIMP AllocateDataUnit(DWORD cbDataUnit, INSSBuffer ** ppDataUnit);
				STDMETHODIMP IsRealTime(BOOL * pfRealTime);
				STDMETHODIMP OnDataUnit(INSSBuffer * pDataUnit);
				STDMETHODIMP OnEndWriting();
				STDMETHODIMP OnHeader(INSSBuffer * pHeader);

	private:	STDMETHODIMP Write(BYTE * pv, ULONG cb);

				LONG m_nRefCount;
				gcroot<System::IO::Stream^> m_outputStream;
				CCritSec m_cSharedState;
};

private class CNSSBuffer : public INSSBuffer
{
	public:		CNSSBuffer(HRESULT * phr, DWORD cbMaxLen);
				~CNSSBuffer(void);

				STDMETHODIMP_(ULONG) AddRef()
				{
					return InterlockedIncrement(&m_nRefCount);
				}
  
				STDMETHODIMP_(ULONG) Release()
				{
					LONG lRef = InterlockedDecrement(&m_nRefCount);
					if (lRef == 0) 
						delete this;
					return lRef;  
				}

				STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
				STDMETHODIMP GetBuffer(BYTE ** ppdwBuffer);
				STDMETHODIMP GetBufferAndLength(BYTE ** ppdwBuffer, DWORD * pdwLength);
				STDMETHODIMP GetLength(DWORD * pdwLength);
				STDMETHODIMP GetMaxLength(DWORD * pdwLength);
				STDMETHODIMP SetLength(DWORD dwLength);

	private:	const DWORD m_cbMaxLength;
				DWORD m_cbLength;
				LONG m_nRefCount;
				BYTE * m_pbData;
};

#endif //WMSINKWRITER_DSWRAPPER_INCLUDED

