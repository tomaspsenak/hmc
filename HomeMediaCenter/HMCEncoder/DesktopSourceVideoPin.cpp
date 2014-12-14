#include "StdAfx.h"
#include "DesktopSourceVideoPin.h"
#include <dvdmedia.h>
#include "DesktopSourceFilter.h"

DesktopSourceVideoPin::DesktopSourceVideoPin(TCHAR * pObjectName, HRESULT * phr, DesktopSourceFilter * filter, LPCWSTR pName) 
	: CSourceStream(pObjectName, phr, filter, pName), m_rtLastFrame(0), m_pFilter(filter), m_rgbBuffer(NULL)
{
	ZeroMemory(&this->m_bitmapInfo, sizeof(BITMAPINFO));
	ZeroMemory(&this->m_rgbBitmapInfo, sizeof(BITMAPINFO));
}

DesktopSourceVideoPin::~DesktopSourceVideoPin(void)
{
	if (this->m_rgbBuffer)
		delete [] this->m_rgbBuffer;
}

STDMETHODIMP DesktopSourceVideoPin::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCDesktopSource)
	{
		return GetInterface(this->m_pFilter->m_params, ppv);
	}

	return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT DesktopSourceVideoPin::GetMediaType(int iPosition, CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);
    if(iPosition < 0)
        return E_INVALIDARG;
    
	VIDEOINFOHEADER * pvi = NULL;

	CAutoLock cAutoLock(m_pFilter->pStateLock());

    pvi = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
    if(NULL == pvi)
        return E_OUTOFMEMORY;
    ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));

    if (iPosition == 0)
	{
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 32;
		pvi->bmiHeader.biPlanes = 1;
		pmt->SetSubtype(&MEDIASUBTYPE_RGB32);
    }
	else if (iPosition == 1)
    {
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 24;
		pvi->bmiHeader.biPlanes = 1;
		pmt->SetSubtype(&MEDIASUBTYPE_RGB24);
    }
	else if (iPosition == 2)
	{
		pvi->bmiHeader.biCompression = MAKEFOURCC('Y','U','Y','2');
		pvi->bmiHeader.biBitCount = 16;
		pvi->bmiHeader.biPlanes = 1;
		pmt->SetSubtype(&MEDIASUBTYPE_YUY2);
	}
	else if (iPosition == 3)
	{
		pvi->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
		pvi->bmiHeader.biBitCount = 12;
		pvi->bmiHeader.biPlanes = 3;
		pmt->SetSubtype(&MEDIASUBTYPE_YV12);
	}
	else
	{
		return VFW_S_NO_MORE_ITEMS;
	}
	
    pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    
	pvi->bmiHeader.biClrImportant = 0;

    pmt->SetType(&MEDIATYPE_Video);
	pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);

	ApplyParametersToMT(pmt);

    return S_OK;
}

HRESULT DesktopSourceVideoPin::SetMediaType(const CMediaType * pMediaType)
{
	HRESULT hr = S_OK;

    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CHECK_HR(hr = CSourceStream::SetMediaType(pMediaType));

	CHECK_HR(hr = GetBitmapInfo(pMediaType, &this->m_bitmapInfo.bmiHeader));

done:    
	return hr;
}

HRESULT DesktopSourceVideoPin::CheckMediaType(const CMediaType * pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	HRESULT hr = S_OK;
	LONG width = 0, height = 0;

	if((pMediaType->majortype != MEDIATYPE_Video) || (!pMediaType->IsFixedSize()) || (pMediaType->subtype == GUID_NULL) || (pMediaType->pbFormat == NULL))
		CHECK_HR(hr = VFW_E_NO_ACCEPTABLE_TYPES);                                             

	if (pMediaType->subtype != MEDIASUBTYPE_RGB24 && pMediaType->subtype != MEDIASUBTYPE_RGB32 && pMediaType->subtype != MEDIASUBTYPE_YUY2 && 
		pMediaType->subtype != MEDIASUBTYPE_YV12)
		CHECK_HR(hr = VFW_E_NO_ACCEPTABLE_TYPES);

	CHECK_HR(hr = GetWidthHeight(pMediaType, &width, &height));

	//Neakceptovat otoceny obraz - napriklad pre EVR
	if (height < 0)
        CHECK_HR(hr = E_INVALIDARG);

done:
    return hr;
}

