#include "StdAfx.h"
#include "DesktopSourcePin.h"
#include "DibHelper.h"
#include <dvdmedia.h>
#include "DesktopSourceFilter.h"

DesktopSourcePin::DesktopSourcePin(TCHAR * pObjectName, HRESULT * phr, CSource * pms, LPCWSTR pName, UINT32 fps) 
	: CSourceStream(pObjectName, phr, pms, pName), m_rtLastFrame(0), m_rtFrameLength(UNITS / fps), m_colorConverter(NULL), m_nextTick(0), m_lastSync(0)
{
	//Ak sa neda vytvorit ColorConvertDMO, bude NULL - bude sa pouzivat iba RGB
	CoCreateInstance(__uuidof(CColorConvertDMO), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMediaObject), (void**)&this->m_colorConverter);
}

DesktopSourcePin::~DesktopSourcePin(void)
{
	SAFE_RELEASE(this->m_colorConverter);
}

HRESULT DesktopSourcePin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
    
	HRESULT hr = S_OK;
	RECT screenRect;
	DMO_MEDIA_TYPE dmoType;
	DWORD dmoSize = 0, dmoAlign = 0;
	VIDEOINFOHEADER * pvi = NULL;

	SetRectEmpty(&screenRect);
	dmoType.cbFormat = 0;
	dmoType.pUnk = NULL;

	CAutoLock cAutoLock(m_pFilter->pStateLock());

    pvi = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
    if(NULL == pvi)
        CHECK_HR(hr = E_OUTOFMEMORY);
    ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));

    if (iPosition == 0 || this->m_colorConverter != NULL)
	{
		//Ak je vytvoreny colorConverter, nainicializuje sa pre neho vstupna hlavicka
		//Inak sa je k dispozicii iba RBG32 a RBG24
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 32;
		pmt->SetSubtype(&MEDIASUBTYPE_RGB32);
    }
	else if (iPosition == 1)
    {
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 24;
		pmt->SetSubtype(&MEDIASUBTYPE_RGB24);
    }
	else
	{
		CHECK_HR(hr = VFW_S_NO_MORE_ITEMS);
	}

    screenRect = GetDesktopResolution();

    pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth = screenRect.right  - screenRect.left;
    pvi->bmiHeader.biHeight = screenRect.bottom - screenRect.top;
    pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biClrImportant = 0;
    pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	pvi->AvgTimePerFrame = this->m_rtFrameLength;

    pmt->SetType(&MEDIATYPE_Video);
	pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);
    pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

	if (this->m_colorConverter != NULL)
	{
		//Ak je vytvoreny colorConverter, budu sa brat jeho typy
		CHECK_HR(hr = this->m_colorConverter->SetInputType(0, pmt, 0));

		//bitmapInfo je pre colorConverter rovnake, inak sa nastavi v SetMediaType
		GetBitmapInfo(pmt, &this->m_bitmapInfo);

		if (this->m_colorConverter->GetOutputType(0, iPosition, &dmoType) != S_OK)
			CHECK_HR(hr = VFW_S_NO_MORE_ITEMS);

		CHECK_HR(hr = this->m_colorConverter->SetOutputType(0, &dmoType, 0));
		CHECK_HR(hr = this->m_colorConverter->GetOutputSizeInfo(0, &dmoSize, &dmoAlign));

		//Skopiruje vlastnosti colorConverter-a do vystupneho parametra
		pmt->ResetFormatBuffer();
		CopyMediaType(pmt, &dmoType);
		pmt->SetSampleSize(dmoSize);
	}

done:

	FreeMediaType(dmoType);

    return hr;
}

HRESULT DesktopSourcePin::SetMediaType(const CMediaType * pMediaType)
{
	HRESULT hr = S_OK;

    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CHECK_HR(hr = CSourceStream::SetMediaType(pMediaType));

	if (this->m_colorConverter == NULL)
		GetBitmapInfo(pMediaType, &this->m_bitmapInfo);
	else
		CHECK_HR(hr = this->m_colorConverter->SetOutputType(0, pMediaType, 0));

done:    
	return hr;
}

