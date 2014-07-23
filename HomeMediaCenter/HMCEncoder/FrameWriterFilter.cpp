#include "StdAfx.h"
#include "FrameWriterFilter.h"
#include <dvdmedia.h>

FrameWriterFilter::FrameWriterFilter(LPUNKNOWN punk, HRESULT * phr) : 
	CBaseRenderer(CLSID_HMCFrameWriter, L"HMCFrameWriterFilter", punk, phr), m_encoder(NULL), m_params(NULL), m_fileName(NULL), m_hFile(NULL)
{
	this->m_encoder = new AVEncoder();
	if (this->m_encoder == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}

	this->m_params = new FrameWriterParameters(this);
	if (this->m_params == NULL)
	{
		*phr = E_OUTOFMEMORY;
		return;
	}
}

FrameWriterFilter::~FrameWriterFilter(void)
{
	if (this->m_encoder)
		delete this->m_encoder;
	if (this->m_params)
		delete this->m_params;
	if (this->m_fileName)
		CoTaskMemFree(this->m_fileName);
	if (this->m_hFile)
		CloseHandle(this->m_hFile);
}

STDMETHODIMP FrameWriterFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IHMCFrameWriter)
	{
		return GetInterface(this->m_params, ppv);
	}
	else if (riid == IID_ISpecifyPropertyPages)
	{
		return GetInterface((ISpecifyPropertyPages*) this, ppv);
	}
	else if (riid == IID_IFileSinkFilter)
	{
		return GetInterface((IFileSinkFilter*) this, ppv);
	}

	return CBaseRenderer::NonDelegatingQueryInterface(riid, ppv);
}


//********** ISpecifyPropertyPages **********\\

STDMETHODIMP FrameWriterFilter::GetPages(CAUUID * pPages)
{
	CheckPointer(pPages, E_POINTER);

	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL)
	{
		pPages->cElems = 0;
		return E_OUTOFMEMORY;
	}

	pPages->cElems = 1;
	pPages->pElems[0] = CLSID_HMCFrameWriterPage;

	return S_OK;
}

//************* IFileSinkFilter *************\\

HRESULT FrameWriterFilter::GetCurFile(LPOLESTR * ppszFileName, AM_MEDIA_TYPE * pmt)
{
	if (!this->m_fileName)
		return E_FAIL;

	const size_t len = wcslen(this->m_fileName) + 1;
	*ppszFileName = (LPOLESTR)CoTaskMemAlloc(len * sizeof(OLECHAR));
	if (!(*ppszFileName))
		return E_FAIL;

	wcscpy_s(*ppszFileName, len, this->m_fileName);

	ZeroMemory(pmt, sizeof(AM_MEDIA_TYPE));

	return S_OK;
}

HRESULT FrameWriterFilter::SetFileName(LPCOLESTR pszFileName, const AM_MEDIA_TYPE * pmt)
{
	if (this->m_fileName)
		CoTaskMemFree(this->m_fileName);	
	
	const size_t len = wcslen(pszFileName) + 1;
	this->m_fileName = (LPOLESTR)CoTaskMemAlloc(len * sizeof(OLECHAR));
	if (!this->m_fileName)
		return E_FAIL;

	wcscpy_s(this->m_fileName, len, pszFileName);

	return S_OK;
}

//************** CBaseRenderer **************\\

HRESULT FrameWriterFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cAutoLock(this->m_pLock);

	HRESULT hr = S_OK;
	bool hasVideo;
	AM_MEDIA_TYPE vmt;
	ZeroMemory(&vmt, sizeof(AM_MEDIA_TYPE));

	hasVideo = (this->m_pInputPin->ConnectionMediaType(&vmt) == S_OK);

	CHECK_HR(hr = this->m_encoder->Start(&this->m_params->m_params, NULL, (hasVideo) ? &vmt : NULL, OutBufferSize, 
		this, ReadPackets, WritePackets, Seek));

	this->m_firstSample = TRUE;

	CHECK_HR(hr = CBaseRenderer::Run(tStart));

