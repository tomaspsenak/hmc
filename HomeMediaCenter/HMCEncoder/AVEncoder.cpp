#include "StdAfx.h"
#include "AVEncoder.h"
#include <dvdmedia.h>

#define AV_PKT_FLAG_KEY			0x0001
#define FF_COMPLIANCE_EXPERIMENTAL	-2

AVEncoder::AVEncoder(void) : m_formatContext(NULL), m_audioStream(NULL), m_videoStream(NULL), m_audioResample(NULL), m_audioInChannels(0),
	m_audioInSampleRate(0), m_audioInBuf(NULL), m_audioFIFO(NULL), m_audioFrame(NULL), m_pictureFormat(PIX_FMT_UYVY422), 
	m_pictureContext(NULL), m_pictureInFrame(NULL), m_pictureOutFrame(NULL), m_isStopped(FALSE)
{
	av_register_all();
}

AVEncoder::~AVEncoder(void)
{
	FreeContext();
}

HRESULT AVEncoder::Start(StreamOutputPin * streamPin, HMCParameters * params, AM_MEDIA_TYPE * audioMT, AM_MEDIA_TYPE * videoMT)
{
	//Lock musi byt v nasledujucom poradi aby nevznikol deadlock (audio / video lock nasledne ziada main lock)
	CAutoLock cAutoLock1(&this->m_audioLock);
	CAutoLock cAutoLock2(&this->m_videoLock);
	CAutoLock cAutoLock3(&this->m_mainLock);
	HRESULT hr = S_OK;

	//Kontext musi byt null, inak uz je vytvoreny
	if (this->m_formatContext || this->m_audioStream || this->m_videoStream)
	{
		goto done;
	}

	//Inicializacia kontextu podla nazvu kontajnera
	if (avformat_alloc_output_context2(&this->m_formatContext, NULL, params->m_containerStr, NULL) < 0)
	{
		CHECK_HR(hr = E_FAIL);
	}

	//Nastavenie videa - ak je pripojeny video pin
	if (videoMT)
	{
		if (params->m_videoCodec != CODEC_ID_NONE)
			this->m_formatContext->oformat->video_codec = params->m_videoCodec;

		if (!(this->m_videoStream = AddVideo(params, videoMT)))
		{
			CHECK_HR(hr = E_FAIL);
		}
	}

	//Nastavenie audia - ak je pripojeny audio pin
	if (audioMT)
	{
		if (params->m_audioCodec != CODEC_ID_NONE)
			this->m_formatContext->oformat->audio_codec = params->m_audioCodec;

		if (!(this->m_audioStream = AddAudio(params, audioMT)))
		{
			CHECK_HR(hr = E_FAIL);
		}
	}

	unsigned char * buffer = (unsigned char *)av_malloc(HMCFilter::OutBufferSize);
	if (!buffer)
	{
		CHECK_HR(hr = E_OUTOFMEMORY);
	}

	this->m_formatContext->pb = avio_alloc_context(buffer, HMCFilter::OutBufferSize, 1, streamPin, 
		StreamOutputPin::ReadPackets, StreamOutputPin::WritePackets, params->m_streamable ? NULL : StreamOutputPin::Seek);
	if (!this->m_formatContext->pb)
	{
		CHECK_HR(hr = E_FAIL);
	}

	this->m_formatContext->packet_size = 3072;
	this->m_formatContext->max_delay = (int)(0.7 * AV_TIME_BASE);
	this->m_formatContext->pb->seekable = !params->m_streamable;
	this->m_formatContext->flags |= AVFMT_NOFILE;

	//Zapise hlavicku suboru ak ju kontajner obsahuje
	if (avformat_write_header(this->m_formatContext, NULL) < 0)
	{
		CHECK_HR(hr = E_FAIL);
	}

	this->m_isStopped = FALSE;

done:

	if (FAILED(hr))
		FreeContext();

	return hr;
}

