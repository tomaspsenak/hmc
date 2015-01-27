#include "StdAfx.h"
#include "EncoderParameters.h"
#include "EncoderFilter.h"

EncoderParameters::EncoderParameters(EncoderFilter * filter) : CUnknown(L"EncoderParameters", filter->GetOwner()), m_filter(filter)
{
	SetContainer(Container_MPEG2PS);
}

EncoderParameters::~EncoderParameters(void)
{
}

HRESULT EncoderParameters::SetContainer(enum Container container)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (container)
	{
		case Container_MPEG2PS:
			strcpy_s(this->m_params.m_containerStr, "vob");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = true;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 48000;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		case Container_MPEG2TS:
			strcpy_s(this->m_params.m_containerStr, "mpegts");
			this->m_params.m_audioCodec = AV_CODEC_ID_MP2;
			this->m_params.m_videoCodec = AV_CODEC_ID_MPEG2VIDEO;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = true;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 48000;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		case Container_MPEG2TSH264:
			strcpy_s(this->m_params.m_containerStr, "mpegts");
			this->m_params.m_audioCodec = AV_CODEC_ID_AAC;
			this->m_params.m_videoCodec = AV_CODEC_ID_H264;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_MP3:
			strcpy_s(this->m_params.m_containerStr, "mp3");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = false;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			break;
		case Container_MP4:
			strcpy_s(this->m_params.m_containerStr, "mp4");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = false;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_AVI:
			strcpy_s(this->m_params.m_containerStr, "avi");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = false;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_FLV:
			strcpy_s(this->m_params.m_containerStr, "flv");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 12;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		case Container_FLVH264:
			strcpy_s(this->m_params.m_containerStr, "flv");
			this->m_params.m_audioCodec = AV_CODEC_ID_AAC;
			this->m_params.m_videoCodec = AV_CODEC_ID_H264;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 15;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_ASF:
			strcpy_s(this->m_params.m_containerStr, "asf");
			this->m_params.m_audioCodec = AV_CODEC_ID_WMAV2;
			this->m_params.m_videoCodec = AV_CODEC_ID_WMV2;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 12;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(40 * 1000);
			this->m_params.m_audioSamplerate = 32000;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(400 * 1000);

			break;
		case Container_WEBM:
			strcpy_s(this->m_params.m_containerStr, "webm");
			this->m_params.m_audioCodec = CODEC_ID_NONE;
			this->m_params.m_videoCodec = CODEC_ID_NONE;
			this->m_params.m_streamable = true;
			this->m_params.m_videoInterlaced = false;
			this->m_params.m_videoBFrames = false;
			this->m_params.m_videoGopSize = 12;
			this->m_params.m_videoBufferSize = 224 * 1024 * 8;
			this->m_params.m_videoQuality = 50;

			SetAudioCBR(128 * 1000);
			this->m_params.m_audioSamplerate = 44100;
			this->m_params.m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		default:
			return E_FAIL;
	}

	this->m_container = container;
	this->m_params.m_height = 0;
	this->m_params.m_width = 0;
	this->m_params.m_hlsSegmentTime = 0;

	return S_OK;
}

HRESULT EncoderParameters::GetContainer(enum Container * pContainer)
{
	CheckPointer(pContainer, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pContainer = this->m_container;

	return S_OK;
}

HRESULT EncoderParameters::SetStreamable(BOOL streamable)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_MP3:
		case Container_MP4:
		case Container_FLV:
		case Container_FLVH264:
		case Container_ASF:
		case Container_WEBM:
			this->m_params.m_streamable = streamable;
			return S_OK;
		default:
			if (streamable)
				return E_FAIL;

			this->m_params.m_streamable = streamable;
			return S_OK;
	}
}

HRESULT EncoderParameters::GetStreamable(BOOL * pStreamable)
{
	CheckPointer(pStreamable, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pStreamable = this->m_params.m_streamable;

	return S_OK;
}

HRESULT EncoderParameters::SetAudioCBR(int audioBitrate)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_params.m_audioBitrate = audioBitrate;
	this->m_params.m_audioMode = BitrateMode_CBR;

	return S_OK;
}