HRESULT DesktopSourceVideoPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProperties)
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

	//Vytvorenie RGB buffera - pouziva sa pri konverzii na YUV
	if (this->m_rgbBuffer)
	{
		delete [] this->m_rgbBuffer;
		this->m_rgbBuffer = NULL;
		ZeroMemory(&this->m_rgbBitmapInfo, sizeof(BITMAPINFO));
	}
	if (m_mt.subtype != MEDIASUBTYPE_RGB24 && m_mt.subtype != MEDIASUBTYPE_RGB32)
	{
		CHECK_HR(hr = GetBitmapInfo(&m_mt, &this->m_rgbBitmapInfo.bmiHeader));
		this->m_rgbBitmapInfo.bmiHeader.biPlanes = 1;
		this->m_rgbBitmapInfo.bmiHeader.biCompression = BI_RGB;
		this->m_rgbBitmapInfo.bmiHeader.biBitCount = 24;
		this->m_rgbBitmapInfo.bmiHeader.biSizeImage = GetBitmapSize(&this->m_rgbBitmapInfo.bmiHeader);

		this->m_rgbBuffer = new BYTE[this->m_rgbBitmapInfo.bmiHeader.biSizeImage];
		if (!this->m_rgbBuffer)
			CHECK_HR(hr = E_OUTOFMEMORY);
	}

done:
    return hr;
}

HRESULT DesktopSourceVideoPin::FillBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	HRESULT hr = S_OK;
	BYTE * pBuffer;

    CAutoLock cAutoLockShared(&m_cSharedState);

    CHECK_HR(hr = pSample->GetPointer(&pBuffer));

	//Skopirovanie plochy do buffera
	HANDLE hDib = CopyScreenToBitmap(pBuffer, &this->m_bitmapInfo, this->m_pFilter->m_params->m_videoStretchMode,
		this->m_pFilter->m_params->m_captureCursor, this->m_pFilter->m_params->m_keepAspectRatio);
	if (hDib)
        DeleteObject(hDib);

    CHECK_HR(hr = pSample->SetSyncPoint(TRUE));
	CHECK_HR(hr = pSample->SetActualDataLength(m_mt.lSampleSize));
	
done:
    return hr;
}

HRESULT DesktopSourceVideoPin::ReconnectWithChangesSync(void)
{
	HRESULT hr = S_OK;
	CMediaType mediaType;

	CHECK_HR(hr = mediaType.Set(this->m_mt));
	CHECK_HR(hr = ApplyParametersToMT(&mediaType));

	CHECK_HR(hr = this->m_pFilter->ReconnectPinSync(this, &mediaType));

done:
	return hr;
}

//***************  Protected   ***************\\

HRESULT DesktopSourceVideoPin::DoBufferProcessingLoop(void)
{
	Command com;
	HRESULT hr = S_OK;
	DWORD lastTick = 0;
	REFERENCE_TIME rtRealTime = 0;
	IMediaSample * pSample = NULL;
	this->m_rtLastFrame = 0;

    OnThreadStartPlay();

	//Synchronizacia audio / video
	((DesktopSourceFilter*)this->m_pFilter)->SyncPins(0);

	lastTick = GetTickCount();

    do
	{
		while (!CheckRequest(&com))
		{
			BOOL isNewSample;

			if (pSample == NULL)
			{
				if (FAILED(GetDeliveryBuffer(&pSample, NULL, NULL, 0)))
				{
					Sleep(1);
					continue;	// go round again. Perhaps the error will go away
								// or the allocator is decommited & we will be asked to
								// exit soon.
				}

				isNewSample = TRUE;
			}
			else
			{
				isNewSample = FALSE;
			}

			//Spomalenie snimkovania podla stanoveneho FPS v realnom case
			DWORD tick = GetTickCount();
			rtRealTime += ((tick - lastTick) * 10000);
			lastTick = tick;

			if (this->m_rtLastFrame >= rtRealTime)
			{
				//Snimka musi pockat stanoveny cas - sleepTime
				if (!isNewSample)
				{
					SAFE_RELEASE(pSample);
					continue;
				}

				DWORD sleepTime = (DWORD)((this->m_rtLastFrame - rtRealTime) / 10000);
				Sleep(sleepTime);
			}

			if (isNewSample)
				hr = FillBuffer(pSample);
			else
				hr = S_OK;

			REFERENCE_TIME rtStart = this->m_rtLastFrame;
			REFERENCE_TIME rtStop  = rtStart + this->m_pFilter->m_params->m_rtFrameLength;
			this->m_rtLastFrame = rtStop;

			pSample->SetTime(&rtStart, &rtStop);

			if (hr == S_OK)
			{
				hr = Deliver(pSample);
				SAFE_RELEASE(pSample);

				// downstream filter returns S_FALSE if it wants us to
				// stop or an error if it's reporting an error.
				if(hr != S_OK)
				{
					DbgLog((LOG_TRACE, 2, TEXT("Deliver() returned %08x; stopping"), hr));
					return S_OK;
				}

			}
			else if (hr == S_FALSE)
			{
				// derived class wants us to stop pushing data
				SAFE_RELEASE(pSample);
				DeliverEndOfStream();
				return S_OK;
			} 
			else 
			{
				// derived class encountered an error
				SAFE_RELEASE(pSample);
				DbgLog((LOG_ERROR, 1, TEXT("Error %08lX from FillBuffer!!!"), hr));
				DeliverEndOfStream();
				m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
				return hr;
			}

			// all paths release the sample
		}

		// For all commands sent to us there must be a Reply call!

		if (com == CMD_RUN || com == CMD_PAUSE)
		{
			Reply(NOERROR);
		} 
		else if (com != CMD_STOP)
		{
			Reply((DWORD) E_UNEXPECTED);
			DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
		}
    } while (com != CMD_STOP);

	SAFE_RELEASE(pSample);

    return S_FALSE;
}