HRESULT AVEncoder::Stop(BOOL isEOS)
{
	CAutoLock cAutoLock1(&this->m_audioLock);
	CAutoLock cAutoLock2(&this->m_videoLock);
	CAutoLock cAutoLock3(&this->m_mainLock);

	AVPacket * pkt = new AVPacket();

	if (pkt)
	{
		if (this->m_videoStream)
		{
			//Zapis nespracovane video snimky
			AVCodecContext * c = this->m_videoStream->codec;

			for (;;)
			{
				av_init_packet(pkt);

				int got_packet;
				int nOutSize = avcodec_encode_video2(c, pkt, NULL, &got_packet);
				if (nOutSize < 0 || !got_packet)
					break;

				pkt->stream_index = this->m_videoStream->index;

				if (pkt->pts != AV_NOPTS_VALUE)
					pkt->pts = av_rescale_q(pkt->pts, c->time_base, this->m_videoStream->time_base);
				if (pkt->dts != AV_NOPTS_VALUE)
					pkt->dts = av_rescale_q(pkt->dts, c->time_base, this->m_videoStream->time_base);

				av_interleaved_write_frame(this->m_formatContext, pkt);

				av_free_packet(pkt);
			}
		}

		if (this->m_audioStream)
		{
			//Zapis nespracovane audio snimky
			AVCodecContext* c = this->m_audioStream->codec;

			//Ak bol dosiahnuty koniec streamu, zapisu sa buffrovane audio packety
			if (isEOS)
				EncodeAudio(NULL, 0);

			for (;;)
			{
				av_init_packet(pkt);

				int got_packet;
				int nOutSize = avcodec_encode_audio2(c, pkt, NULL, &got_packet);
				if (nOutSize < 0 || !got_packet)
					break;

				pkt->stream_index= this->m_audioStream->index;

				av_interleaved_write_frame(this->m_formatContext, pkt);

				av_free_packet(pkt);
			}
		}

		delete pkt;
	}

	if (this->m_formatContext)
	{
		//Ak bolo prerusene enkodovanie, nie je potrebne zapisovat nespracovane pakety
		if (!isEOS)
			FreeInterleavePackets(this->m_formatContext);

		//Zapise zahlavie suboru ak ho kontajner obsahuje
		av_write_trailer(this->m_formatContext);
	}

	FreeContext();

	this->m_isStopped = TRUE;

	return S_OK;
}

