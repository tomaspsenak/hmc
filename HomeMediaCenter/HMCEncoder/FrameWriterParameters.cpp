#include "StdAfx.h"
#include "FrameWriterParameters.h"
#include "FrameWriterFilter.h"

FrameWriterParameters::FrameWriterParameters(FrameWriterFilter * filter) : CUnknown(L"FrameWriterParameters", filter->GetOwner()), 
	m_filter(filter)
{
	SetFormat(ImageFormat_JPEG);
}

FrameWriterParameters::~FrameWriterParameters(void)
{
}

HRESULT FrameWriterParameters::SetFormat(enum ImageFormat format)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (format)
	{
		case ImageFormat_BMP:
			strcpy_s(this->m_params.m_containerStr, "image2pipe");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = AV_CODEC_ID_BMP;

			this->m_params.m_videoBitrate = 0;

			break;
		case ImageFormat_JPEG:
			strcpy_s(this->m_params.m_containerStr, "image2pipe");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = AV_CODEC_ID_MJPEG;

			this->m_params.m_videoBitrate = 4000 * 1000;

			break;
		case ImageFormat_PNG:
			strcpy_s(this->m_params.m_containerStr, "image2pipe");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = AV_CODEC_ID_PNG;

			this->m_params.m_videoBitrate = 0;

			break;
		default:
			return E_FAIL;
	}

	this->m_format = format;
	this->m_params.m_height = 0;
	this->m_params.m_width = 0;

	this->m_params.m_videoQuality = 0;
	this->m_params.m_videoBitrateMax = 0;
	this->m_params.m_videoBitrateMin = 0;
	this->m_params.m_videoMode = BitrateMode_CBR;

	this->m_params.m_streamable = true;
	this->m_params.m_videoInterlaced = false;
	this->m_params.m_videoBFrames = false;
	this->m_params.m_videoGopSize = 15;
	this->m_params.m_videoBufferSize = 224 * 1024 * 8;

	return S_OK;
}

HRESULT FrameWriterParameters::GetFormat(enum ImageFormat * pFormat)
{
	CheckPointer(pFormat, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pFormat = this->m_format;

	return S_OK;
}

HRESULT FrameWriterParameters::SetWidth(UINT32 width)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (width % 8 != 0)
		return E_FAIL;

	switch (this->m_format)
	{
		case ImageFormat_BMP:
		case ImageFormat_JPEG:
		case ImageFormat_PNG:
			this->m_params.m_width = width;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT FrameWriterParameters::GetWidth(UINT32 * pWidth)
{
	CheckPointer(pWidth, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pWidth = this->m_params.m_width;

	return S_OK;
}

HRESULT FrameWriterParameters::SetHeight(UINT32 height)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (height % 8 != 0)
		return E_FAIL;

	switch (this->m_format)
	{
		case ImageFormat_BMP:
		case ImageFormat_JPEG:
		case ImageFormat_PNG:
			this->m_params.m_height = height;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT FrameWriterParameters::GetHeight(UINT32 * pHeight)
{
	CheckPointer(pHeight, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pHeight = this->m_params.m_height;

	return S_OK;
}

HRESULT FrameWriterParameters::SetBitrate(int imageBitrate)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_format)
	{
		case ImageFormat_JPEG:
			this->m_params.m_videoBitrate = imageBitrate;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT FrameWriterParameters::GetBitrate(int * pImageBitrate)
{
	CheckPointer(pImageBitrate, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pImageBitrate = this->m_params.m_videoBitrate;

	return S_OK;
}