HRESULT EncoderParameters::SetAudioVBR(int audioBitrate, int audioBitrateMax, int audioBitrateMin, int audioQuality)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_params.m_audioBitrate = audioBitrate;
	this->m_params.m_audioBitrateMax = audioBitrateMax;
	this->m_params.m_audioBitrateMin = audioBitrateMin;
	this->m_params.m_audioQuality = audioQuality;
	this->m_params.m_audioMode = BitrateMode_VBR;

	return S_OK;
}

HRESULT EncoderParameters::GetAudioBitrate(BOOL * pIsConstant, int * pAudioBitrate, int * pAudioBitrateMax, int * pAudioBitrateMin, int * pAudioQuality)
{
	CheckPointer(pIsConstant, E_POINTER);
	CheckPointer(pAudioBitrate, E_POINTER);
	CheckPointer(pAudioBitrateMax, E_POINTER);
	CheckPointer(pAudioBitrateMin, E_POINTER);
	CheckPointer(pAudioQuality, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pAudioBitrate = this->m_params.m_audioBitrate;

	if (this->m_params.m_audioMode == BitrateMode_CBR)
	{
		*pIsConstant = TRUE;
		*pAudioBitrateMax = 0;
		*pAudioBitrateMin = 0;
		*pAudioQuality = 0;
	}
	else
	{
		*pIsConstant = FALSE;
		*pAudioBitrateMax = this->m_params.m_audioBitrateMax;
		*pAudioBitrateMin = this->m_params.m_audioBitrateMin;
		*pAudioQuality = this->m_params.m_audioQuality;
	}

	return S_OK;
}

HRESULT EncoderParameters::SetVideoCBR(int videoBitrate)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_params.m_videoBitrate = videoBitrate;
	this->m_params.m_videoMode = BitrateMode_CBR;

	return S_OK;
}

HRESULT EncoderParameters::SetVideoVBR(int videoBitrate, int videoBitrateMax, int videoBitrateMin, int videoQuality)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	this->m_params.m_videoBitrate = videoBitrate;
	this->m_params.m_videoBitrateMax = videoBitrateMax;
	this->m_params.m_videoBitrateMin = videoBitrateMin;
	this->m_params.m_videoQuality = videoQuality;
	this->m_params.m_videoMode = BitrateMode_VBR;

	return S_OK;
}

HRESULT EncoderParameters::GetVideoBitrate(BOOL * pIsConstant, int * pVideoBitrate, int * pVideoBitrateMax, int * pVideoBitrateMin, int * pVideoQuality)
{
	CheckPointer(pIsConstant, E_POINTER);
	CheckPointer(pVideoBitrate, E_POINTER);
	CheckPointer(pVideoBitrateMax, E_POINTER);
	CheckPointer(pVideoBitrateMin, E_POINTER);
	CheckPointer(pVideoQuality, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pVideoBitrate = this->m_params.m_videoBitrate;

	if (this->m_params.m_videoMode == BitrateMode_CBR)
	{
		*pIsConstant = TRUE;
		*pVideoBitrateMax = 0;
		*pVideoBitrateMin = 0;
		*pVideoQuality = 0;
	}
	else
	{
		*pIsConstant = FALSE;
		*pVideoBitrateMax = this->m_params.m_videoBitrateMax;
		*pVideoBitrateMin = this->m_params.m_videoBitrateMin;
		*pVideoQuality = this->m_params.m_videoQuality;
	}

	return S_OK;
}

HRESULT EncoderParameters::SetVideoInterlace(BOOL interlaced)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_AVI:
			this->m_params.m_videoInterlaced = interlaced;
			return S_OK;
		default:
			if (interlaced)
				return E_FAIL;

			this->m_params.m_videoInterlaced = interlaced;
			return S_OK;
	}
}