done:

	FreeMediaType(vmt);

	return hr;
}

HRESULT FrameWriterFilter::Stop(void)
{
	this->m_encoder->Stop(FALSE);

	CloseHandle(this->m_hFile);
	this->m_hFile = NULL;

	return CBaseRenderer::Stop();
}

HRESULT FrameWriterFilter::CheckMediaType(const CMediaType * pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype == MEDIATYPE_Video)
	{
		if (pmt->subtype == MEDIASUBTYPE_YUY2 || pmt->subtype == MEDIASUBTYPE_UYVY || pmt->subtype == MEDIASUBTYPE_RGB24 || 
			pmt->subtype == MEDIASUBTYPE_YV12)
		{
			if (pmt->formattype == FORMAT_VideoInfo)
			{
				if (pmt->subtype == MEDIASUBTYPE_YV12)
				{
					VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *)pmt->pbFormat;
					if (vih->bmiHeader.biPlanes == 3 && vih->bmiHeader.biCompression == MAKEFOURCC('Y','V','1','2'))
						return S_OK;
				}
				else
				{
					return S_OK;
				}
			}
			else if (pmt->formattype == FORMAT_VideoInfo2)
			{
				if (pmt->subtype == MEDIASUBTYPE_YV12)
				{
					VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *)pmt->pbFormat;
					if (vih->bmiHeader.biPlanes == 3 && vih->bmiHeader.biCompression == MAKEFOURCC('Y','V','1','2'))
						return S_OK;
				}
				else
				{
					return S_OK;
				}
			}
		}
	}

	return VFW_E_NO_ACCEPTABLE_TYPES;
}

HRESULT FrameWriterFilter::DoRenderSample(IMediaSample * pMediaSample)
{
	CheckPointer(pMediaSample, E_POINTER);
	BYTE * buffer;
	HRESULT hr;

	if (!this->m_firstSample)
	{
		hr = E_FAIL;
		goto done;
	}

	//Ignorovat sample so zapornym timeStart - napr. po seekovani treba zaciatok vynechat
	REFERENCE_TIME timeStart = 0, timeEnd;
	CHECK_HR(hr = pMediaSample->GetTime(&timeStart, &timeEnd));
	if (timeStart < 0)
	{
		hr = S_OK;
		goto done;
	}

	long length = pMediaSample->GetActualDataLength();
	CHECK_HR(hr = pMediaSample->GetPointer(&buffer));

	if (!this->m_hFile)
		this->m_hFile = CreateFile(this->m_fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (this->m_hFile == INVALID_HANDLE_VALUE)
	{
		hr = E_FAIL;
		goto done;
	}
	
	CHECK_HR(this->m_encoder->EncodeVideo(buffer, length));

	this->m_firstSample = FALSE;
	this->NotifyEvent(EC_ERRORABORT, S_FALSE, NULL);

done:
	return hr;
}

//*********** FrameWriterFilter ************\\

CUnknown * WINAPI FrameWriterFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    FrameWriterFilter * filter = new FrameWriterFilter(pUnk, phr);
    if (filter == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return filter;
}

//***************    Private    ***************\\

int FrameWriterFilter::WritePackets(void * opaque, uint8_t * buf, int buf_size)
{
	FrameWriterFilter * pThis = (FrameWriterFilter *)opaque;

	DWORD dwBytesWritten = 0;
	WriteFile(pThis->m_hFile, buf, buf_size, &dwBytesWritten, NULL);
	
	return dwBytesWritten;
}

int FrameWriterFilter::ReadPackets(void * opaque, uint8_t * buf, int buf_size)
{
	FrameWriterFilter * pThis = (FrameWriterFilter *)opaque;

	return 0;
}

int64_t FrameWriterFilter::Seek(void * opaque, int64_t offset, int whence)
{
	FrameWriterFilter * pThis = (FrameWriterFilter *)opaque;

	return 0;
}