#pragma once

#if !defined(DESKTOPSOURCEPIN_DSWRAPPER_INCLUDED)
#define DESKTOPSOURCEPIN_DSWRAPPER_INCLUDED

#include <wmcodecdsp.h>

private class DesktopSourcePin : public CSourceStream
{
	public:		DesktopSourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, UINT32 fps);
				~DesktopSourcePin(void);

				HRESULT GetMediaType(int iPosition, CMediaType * pmt);
				HRESULT CheckMediaType(const CMediaType * pMediaType);
				HRESULT SetMediaType(const CMediaType * pMediaType);
				HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);
				HRESULT FillBuffer(IMediaSample * pSample);

				STDMETHODIMP Notify(IBaseFilter * pSelf, Quality q)
				{
					return E_FAIL;
				}

	protected:	HRESULT DoBufferProcessingLoop(void);

	private:	RECT GetDesktopResolution(void);
				void GetWidthHeight(const AM_MEDIA_TYPE * type, LONG * width, LONG * height); 
				void GetBitmapInfo(const AM_MEDIA_TYPE * type, BITMAPINFOHEADER * bitmapInfo);
		
				REFERENCE_TIME m_rtLastFrame;
				const REFERENCE_TIME m_rtFrameLength;
				CCritSec m_cSharedState;
				IMediaObject * m_colorConverter;
				BITMAPINFOHEADER m_bitmapInfo;
};

private class CMediaBuffer : public IMediaBuffer
{
	private:	CMediaBuffer(HRESULT * phr, DWORD cbMaxLength, BYTE * pbData  = NULL);
				~CMediaBuffer();

				const DWORD m_cbMaxLength;
				DWORD m_cbLength;
				LONG m_nRefCount;
				BOOL m_deleteBuf;
				BYTE * m_pbData;
  
	public:		static HRESULT Create(IMediaBuffer ** ppBuffer, long cbMaxLen, BYTE * pbData  = NULL);
  
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
				STDMETHODIMP SetLength(DWORD cbLength);
				STDMETHODIMP GetMaxLength(DWORD * pcbMaxLength);
				STDMETHODIMP GetBufferAndLength(BYTE ** ppbBuffer, DWORD * pcbLength);
};

#endif //DESKTOPSOURCEPIN_DSWRAPPER_INCLUDED