HRESULT EncoderParameters::GetVideoInterlace(BOOL * interlaced)
{
	CheckPointer(interlaced, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*interlaced = this->m_params.m_videoInterlaced;

	return S_OK;
}

HRESULT EncoderParameters::SetWidth(UINT32 width)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (width % 8 != 0)
		return E_FAIL;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_AVI:
		case Container_MP4:
		case Container_FLV:
		case Container_FLVH264:
		case Container_ASF:
		case Container_WEBM:
			this->m_params.m_width = width;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT EncoderParameters::GetWidth(UINT32 * pWidth)
{
	CheckPointer(pWidth, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pWidth = this->m_params.m_width;

	return S_OK;
}

HRESULT EncoderParameters::SetHeight(UINT32 height)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (height % 8 != 0)
		return E_FAIL;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_AVI:
		case Container_MP4:
		case Container_FLV:
		case Container_FLVH264:
		case Container_ASF:
		case Container_WEBM:
			this->m_params.m_height = height;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT EncoderParameters::GetHeight(UINT32 * pHeight)
{
	CheckPointer(pHeight, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pHeight = this->m_params.m_height;

	return S_OK;
}

HRESULT EncoderParameters::SetVideoQuality(int videoQuality)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_AVI:
		case Container_MP4:
		case Container_FLV:
		case Container_FLVH264:
		case Container_ASF:
		case Container_WEBM:
			this->m_params.m_videoQuality = videoQuality;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT EncoderParameters::GetVideoQuality(int * pVideoQuality)
{
	CheckPointer(pVideoQuality, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pVideoQuality = this->m_params.m_videoQuality;

	return S_OK;
}

HRESULT EncoderParameters::SetVideoBFrames(BOOL bFrames)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_AVI:
		case Container_MP4:
		case Container_FLVH264:
		case Container_WEBM:
			this->m_params.m_videoBFrames = bFrames;
			return S_OK;
		default:
			if (bFrames)
				return E_FAIL;

			this->m_params.m_videoBFrames = bFrames;
			return S_OK;
	}
}

HRESULT EncoderParameters::GetVideoBFrames(BOOL * bFrames)
{
	CheckPointer(bFrames, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*bFrames = this->m_params.m_videoBFrames;

	return S_OK;
}

HRESULT EncoderParameters::SetVideoGopSize(int gopSize)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MPEG2TSH264:
		case Container_AVI:
		case Container_MP4:
		case Container_FLV:
		case Container_FLVH264:
		case Container_WEBM:
			this->m_params.m_videoGopSize = gopSize;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT EncoderParameters::GetVideoGopSize(int * gopSize)
{
	CheckPointer(gopSize, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*gopSize = this->m_params.m_videoGopSize;

	return S_OK;
}

HRESULT EncoderParameters::SetHlsSegmenter(const char * pPlaylistUrl, const char * pFileUrl, UINT32 segmentTime, BOOL enabled)
{
	CAutoLock cObjectLock(this->m_filter->m_pLock);

	if (this->m_filter->IsActive())
		return VFW_E_FILTER_ACTIVE;

	if (enabled)
	{
		CheckPointer(pPlaylistUrl, E_POINTER);
		CheckPointer(pFileUrl, E_POINTER);

		if (segmentTime < 1)
			return E_FAIL;

		size_t strLen = strlen(pPlaylistUrl);
		if (strLen >= AVParams_MaxUrlLength)
			return E_OUTOFMEMORY;

		strLen = strlen(pFileUrl);
		if (strLen >= AVParams_MaxUrlLength)
			return E_OUTOFMEMORY;

		HRESULT hr = SetContainer(Container_MPEG2TSH264);
		if (hr != S_OK)
			return hr;

		strcpy_s(this->m_params.m_hlsPlaylistUrl, pPlaylistUrl);
		strcpy_s(this->m_params.m_hlsFileUrl, pFileUrl);
		this->m_params.m_hlsSegmentTime = segmentTime;
	}
	else
	{
		this->m_params.m_hlsSegmentTime = 0;
	}

	return S_OK;
}

HRESULT EncoderParameters::GetHlsSegmenter(BOOL * pEnabled)
{
	CheckPointer(pEnabled, E_POINTER);

	CAutoLock cObjectLock(this->m_filter->m_pLock);

	*pEnabled = (BOOL)this->m_params.m_hlsSegmentTime;

	return S_OK;
}