HRESULT DesktopSourceVideoPin::ApplyParametersToMT(CMediaType * pmt)
{
	HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

	LONG width = (this->m_pFilter->m_params->m_width) ? this->m_pFilter->m_params->m_width : GetDeviceCaps(hDC, HORZRES);
	LONG height = (this->m_pFilter->m_params->m_height) ? this->m_pFilter->m_params->m_height : GetDeviceCaps(hDC, VERTRES);

	VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *)pmt->pbFormat;

	pvi->rcTarget.right = pvi->rcSource.right = pvi->bmiHeader.biWidth = width;
    pvi->rcTarget.bottom = pvi->rcSource.bottom = pvi->bmiHeader.biHeight = height;
	pvi->AvgTimePerFrame = this->m_pFilter->m_params->m_rtFrameLength;
	if (pmt->subtype == MEDIASUBTYPE_YUY2)
		pvi->bmiHeader.biSizeImage = 2 * ((width * height) >> 1) + (width * height);
	else if (pmt->subtype == MEDIASUBTYPE_YV12)
		pvi->bmiHeader.biSizeImage = 2 * ((width * height) >> 2) + (width * height);
	else
		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);

	pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

	DeleteDC(hDC);

	return S_OK;
}

//***************    Private    ***************\\

HRESULT DesktopSourceVideoPin::GetWidthHeight(const AM_MEDIA_TYPE * type, LONG * width, LONG * height)
{
	BITMAPINFOHEADER info;
	if (GetBitmapInfo(type, &info) != S_OK)
		return E_FAIL;

	*width = info.biWidth;
	*height = info.biHeight;

	return S_OK;
}

