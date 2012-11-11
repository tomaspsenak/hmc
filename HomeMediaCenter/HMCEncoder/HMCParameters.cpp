#include "StdAfx.h"
#include "HMCParameters.h"

HMCParameters::HMCParameters(LPUNKNOWN pUnk) : CUnknown(L"HMCParameters", pUnk)
{
	SetContainer(Container_MPEG2PS);
}

HMCParameters::~HMCParameters(void)
{
}

HRESULT HMCParameters::SetContainer(enum Container container)
{
	switch (container)
	{
		case Container_MPEG2PS:
			strcpy_s(this->m_containerStr, "vob");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = true;
			this->m_videoInterlaced = true;
			this->m_videoBFrames = false;
			this->m_videoGopSize = 15;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 48000;
			this->m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		case Container_MPEG2TS:
			strcpy_s(this->m_containerStr, "mpegts");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = true;
			this->m_videoInterlaced = true;
			this->m_videoBFrames = false;
			this->m_videoGopSize = 15;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 48000;
			this->m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		case Container_MP3:
			strcpy_s(this->m_containerStr, "mp3");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = false;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 44100;
			this->m_audioChannels = 2;

			break;
		case Container_MP4:
			strcpy_s(this->m_containerStr, "mp4");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = false;
			this->m_videoInterlaced = false;
			this->m_videoBFrames = false;
			this->m_videoGopSize = 15;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 44100;
			this->m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_AVI:
			strcpy_s(this->m_containerStr, "avi");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = false;
			this->m_videoInterlaced = false;
			this->m_videoBFrames = false;
			this->m_videoGopSize = 15;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 44100;
			this->m_audioChannels = 2;

			SetVideoCBR(800 * 1000);

			break;
		case Container_FLV:
			strcpy_s(this->m_containerStr, "flv");
			this->m_audioCodec = CODEC_ID_NONE;
			this->m_videoCodec = CODEC_ID_NONE;
			this->m_streamable = true;
			this->m_videoInterlaced = false;
			this->m_videoBFrames = false;
			this->m_videoGopSize = 15;

			SetAudioCBR(128 * 1000);
			this->m_audioSamplerate = 44100;
			this->m_audioChannels = 2;

			SetVideoCBR(4000 * 1000);

			break;
		default:
			return E_FAIL;
	}

	this->m_container = container;

	return S_OK;
}

HRESULT HMCParameters::GetContainer(enum Container * pContainer)
{
	CheckPointer(pContainer, E_POINTER);
	*pContainer = this->m_container;

	return S_OK;
}

HRESULT HMCParameters::SetStreamable(BOOL streamable)
{
	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_MP3:
		case Container_FLV:
			this->m_streamable = streamable;
			return S_OK;
		default:
			if (streamable)
				return E_FAIL;

			this->m_streamable = streamable;
			return S_OK;
	}
}

HRESULT HMCParameters::GetStreamable(BOOL * pStreamable)
{
	CheckPointer(pStreamable, E_POINTER);
	*pStreamable = this->m_streamable;

	return S_OK;
}

HRESULT HMCParameters::SetAudioCBR(int audioBitrate)
{
	this->m_audioBitrate = audioBitrate;
	this->m_audioMode = BitrateMode_CBR;

	return S_OK;
}

HRESULT HMCParameters::SetAudioVBR(int audioBitrate, int audioBitrateMax, int audioBitrateMin, int audioQuality)
{
	this->m_audioBitrate = audioBitrate;
	this->m_audioBitrateMax = audioBitrateMax;
	this->m_audioBitrateMin = audioBitrateMin;
	this->m_audioQuality = audioQuality;
	this->m_audioMode = BitrateMode_VBR;

	return S_OK;
}

HRESULT HMCParameters::SetVideoCBR(int videoBitrate)
{
	this->m_videoBitrate = videoBitrate;
	this->m_videoMode = BitrateMode_CBR;

	return S_OK;
}

HRESULT HMCParameters::SetVideoVBR(int videoBitrate, int videoBitrateMax, int videoBitrateMin, int videoQuality)
{
	this->m_videoBitrate = videoBitrate;
	this->m_videoBitrateMax = videoBitrateMax;
	this->m_videoBitrateMin = videoBitrateMin;
	this->m_videoQuality = videoQuality;
	this->m_videoMode = BitrateMode_VBR;

	return S_OK;
}

HRESULT HMCParameters::SetVideoInterlace(BOOL interlaced)
{
	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_AVI:
			this->m_videoInterlaced = interlaced;
			return S_OK;
		default:
			if (interlaced)
				return E_FAIL;

			this->m_videoInterlaced = interlaced;
			return S_OK;
	}
}

HRESULT HMCParameters::GetVideoInterlace(BOOL * interlaced)
{
	CheckPointer(interlaced, E_POINTER);
	*interlaced = this->m_videoInterlaced;

	return S_OK;
}

HRESULT HMCParameters::SetVideoBFrames(BOOL bFrames)
{
	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_AVI:
		case Container_MP4:
			this->m_videoBFrames = bFrames;
			return S_OK;
		default:
			if (bFrames)
				return E_FAIL;

			this->m_videoBFrames = bFrames;
			return S_OK;
	}
}

HRESULT HMCParameters::GetVideoBFrames(BOOL * bFrames)
{
	CheckPointer(bFrames, E_POINTER);
	*bFrames = this->m_videoBFrames;

	return S_OK;
}

HRESULT HMCParameters::SetVideoGopSize(int gopSize)
{
	switch (this->m_container)
	{
		case Container_MPEG2PS:
		case Container_MPEG2TS:
		case Container_AVI:
		case Container_MP4:
		case Container_FLV:
			this->m_videoGopSize = gopSize;
			return S_OK;
		default:
			return E_FAIL;
	}
}

HRESULT HMCParameters::GetVideoGopSize(int * gopSize)
{
	CheckPointer(gopSize, E_POINTER);
	*gopSize = this->m_videoGopSize;

	return S_OK;
}