HRESULT DesktopSourcePin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	HRESULT hr = S_OK;
	GUID subType;
	RECT screenRect;
	LONG width = 0, height = 0;

	SetRectEmpty(&screenRect);

    if((*pMediaType->Type() != MEDIATYPE_Video) || (!pMediaType->IsFixedSize()) || (pMediaType->Subtype() == NULL) || (pMediaType->Format() == NULL))
		CHECK_HR(hr = E_INVALIDARG);                                             

    subType = *pMediaType->Subtype();

	if (this->m_colorConverter == NULL)
	{
		if (subType != MEDIASUBTYPE_RGB24 && subType != MEDIASUBTYPE_RGB32)
			CHECK_HR(hr = E_INVALIDARG);
	}
	else
	{
		CHECK_HR(hr = this->m_colorConverter->SetOutputType(0, pMediaType, DMO_SET_TYPEF_TEST_ONLY));
	}

	GetWidthHeight(pMediaType, &width, &height);
	screenRect = GetDesktopResolution();

	//Otestuje, ci sa nezmenilo rozlisenie obrazovky
    if((width != (screenRect.right - screenRect.left)) || (abs(height) != (screenRect.bottom - screenRect.top)))
        CHECK_HR(hr = E_INVALIDARG);

    if (height < 0)
        CHECK_HR(hr = E_INVALIDARG);

done:
    return hr;
}

HRESULT DesktopSourcePin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
{
    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);

	HRESULT hr = S_OK;
	ALLOCATOR_PROPERTIES aProp;

    CAutoLock cAutoLock(m_pFilter->pStateLock());

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_mt.lSampleSize;
    CHECK_SUCCEED(hr = pAlloc->SetProperties(pProperties, &aProp));

    if(aProp.cbBuffer < pProperties->cbBuffer)
		CHECK_HR(hr = E_FAIL);

done:
    return hr;
}

HRESULT DesktopSourcePin::FillBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	RECT screenRect;
	BYTE * sampleData, * inBuffer;
    DWORD sampleDataCount, status, nowTick;
	ULONG writeCount = 0;

	DMO_MEDIA_TYPE dmoType;
	IMediaBuffer * inMediaBuffer = NULL;
	DMO_OUTPUT_DATA_BUFFER outMediaBuffers[1];

	SetRectEmpty(&screenRect);
	dmoType.cbFormat = 0;
	dmoType.pUnk = NULL;
	outMediaBuffers[0].pBuffer = NULL;

    CAutoLock cAutoLockShared(&m_cSharedState);

	if (this->m_colorConverter == NULL)
	{
		dmoType = m_mt;
		writeCount = m_mt.lSampleSize;
	}
	else
	{
		CHECK_HR(hr = this->m_colorConverter->GetInputCurrentType(0, &dmoType));
	}

	screenRect = GetDesktopResolution();

	//Otestuje, ci sa nezmenilo rozlisenie obrazovky
    if((this->m_bitmapInfo.biWidth != (screenRect.right - screenRect.left)) || (abs(this->m_bitmapInfo.biHeight) != (screenRect.bottom - screenRect.top)))
        CHECK_HR(hr = E_INVALIDARG);

    CHECK_HR(hr = pSample->GetPointer(&sampleData));
    sampleDataCount = pSample->GetSize();

	if (this->m_colorConverter == NULL)
	{
		inBuffer = sampleData;
	}
	else
	{
		//Ak sa pouziva colorConverter, vytvorit vstupny a vystupy buffer pre neho
		CMediaBuffer::Create(&inMediaBuffer, dmoType.lSampleSize);
		CMediaBuffer::Create(&outMediaBuffers[0].pBuffer, sampleDataCount, sampleData);

		CHECK_HR(hr = inMediaBuffer->GetBufferAndLength(&inBuffer, NULL));
		CHECK_HR(inMediaBuffer->SetLength(dmoType.lSampleSize));
	}

	//Spomalenie snimkovania podla stanoveneho FPS
	nowTick = GetTickCount();
	if (nowTick < this->m_nextTick)
		Sleep((DWORD)(this->m_nextTick - nowTick));
	this->m_nextTick = (DWORD)(this->m_nextTick + (this->m_rtFrameLength / 10000));

	//Skopirovanie plochy do buffera
    HDIB hDib = CopyScreenToBitmap(&screenRect, inBuffer, (BITMAPINFO *)&this->m_bitmapInfo);
	if (hDib)
        DeleteObject(hDib);
	
	if (this->m_colorConverter != NULL)
	{
		//Ak sa pouziva colorConverter, konvertuj do pozadovaneho nekomprimovaneho formatu
		CHECK_HR(hr = this->m_colorConverter->ProcessInput(0, inMediaBuffer, 0, 0, 0));
		CHECK_HR(hr = this->m_colorConverter->ProcessOutput(0, 1, outMediaBuffers, &status));
		outMediaBuffers[0].pBuffer->GetBufferAndLength(NULL, &writeCount);
	}

	//Synchronizacia audio / video
	if (nowTick > (this->m_lastSync + DesktopSourceFilter::SyncTime))
	{
		((DesktopSourceFilter*)this->m_pFilter)->SyncPins(0, this->m_rtLastFrame);
		this->m_lastSync = this->m_nextTick = nowTick = GetTickCount();
	}

    REFERENCE_TIME rtStart = this->m_rtLastFrame;
    REFERENCE_TIME rtStop  = rtStart + this->m_rtFrameLength;
	this->m_rtLastFrame = rtStop;

    CHECK_HR(hr = pSample->SetTime(&rtStart, &rtStop));
    CHECK_HR(hr = pSample->SetSyncPoint(TRUE));
	CHECK_HR(hr = pSample->SetActualDataLength(writeCount));
	