HRESULT DesktopSourceVideoPin::GetBitmapInfo(const AM_MEDIA_TYPE * type, BITMAPINFOHEADER * bitmapInfo)
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
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HBITMAP DesktopSourceVideoPin::CopyScreenToBitmap(BYTE * pData, BITMAPINFO * pHeader, int stretchMode, BOOL captureCursor, BOOL keepAspectRatio)
{
    HDC         hScrDC, hMemDC;         // screen DC and memory DC
    HBITMAP     hBitmap, hOldBitmap;    // handles to deice-dependent bitmaps
    int         nWidth, nHeight;        // DIB width and height
    int         xScrn, yScrn;           // screen resolution
	CURSORINFO	pci;
	
	pci.cbSize = sizeof(CURSORINFO);

    // create a DC for the screen and create
    // a memory DC compatible to screen DC   
    hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    hMemDC = CreateCompatibleDC(hScrDC);

    // get screen resolution
    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);

	nWidth  = pHeader->bmiHeader.biWidth;
	nHeight = pHeader->bmiHeader.biHeight;

    // create a bitmap compatible with the screen DC
    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    // select new bitmap into memory DC
    hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);

	if (xScrn == nWidth && yScrn == nHeight)
	{
		// bitblt screen DC to memory DC
		BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, 0, 0, SRCCOPY);

		// write cursor to output bitmap
		if (captureCursor && GetCursorInfo(&pci))
		{
			DrawIconEx(hMemDC, pci.ptScreenPos.x, pci.ptScreenPos.y, pci.hCursor, 0, 0, 0, NULL, DI_NORMAL);
		}
	}
	else
	{
		SetStretchBltMode(hMemDC, stretchMode);

		int posX, posY, newHeight, newWidth;
		if (keepAspectRatio)
		{
			double ratio = min((double)nWidth / xScrn, (double)nHeight / yScrn);
		
			newWidth = (int)(ratio * xScrn);
			newHeight = (int)(ratio * yScrn);

			//Vypocitanie X a Y suradnice
			posX = (nWidth - newWidth) / 2;
			posY = (nHeight - newHeight) / 2;
		}
		else
		{
			posX = 0;
            posY = 0;
            newWidth = nWidth;
            newHeight = nHeight;
		}

		StretchBlt(hMemDC, posX, posY, newWidth, newHeight, hScrDC, 0, 0, xScrn, yScrn, SRCCOPY);

		// write cursor to output bitmap
		if (captureCursor && GetCursorInfo(&pci))
		{
			double ratioX = (double)newWidth / xScrn, ratioY = (double)newHeight / yScrn;
			DrawIconEx(hMemDC, (int)(ratioX * pci.ptScreenPos.x) + posX, (int)(ratioY * pci.ptScreenPos.y) + posY, pci.hCursor, 
				(int)(GetSystemMetrics(SM_CXCURSOR) * ratioX), (int)(GetSystemMetrics(SM_CYCURSOR) * ratioY), 0, NULL, DI_NORMAL);
		}
	}

    // select old bitmap back into memory DC and get handle to
    // bitmap of the screen   
    hBitmap = (HBITMAP)  SelectObject(hMemDC, hOldBitmap);

	// Copy the bitmap data into the provided BYTE buffer
	if (pHeader->bmiHeader.biCompression == BI_RGB)
	{
		GetDIBits(hScrDC, hBitmap, 0, nHeight, pData, pHeader, DIB_RGB_COLORS);
	}
	else if (pHeader->bmiHeader.biCompression == MAKEFOURCC('Y','U','Y','2'))
	{
		GetDIBits(hScrDC, hBitmap, 0, nHeight, this->m_rgbBuffer, &this->m_rgbBitmapInfo, DIB_RGB_COLORS);

		BYTE R, B, G, V;
		BYTE * pSrcBuffer;
		DWORD rgbLineSize = DIBWIDTHBYTES(this->m_rgbBitmapInfo.bmiHeader);

		for (int i = (nHeight - 1); i >= 0; i--)
		{
			pSrcBuffer = this->m_rgbBuffer + (rgbLineSize * i);

			for (int j = 0; j < nWidth; j++)
			{
				B = *pSrcBuffer; pSrcBuffer++;
				G = *pSrcBuffer; pSrcBuffer++;
				R = *pSrcBuffer; pSrcBuffer++;

				*pData = (BYTE)((0.299 * R) + (0.587 * G) + (0.114 * B)); pData++; //Y

				if (j % 2 == 0)
				{
					*pData = (BYTE)((-0.14317 * R) + (-0.28886 * G) + (0.436 * B) + 128); pData++; //U
					V = (BYTE)((0.615 * R) + (-0.51499 * G) + (-0.10001 * B) + 128); //V temp
				}
				else
				{
					*pData = V; pData++; //V
				}
			}
		}
	}
	else if (pHeader->bmiHeader.biCompression == MAKEFOURCC('Y','V','1','2'))
	{
		GetDIBits(hScrDC, hBitmap, 0, nHeight, this->m_rgbBuffer, &this->m_rgbBitmapInfo, DIB_RGB_COLORS);

		BYTE R, B, G;
		BYTE * pSrcBuffer;
		BYTE * pDataY = pData;
		BYTE * pDataV = pDataY + (nWidth * nHeight);
		BYTE * pDataU = pDataV + ((nWidth * nHeight) >> 2);
		DWORD rgbLineSize = DIBWIDTHBYTES(this->m_rgbBitmapInfo.bmiHeader);

		for (int i = (nHeight - 1); i >= 0; i--)
		{
			pSrcBuffer = this->m_rgbBuffer + (rgbLineSize * i);

			for (int j = 0; j < nWidth; j++)
			{
				B = *pSrcBuffer; pSrcBuffer++;
				G = *pSrcBuffer; pSrcBuffer++;
				R = *pSrcBuffer; pSrcBuffer++;

				*pDataY = (BYTE)((0.299 * R) + (0.587 * G) + (0.114 * B)); pDataY++; //Y

				if (i % 2 == 0 && j % 2 == 0)
				{
					*pDataV = (BYTE)((0.615 * R) + (-0.51499 * G) + (-0.10001 * B) + 128); pDataV++; //V
					*pDataU = (BYTE)((-0.14317 * R) + (-0.28886 * G) + (0.436 * B) + 128); pDataU++; //U
				}
			}
		}
	}

    // clean up
    DeleteDC(hScrDC);
    DeleteDC(hMemDC);

    // return handle to the bitmap
    return hBitmap;
}