AVStream * AVEncoder::AddAudio(HMCParameters * params, AM_MEDIA_TYPE * audioMT)
{
    AVCodecContext * c;
	AVCodec * codec;
    AVStream * st;

	WAVEFORMATEX * inputFormat;

	if (audioMT->formattype == FORMAT_WaveFormatEx)
		inputFormat = (WAVEFORMATEX*)audioMT->pbFormat;
	else
		return NULL;

	codec = avcodec_find_encoder(this->m_formatContext->oformat->audio_codec);
	if (codec == NULL)
		return NULL;

    st = avformat_new_stream(this->m_formatContext, codec);
    if (st == NULL)
        return NULL;

    c = st->codec;
	c->codec_id = this->m_formatContext->oformat->audio_codec;
    c->codec_type = AVMEDIA_TYPE_AUDIO;
	c->sample_fmt = c->codec_id == AV_CODEC_ID_MP3 ? AV_SAMPLE_FMT_S16P : AV_SAMPLE_FMT_S16;

	c->sample_rate = params->m_audioSamplerate;

	c->channels = params->m_audioChannels;
	c->channel_layout = av_get_default_channel_layout(params->m_audioChannels);

	c->bit_rate = params->m_audioBitrate;
	if (params->m_audioMode == BitrateMode_VBR)
	{
		c->global_quality = params->m_audioQuality;
		c->rc_max_rate = params->m_audioBitrateMax;
		c->rc_min_rate = params->m_audioBitrateMin;
		c->flags |= CODEC_FLAG_QSCALE;
	}
	
	if (this->m_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (avcodec_open2(c, codec, NULL) < 0)
        return NULL;

	this->m_audioFIFO = av_audio_fifo_alloc(c->sample_fmt, c->channels, c->frame_size);
	this->m_audioFrame = avcodec_alloc_frame();

	if (av_samples_alloc_array_and_samples(&this->m_audioInBuf, NULL, c->channels, c->frame_size, c->sample_fmt, 0) < 0 ||
		!this->m_audioFIFO || !this->m_audioFrame)
	{
		avcodec_close(st->codec);
		return NULL;
	}

	this->m_audioInChannels = inputFormat->nChannels;
	this->m_audioInSampleRate = inputFormat->nSamplesPerSec;

	if (params->m_audioChannels != inputFormat->nChannels || params->m_audioSamplerate != inputFormat->nSamplesPerSec ||
		c->sample_fmt != AV_SAMPLE_FMT_S16) 
	{
		this->m_audioResample = swr_alloc();
		if(!this->m_audioResample)
		{
			avcodec_close(st->codec);
			return NULL;
		}

		av_opt_set_int(this->m_audioResample, "in_channel_layout", av_get_default_channel_layout(inputFormat->nChannels), 0);
		av_opt_set_int(this->m_audioResample, "out_channel_layout", c->channel_layout, 0);
		av_opt_set_int(this->m_audioResample, "in_sample_rate", inputFormat->nSamplesPerSec, 0);
		av_opt_set_int(this->m_audioResample, "out_sample_rate", c->sample_rate, 0);
		av_opt_set_sample_fmt(this->m_audioResample, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
		av_opt_set_sample_fmt(this->m_audioResample, "out_sample_fmt", c->sample_fmt, 0);

		if(swr_init(this->m_audioResample) < 0)
		{
			avcodec_close(st->codec);
			return NULL;
		}
	}

    return st;
}

AVStream * AVEncoder::AddVideo(HMCParameters * params, AM_MEDIA_TYPE * videoMT)
{
	AVCodecContext * c;
	AVCodec * codec;
    AVStream * st;

	BITMAPINFOHEADER * inputFormat;
	REFERENCE_TIME timePerFrame;

	if (videoMT->subtype == MEDIASUBTYPE_UYVY)
		this->m_pictureFormat = PIX_FMT_UYVY422;
	else if (videoMT->subtype == MEDIASUBTYPE_YUY2)
		this->m_pictureFormat = PIX_FMT_YUYV422;
	else if (videoMT->subtype == MEDIASUBTYPE_RGB24)
		this->m_pictureFormat = PIX_FMT_BGR24;
	else if (videoMT->subtype == MEDIASUBTYPE_YV12)
		this->m_pictureFormat = PIX_FMT_YUV420P;
	else
		return NULL;

	if (videoMT->formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *)videoMT->pbFormat;
		timePerFrame = vih->AvgTimePerFrame;
		inputFormat = &vih->bmiHeader;
	}
	else if (videoMT->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *)videoMT->pbFormat;
		timePerFrame = vih->AvgTimePerFrame;
		inputFormat = &vih->bmiHeader;
	}
	else
	{
		return NULL;
	}

	codec = avcodec_find_encoder(this->m_formatContext->oformat->video_codec);
	if (codec == NULL)
		return NULL;

	st = avformat_new_stream(this->m_formatContext, codec);
    if (st == NULL)
        return NULL;

    c = st->codec;
	c->codec_id = this->m_formatContext->oformat->video_codec;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
	c->pix_fmt = PIX_FMT_YUV420P;

	c->sample_aspect_ratio.den = 1;
	c->sample_aspect_ratio.num = 1;
	st->sample_aspect_ratio = c->sample_aspect_ratio;

	AVRational frameRate =  av_d2q((double)UNITS / timePerFrame, 30000);
	c->time_base.den =  frameRate.num;
	c->time_base.num =  frameRate.den;

	c->width = inputFormat->biWidth;
	c->height = inputFormat->biHeight;

	c->bit_rate = params->m_videoBitrate;
	if (params->m_videoMode == BitrateMode_VBR)
	{
		c->global_quality = params->m_videoQuality;
		c->rc_max_rate = params->m_videoBitrateMax;
		c->rc_min_rate = params->m_videoBitrateMin;
		c->flags |= CODEC_FLAG_QSCALE;
		c->rc_buffer_size = params->m_videoBufferSize;
	}
	else
	{
		if (c->codec_id == CODEC_ID_MPEG2VIDEO)
		{
			c->rc_buffer_size = params->m_videoBufferSize;
			c->rc_max_rate = c->rc_min_rate = params->m_videoBitrate;
		}
		else
		{
			c->rc_max_rate = c->rc_buffer_size = params->m_videoBitrate * 2;
		}
	}

	c->rc_initial_buffer_occupancy = c->rc_buffer_size * 3 / 4; 

	if (params->m_videoInterlaced)
	{
		c->flags |= CODEC_FLAG_INTERLACED_ME; //CODEC_FLAG_INTERLACED_DCT
	}

	if (c->codec_id == CODEC_ID_MPEG2VIDEO)
	{
		if (c->time_base.num != 1 || (c->time_base.den != 25 || c->time_base.den != 30  || c->time_base.den != 50 || c->time_base.den != 60))
		{
			//Nedodrzat standard
			c->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
		}
	}

	c->gop_size = params->m_videoGopSize;
	if (params->m_videoBFrames)
	{
        c->max_b_frames = 2;
		c->b_quant_factor = 2.0;
	}

	if (this->m_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (avcodec_open2(c, codec, NULL) < 0)
		return NULL;

	this->m_pictureOutFrame = CreateFrame(c->pix_fmt, c->width, c->height);
	if (!this->m_pictureOutFrame)
	{
		avcodec_close(st->codec);
        return NULL;
	}
	this->m_pictureOutFrame->quality = c->global_quality;

	if (this->m_pictureFormat != PIX_FMT_YUV420P || c->width != inputFormat->biWidth || c->height != inputFormat->biHeight)
	{
		//Ak vstupny format sa nezhoduje s vystupnym - je potrebna konverzia cez sws

		this->m_pictureInFrame = CreateFrame(this->m_pictureFormat, inputFormat->biWidth, inputFormat->biHeight);
		this->m_pictureContext = sws_getContext(inputFormat->biWidth, inputFormat->biHeight, this->m_pictureFormat, inputFormat->biWidth, 
			inputFormat->biHeight, c->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
		if (!this->m_pictureInFrame || !this->m_pictureContext)
		{
			avcodec_close(st->codec);
			return NULL;
		}
	}

	return st;
}

HRESULT AVEncoder::FreeContext(void)
{
	if (this->m_formatContext)
	{
		if (this->m_formatContext->pb)
		{
			if (this->m_formatContext->pb->buffer)
			{
				av_free(this->m_formatContext->pb->buffer);
				this->m_formatContext->pb->buffer = NULL;
			}	

			av_free(this->m_formatContext->pb);
			this->m_formatContext->pb = NULL;
		}

		if (this->m_audioStream)
		{
			avcodec_close(this->m_audioStream->codec);
			this->m_audioStream = NULL;
		}

		if (this->m_videoStream)
		{
			avcodec_close(this->m_videoStream->codec);
			this->m_videoStream = NULL;
		}

		avformat_free_context(this->m_formatContext);
		this->m_formatContext = NULL;
	}

	if (this->m_audioInBuf)
	{
		if (this->m_audioInBuf[0])
			av_freep(&this->m_audioInBuf[0]);
		av_freep(&this->m_audioInBuf);
	}

	if (this->m_audioFIFO)
	{
		av_audio_fifo_free(this->m_audioFIFO);
		this->m_audioFIFO = NULL;
	}

	if (this->m_audioFrame)
	{
		av_free(this->m_audioFrame);
		this->m_audioFrame = NULL;
	}
	
	if (this->m_audioResample)
	{
		swr_free(&this->m_audioResample);
		this->m_audioResample = NULL;
	}

	if (this->m_pictureInFrame)
	{
		av_free(this->m_pictureInFrame->data[0]);
		av_free(this->m_pictureInFrame);
		this->m_pictureInFrame = NULL;
	}

	if (this->m_pictureOutFrame)
	{
		av_free(this->m_pictureOutFrame->data[0]);
		av_free(this->m_pictureOutFrame);
		this->m_pictureOutFrame = NULL;
	}

	if (this->m_pictureContext)
	{
		sws_freeContext(this->m_pictureContext);
		this->m_pictureContext = NULL;
	}

	return S_OK;
}

AVFrame * AVEncoder::CreateFrame(PixelFormat pix_fmt, int width, int height)
{
	AVFrame * frame;
    uint8_t * buffer;
    size_t size;
    
    frame = avcodec_alloc_frame();
    if (!frame)
        return NULL;

    size = avpicture_get_size(pix_fmt, width, height);
    buffer = (uint8_t *)av_malloc(size);
    if (!buffer)
	{
        av_free(frame);
        return NULL;
    }

	frame->width = width;
	frame->height = height;
    avpicture_fill((AVPicture *)frame, buffer, pix_fmt, width, height);

    return frame;
}

//Nahrada za (libavformat/mux.c):
//int ff_interleave_packet_per_dts(AVFormatContext *s, AVPacket *out, AVPacket *pkt, int flush)
//funkcia urobi flush nad packetmi
int AVEncoder::ff_interleave_packet_per_dts(AVFormatContext * s, AVPacket * out)
{
    AVPacketList * pktl;
    int stream_count = 0, noninterleaved_count = 0;
    unsigned int i;

    for (i = 0; i < s->nb_streams; i++)
	{
        if (s->streams[i]->last_in_packet_buffer)
			++stream_count;
		else if (s->streams[i]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE)
			++noninterleaved_count;
    }

    if (stream_count)
	{
        AVStream * st;
        pktl = s->packet_buffer;
        *out = pktl->pkt;
        st   = s->streams[out->stream_index];

        s->packet_buffer = pktl->next;
        if (!s->packet_buffer)
            s->packet_buffer_end = NULL;

        if (st->last_in_packet_buffer == pktl)
            st->last_in_packet_buffer = NULL;
        av_freep(&pktl);

        return 1;
    }
	else
	{
        av_init_packet(out);
    }

	return 0;
}

int AVEncoder::FreeInterleavePackets(AVFormatContext * s)
{
	int ret;

	for(;;)
	{
		AVPacket pkt;

		if (s->oformat->interleave_packet)
			ret = s->oformat->interleave_packet(s, &pkt, NULL, 1);
		else
			ret = ff_interleave_packet_per_dts(s, &pkt);

		if(ret < 0)
             return ret;
         if(!ret)
             break;

         av_free_packet(&pkt);
     }

     return ret;
}

HRESULT AVEncoder::CheckStream(AVStream* & stream, CCritSec & streamLock)
{
	//streamLock musi byt uzamknuty
	HRESULT hr = S_OK;
	if (!stream)
	{
		//Filter este nemusi byt odstartovany - pockaj na odstartovanie
		hr = E_POINTER;
		for (int i = 0; i < 20; i++)
		{
			streamLock.Unlock();
			Sleep(50);
			streamLock.Lock();
			if (stream)
			{
				hr = S_OK;
				break;
			}
		}
	}
	return hr;
}

HRESULT AVEncoder::EncodeAudio(BYTE * buffer, long length)
{
	AVCodecContext * c = NULL;
	AVPacket * pkt = NULL;
	HRESULT hr = S_OK;

	CAutoLock cAutoLock(&this->m_audioLock);

	if (this->m_isStopped)
		return E_FAIL;
	CHECK_HR(hr = CheckStream(this->m_audioStream, this->m_audioLock));
	//m_audioStream uz moze byt NULL, preto zistovat codec az po kontrole streamu
	c = this->m_audioStream->codec;

	if (!(pkt = new AVPacket()))
		CHECK_HR(hr = E_OUTOFMEMORY);
	av_init_packet(pkt);

	//Pocet samplov vo vstupnom buffery
	int inSamples = length / (this->m_audioInChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));

	if (inSamples == 0)
	{
		//Nezapisuje sa nic do FIFO
	}
	else if (this->m_audioResample)
	{
		//Uprava audia - ak nesedi pocet kanalov, samplerate alebo vystupny format
		uint8_t ** output = NULL;
		int outSamples = av_rescale_rnd(swr_get_delay(this->m_audioResample, this->m_audioInSampleRate) + inSamples, 
			c->sample_rate, this->m_audioInSampleRate, AV_ROUND_UP);

		if (av_samples_alloc_array_and_samples(&output, NULL, c->channels, outSamples, c->sample_fmt, 0) < 0)
			CHECK_HR(hr = E_FAIL);

		outSamples = swr_convert(this->m_audioResample, output, outSamples, (const uint8_t **)&buffer, inSamples);
		if (outSamples < 0)
		{
			if (output[0])
				av_freep(&output[0]);
			av_freep(&output);

			CHECK_HR(hr = E_FAIL);
		}

		av_audio_fifo_write(this->m_audioFIFO, (void **)output, outSamples);
		
		if (output[0])
			av_freep(&output[0]);
		av_freep(&output);
	}
	else
	{
		//Bez upravi audia - zapis buffer tak ako je
		av_audio_fifo_write(this->m_audioFIFO, (void **)&buffer, inSamples);
	}

	//Nacitava FIFO pokial nie je pozadovana velkost paketu (frame_size) alebo koniec streamu (inSamples == 0)
	while (av_audio_fifo_size(this->m_audioFIFO) >= c->frame_size || inSamples == 0)
	{
		int outSamples = av_audio_fifo_read(this->m_audioFIFO, (void **)this->m_audioInBuf, c->frame_size);
		if (outSamples <= 0)
			break;

		int audioInBufSize = av_samples_get_buffer_size(NULL, c->channels, outSamples, c->sample_fmt, 0);

		this->m_audioFrame->nb_samples  = outSamples;
		//nie je potrebne nastavit pts - nastavi sa automaticky
		//this->m_audioFrame->pts = av_rescale_q(c->frame_number * this->m_audioFrame->nb_samples, c->time_base, this->m_audioStream->time_base);

		avcodec_fill_audio_frame(this->m_audioFrame, c->channels, c->sample_fmt, this->m_audioInBuf[0], audioInBufSize, 0);

		av_init_packet(pkt);

		int got_packet;
		int nOutSize = avcodec_encode_audio2(c, pkt, this->m_audioFrame, &got_packet);
		if (nOutSize < 0)
			CHECK_HR(hr = E_FAIL);

		if (!got_packet)
			continue;

		pkt->stream_index= this->m_audioStream->index;

		CAutoLock cAutoLock2(&this->m_mainLock);
		if (av_interleaved_write_frame(this->m_formatContext, pkt) != 0)
		{
			CHECK_HR(hr = E_FAIL);
		}

		av_free_packet(pkt);
    }

done:

	av_free_packet(pkt);
	if (pkt)
		delete pkt;

	return hr;
}

HRESULT AVEncoder::EncodeVideo(BYTE * buffer, long length)
{
	AVCodecContext * c = NULL;
	AVPacket * pkt = NULL;
	HRESULT hr = S_OK;

	CAutoLock cAutoLock(&this->m_videoLock);

	if (this->m_isStopped)
		return E_FAIL;
	CHECK_HR(hr = CheckStream(this->m_videoStream, this->m_videoLock));
	//m_videoStream uz moze byt NULL, preto zistovat codec az po kontrole streamu
	c = this->m_videoStream->codec;

	if (!(pkt = new AVPacket()))
		CHECK_HR(hr = E_OUTOFMEMORY);
	av_init_packet(pkt);

	switch (this->m_pictureFormat)
	{
		case PIX_FMT_BGR24: //MEDIASUBTYPE_RGB24
		{
			//RGB obraz z DirectShow je prevrateny - preloz riadky

			int linesize = this->m_pictureInFrame->linesize[0];
			for (long i = 0; i < this->m_pictureInFrame->height; i++)
			{
				long srcPos = i * linesize;
				long dstPos = (length - srcPos) - linesize;
				memcpy(this->m_pictureInFrame->data[0] + dstPos, buffer + srcPos, linesize);
			}

			sws_scale(this->m_pictureContext, this->m_pictureInFrame->data, this->m_pictureInFrame->linesize, 0, 
				this->m_pictureInFrame->height, this->m_pictureOutFrame->data, this->m_pictureOutFrame->linesize);

			break;
		}
		case PIX_FMT_YUYV422: //MEDIASUBTYPE_YUY2
		case PIX_FMT_UYVY422: //MEDIASUBTYPE_UYVY
		{
			uint8_t * tmpData = this->m_pictureInFrame->data[0];
			this->m_pictureInFrame->data[0] = buffer;

			sws_scale(this->m_pictureContext, this->m_pictureInFrame->data, this->m_pictureInFrame->linesize, 0, 
				this->m_pictureInFrame->height, this->m_pictureOutFrame->data, this->m_pictureOutFrame->linesize);

			this->m_pictureInFrame->data[0] = tmpData;

			break;
		}
		case PIX_FMT_YUV420P: //MEDIASUBTYPE_YV12
		{
			//Ak nie je potrebne menit format alebo velkost cez sws_scale, zapise sa rovno do vystupneho frame-u 
			AVFrame * frame = (this->m_pictureContext) ? this->m_pictureInFrame : this->m_pictureOutFrame;

			//Preloz planes z directshow do AVFrame (PIX_FMT_YUV420P ma 3 planes)
			memcpy(frame->data[0], buffer, frame->width * frame->height);
			memcpy(frame->data[2], buffer + (frame->width * frame->height), (frame->width * frame->height) >> 2);
			memcpy(frame->data[1], buffer + (frame->width * frame->height) + ((frame->width * frame->height) >> 2), 
				(frame->width * frame->height) >> 2);

			if (this->m_pictureContext)
			{
				sws_scale(this->m_pictureContext, this->m_pictureInFrame->data, this->m_pictureInFrame->linesize, 0, c->height, 
					this->m_pictureOutFrame->data, this->m_pictureOutFrame->linesize);
			}

			break;
		}
		default:
			CHECK_HR(hr = E_FAIL);
	}

	this->m_pictureOutFrame->pts = c->frame_number;

	int got_packet;
	int nOutSize = avcodec_encode_video2(c, pkt, this->m_pictureOutFrame, &got_packet);
	if (nOutSize < 0)
		CHECK_HR(hr = E_FAIL);

	if (got_packet) 
	{
		pkt->stream_index = this->m_videoStream->index;

		if (pkt->pts != AV_NOPTS_VALUE)
			pkt->pts = av_rescale_q(pkt->pts, c->time_base, this->m_videoStream->time_base);
		if (pkt->dts != AV_NOPTS_VALUE)
			pkt->dts = av_rescale_q(pkt->dts, c->time_base, this->m_videoStream->time_base);
		
		CAutoLock cAutoLock2(&this->m_mainLock);
		if (av_interleaved_write_frame(this->m_formatContext, pkt) != 0)
		{	
			CHECK_HR(hr = E_FAIL);
		}
	}

done:

	av_free_packet(pkt);
	if (pkt)
		delete pkt;

	return hr;
}