done:

	FreeMediaType(dmoType);
	SAFE_RELEASE(inMediaBuffer);
	SAFE_RELEASE(outMediaBuffers[0].pBuffer);

    return hr;
}

//***************    Private    ***************\\

RECT DesktopSourcePin::GetDesktopResolution(void)
{
	RECT screen;
	HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    screen.left = screen.top = 0;
    screen.right = GetDeviceCaps(hDC, HORZRES);
    screen.bottom = GetDeviceCaps(hDC, VERTRES);

    DeleteDC(hDC);

	return screen;
}

void DesktopSourcePin::GetWidthHeight(const AM_MEDIA_TYPE * type, LONG * width, LONG * height)
{
	BITMAPINFOHEADER info;
	GetBitmapInfo(type, &info);

	*width = info.biWidth;
	*height = info.biHeight;
}

void DesktopSourcePin::GetBitmapInfo(const AM_MEDIA_TYPE * type, BITMAPINFOHEADER * bitmapInfo)
{
	if (type->formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *)type->pbFormat;
		*bitmapInfo = vih->bmiHeader;
	}
	else if (type->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *)type->pbFormat;
		*bitmapInfo = vih->bmiHeader;
	}
}

//*************** CMediaBuffer ***************\\
//***************    Private    ***************\\

CMediaBuffer::CMediaBuffer(HRESULT * phr, DWORD cbMaxLength, BYTE * pbData) :
	  m_nRefCount(1), m_cbMaxLength(cbMaxLength), m_cbLength(0), m_pbData(pbData), m_deleteBuf(FALSE)
{
	if (pbData == NULL)
	{
		this->m_pbData = new BYTE[cbMaxLength];
		this->m_deleteBuf = TRUE;
	}

	if (this->m_pbData == NULL) 
		*phr = E_OUTOFMEMORY;
}

CMediaBuffer::~CMediaBuffer()
{
    if (this->m_pbData != NULL && this->m_deleteBuf)
        delete [] this->m_pbData;
	
	this->m_pbData = NULL;
}

//***************    Public    ***************\\

HRESULT CMediaBuffer::Create(IMediaBuffer ** ppBuffer, long cbMaxLen, BYTE * pbData)
{
    HRESULT hr = S_OK;
    CMediaBuffer * pBuffer = NULL;
  
    if (ppBuffer == NULL)
        return E_POINTER;
  
    pBuffer = new CMediaBuffer(&hr, cbMaxLen, pbData);
  
    if (pBuffer == NULL)
        hr = E_OUTOFMEMORY;
  
    if (SUCCEEDED(hr))
    {
        *ppBuffer = pBuffer;
        (*ppBuffer)->AddRef();
    }
  
    if (pBuffer)
        pBuffer->Release();

    return hr;
}

STDMETHODIMP CMediaBuffer::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == NULL) 
    {
        return E_POINTER;
    }
    else if (riid == __uuidof(IMediaBuffer) || riid == IID_IUnknown) 
    {
        *ppv = static_cast<IMediaBuffer *>(this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP CMediaBuffer::SetLength(DWORD cbLength)
{
    if (cbLength > this->m_cbMaxLength) 
        return E_INVALIDARG;
    this->m_cbLength = cbLength;

    return S_OK;
}

STDMETHODIMP CMediaBuffer::GetMaxLength(DWORD * pcbMaxLength)
{
    if (pcbMaxLength == NULL) 
        return E_POINTER;
    *pcbMaxLength = this->m_cbMaxLength;

    return S_OK;
}

STDMETHODIMP CMediaBuffer::GetBufferAndLength(BYTE ** ppbBuffer, DWORD * pcbLength)
{
    if (ppbBuffer == NULL && pcbLength == NULL) 
        return E_POINTER;
    if (ppbBuffer) 
        *ppbBuffer = this->m_pbData;
    if (pcbLength) 
        *pcbLength = this->m_cbLength;

    return S_OK;
}
