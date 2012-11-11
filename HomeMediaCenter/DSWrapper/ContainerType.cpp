#include "StdAfx.h"
#include "ContainerType.h"
#include "DSWrapper.h"
#include "Extern/IffdshowBase.h"
#include "Extern/vp8encoderidl.h"
#include <Ks.h>
#include <codecapi.h>
#include "Extern/IffDecoder.h"
#include "Extern/IVorbisEncodeSettings.h"
#include "HMCEncoder_i.c"
#include "FileWriterFilter.h"
#include <wmsdk.h>
#include <dshowasf.h>
#include "WMSinkWriter.h"
#include "DSException.h"
#include "DSBufferedStream.h"
#include "FramerateFilter.h"
#include <InitGuid.h>
#include "DLLManager.h"

DEFINE_GUID(CLSID_Mpeg2Encoder, 0x5F5AFF4A, 0x2F7F, 0x4279, 0x88, 0xC2, 0xCD, 0x88, 0xEB, 0x39, 0xD1, 0x44);
DEFINE_GUID(CLSID_WebmMux, 0xED3110F0, 0x5211, 0x11DF, 0x94, 0xAF, 0x00, 0x26, 0xB9, 0x77, 0xEE, 0xAA);
DEFINE_GUID(CLSID_WebmVideo, 0xED3110F5, 0x5211, 0x11DF, 0x94, 0xAF, 0x00, 0x26, 0xB9, 0x77, 0xEE, 0xAA);
DEFINE_GUID(CLSID_VorbisEncodeFilter, 0x5C94FE86, 0xB93B, 0x467F, 0xBF, 0xC3, 0xBD, 0x6C, 0x91, 0x41, 0x6F, 0x9B);  
DEFINE_GUID(CLSID_IVorbisEncodeSettings, 0xA4C6A887, 0x7BD3, 0x4B33, 0x9A, 0x57, 0xA3, 0xEB, 0x10, 0x92, 0x4D, 0x3A);  
DEFINE_GUID(CLSID_WebmOut, 0xED3110EB, 0x5211, 0x11DF, 0x94, 0xAF, 0x00, 0x26, 0xB9, 0x77, 0xEE, 0xAA);
DEFINE_GUID(CLSID_FFDSHOWRaw, 0x0B390488, 0xD80F, 0x4A68, 0x84, 0x08, 0x48, 0xDC, 0x19, 0x9F, 0x0E, 0x97);

static DLLManager g_dllmanager;

namespace DSWrapper 
{
	ContainerType ^ ContainerType::MPEG2_PS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
		UINT32 percentQuality, UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, 
		bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate)
	{
		if (IsMPEG2Installed())
		{
			return gcnew ContainerMPEG2_PS(width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, intSubtitles, 
				intSubtitlesPath, keepAspectRatio, mpaLayer, audBitrate);
		}
		else
		{
			return gcnew ContainerHMC(Container_MPEG2PS, true, width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, 
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
		}
	}
	
	ContainerType ^ ContainerType::MPEG2_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
		UINT32 percentQuality, UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath,
		bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate)
	{
		if (IsMPEG2Installed())
		{
			return gcnew ContainerMPEG2_TS(width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, intSubtitles, 
				intSubtitlesPath, keepAspectRatio, mpaLayer, audBitrate);
		}
		else
		{
			return gcnew ContainerHMC(Container_MPEG2TS, true, width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, 
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
		}
	}
	
	ContainerType ^ ContainerType::WEBM(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerWEBM(width, height, bitrateMode, vidBitrate, percentQuality, fps, intSubtitles, intSubtitlesPath, 
			keepAspectRatio, audBitrate, kWebmMuxModeDefault);
	}

	ContainerType ^ ContainerType::WEBM_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerWEBM(width, height, bitrateMode, vidBitrate, percentQuality, fps, intSubtitles, intSubtitlesPath,
			keepAspectRatio, audBitrate, kWebmMuxModeLive);
	}

	ContainerType ^ ContainerType::WMV(UINT32 width, UINT32 height, WMVideoSubtype videoSubtype, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, UINT32 audBitrate)
	{
		if (width == 0 || height == 0)
			throw gcnew DSException(L"WMV - video resolution must be specified", 0);
		if (vidBitrate == 0)
			throw gcnew DSException(L"WMV - video bitrate must be specified", 0);
		if (fps == 0)
			throw gcnew DSException(L"WMV - FPS must be specified", 0);
		if (audBitrate == 0)
			throw gcnew DSException(L"WMV - audio bitrate must be specified", 0);

		return gcnew ContainerWMV(width, height, videoSubtype, vidBitrate, percentQuality, fps, intSubtitles, intSubtitlesPath, audBitrate);
	}

	ContainerType ^ ContainerType::AVI(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerHMC(Container_AVI, false, width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
	}

	ContainerType ^ ContainerType::MP4(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerHMC(Container_MP4, false, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
	}

	ContainerType ^ ContainerType::MP3(BitrateMode bitrateMode, UINT32 audBitrate, UINT32 percentQuality)
	{
		return gcnew ContainerHMC(Container_MP3, false, 0, 0, bitrateMode, 0, percentQuality, 0, ScanType::Interlaced, false, nullptr, 
			false, audBitrate);
	}

	ContainerType ^ ContainerType::MP3_TS(BitrateMode bitrateMode, UINT32 audBitrate, UINT32 percentQuality)
	{
		return gcnew ContainerHMC(Container_MP3, true, 0, 0, bitrateMode, 0, percentQuality, 0, ScanType::Interlaced, false, nullptr, 
			false, audBitrate);
	}

	ContainerType ^ ContainerType::FLV(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerHMC(Container_FLV, false, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
	}

	ContainerType ^ ContainerType::FLV_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate)
	{
		return gcnew ContainerHMC(Container_FLV, true, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate);
	}

	System::Boolean ContainerType::IsMPEG2Installed(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_Mpeg2Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	System::Boolean ContainerType::IsWEBMInstalled(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_WebmVideo, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);
		if (hr != S_OK)
			return false;

		hr = CoCreateInstance(CLSID_VorbisEncodeFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);
		if (hr != S_OK)
			return false;

		hr = CoCreateInstance(CLSID_WebmMux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	System::Boolean ContainerType::IsWMVInstalled(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_WMAsfWriter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	System::Boolean ContainerType::IsFFDSHOWInstalled(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FFDSHOWRaw, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	HRESULT ContainerType::ConfigureDecoder(IGraphBuilder * graphBuilder, IPin ** pVideoPin, IPin ** pAudioPin, IPin ** pSubtitlePin)
	{
		HRESULT hr = S_OK;

		IPin * tempPin = NULL;
		IBaseFilter * ffVideoDecoder = NULL;
		IBaseFilter * framerateFilter = NULL;
		IffdshowBaseW * ffVideoConfig = NULL;

		if ((this->m_intSubtitles || this->m_width > 0 || this->m_height > 0) && (*pVideoPin) != NULL)
		{
			//FFDSHOWRaw funguje aj pre wmv format
			CHECK_HR(hr = CoCreateInstance(CLSID_FFDSHOWRaw, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&ffVideoDecoder));
			//CHECK_HR(hr = CoCreateInstance(CLSID_FFDSHOW, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&ffVideoDecoder));
			CHECK_HR(hr = ffVideoDecoder->QueryInterface(IID_IffdshowBaseW, (void **)&ffVideoConfig));

			CHECK_SUCCEED(hr = graphBuilder->AddFilter(ffVideoDecoder, NULL));

			if (this->m_height > 0)
			{
				//Problem s ffdshow - vysku bere s registrov, musi byt zapisane pred pripojenim
				#ifdef _M_X64
					Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow64_raw\\default", "resizeDy", gcnew int(this->m_height));
				#else
					Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow_raw\\default", "resizeDy", gcnew int(this->m_height));
				#endif
			}

			tempPin = DSEncoder::GetFirstPin(ffVideoDecoder, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(*pVideoPin, tempPin));
			SAFE_RELEASE(tempPin);

			if ((*pSubtitlePin) != NULL)
			{
				//Napojenie pinu s titulkami ak je aktivovany v demultiplexore
				tempPin = DSEncoder::GetPin(ffVideoDecoder, PINDIR_INPUT, 1);
				CHECK_SUCCEED(hr = graphBuilder->Connect(*pSubtitlePin, tempPin));
				SAFE_RELEASE(tempPin);
			}

			//Nezobrazuj tray ikonu
			CHECK_HR(hr = ffVideoConfig->putParam(IDFF_trayIcon, 0));

			if (this->m_intSubtitles)
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_isSubtitles, 1));
				if (this->m_intSubtitlesPath == nullptr)
				{
					//Filter automaticky zisti subor s titulkami
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_subAutoFlnm, 1));
				}
				else
				{
					//Zada sa subor s titulkami
					pin_ptr<const wchar_t> pFilePath = PtrToStringChars(this->m_intSubtitlesPath);
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_subAutoFlnm, 0));
					CHECK_HR(hr = ffVideoConfig->putParamStr(821, pFilePath));
				}
			}

			if (this->m_keepAspectRatio)
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeAspect, 1));
			}
			else
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeAspect, 0));
			}

			if (this->m_width > 0 && this->m_height > 0)
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_isResize, 1));
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeIsDy0, 0));
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDx, this->m_width));
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDy, this->m_height));
			}
			else if (this->m_width > 0)
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_isResize, 1));
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeIsDy0, 1));
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDx, this->m_width));
			}
			else
			{
				CHECK_HR(hr = ffVideoConfig->putParam(IDFF_isResize, 0));
			}

			SAFE_RELEASE(*pVideoPin);
			*pVideoPin = DSEncoder::GetFirstPin(ffVideoDecoder, PINDIR_OUTPUT);
		}

		if (this->m_fps > 0 && (*pVideoPin) != NULL)
		{
			//Nastavenie snimkov za sekundu ak je hodnota vacsia ako 0, inak povodna hodnota
			framerateFilter = new FramerateFilter(NULL, &hr, this->m_fps);

			if (framerateFilter == NULL)
				hr = E_OUTOFMEMORY;
			else
				framerateFilter->AddRef();
			CHECK_HR(hr);

			CHECK_SUCCEED(hr = graphBuilder->AddFilter(framerateFilter, NULL));

			tempPin = DSEncoder::GetFirstPin(framerateFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(*pVideoPin, tempPin));
			SAFE_RELEASE(tempPin);

			SAFE_RELEASE(*pVideoPin);
			*pVideoPin = DSEncoder::GetFirstPin(framerateFilter, PINDIR_OUTPUT);
		}

	done:

		SAFE_RELEASE(tempPin);
		SAFE_RELEASE(ffVideoDecoder);
		SAFE_RELEASE(framerateFilter);
		SAFE_RELEASE(ffVideoConfig);

		return hr;
	}

	HRESULT ContainerType::GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter)
	{
		HRESULT hr = S_OK;

		FileWriterFilter * writer = NULL;

		//Vytvorenie writer-a s nastavenim pozadovaneho subtype pre writer
		//Napr.: rozne pre MPEG2_TS a MPEG2_PS
		writer = new FileWriterFilter(NULL, &hr, outputStream, GetSubtype());

		if (writer == NULL)
			hr = E_OUTOFMEMORY;
		else
			writer->AddRef();
		CHECK_HR(hr);

		CHECK_SUCCEED(hr = graphBuilder->AddFilter(writer, NULL));

		*writerFilter = writer;
		writer = NULL;

	done: 

		SAFE_RELEASE(writer);

		return hr;
	}

	HRESULT ContainerMPEG2_PS::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin)
	{
		HRESULT hr = S_OK;
		ULONG vValCount = 0;

		VARIANT variant;
		VARIANT * vVal = NULL;
		IPin * tempPin = NULL;
		ICodecAPI * codecApi = NULL;
		IBaseFilter * codecFilter = NULL;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		VariantInit(&variant);
		variant.vt = VT_UI4;

		CHECK_HR(hr = CoCreateInstance(CLSID_Mpeg2Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&codecFilter));
		CHECK_HR(hr = codecFilter->QueryInterface(IID_ICodecAPI, (void **)&codecApi));

		CHECK_SUCCEED(hr = graphBuilder->AddFilter(codecFilter, NULL));

		if (this->m_bitrateMode == BitrateMode::CBR)
		{
			//Konstantny bitrate
			variant.ulVal = eAVEncCommonRateControlMode_CBR;
			CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonRateControlMode, &variant));

			variant.ulVal = this->m_percentQuality;
			CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonQualityVsSpeed, &variant));

			if (this->m_vidBitrate > 0)
			{
				variant.ulVal = this->m_vidBitrate;
				CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonMeanBitRate, &variant));
			}
		}
		else
		{
			//Variabilny bitrate
			variant.ulVal = eAVEncCommonRateControlMode_Quality;
			CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonRateControlMode, &variant));

			variant.ulVal = this->m_percentQuality;
			CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonQuality, &variant));

			if (this->m_vidBitrate > 0)
			{
				variant.ulVal = this->m_vidBitrate;
				CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncCommonMaxBitRate, &variant));
			}
		}

		//Nastavenie scan type
		variant.ulVal = this->m_scanType == ScanType::Interlaced ? eAVEncVideoSourceScan_Interlaced : eAVEncVideoSourceScan_Progressive;
		CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncVideoForceSourceScanType, &variant));

		//Nastavenie vrstvy pre audio
		variant.ulVal = this->m_mpaLayer == MpaLayer::Layer1 ? eAVEncMPALayer_1 : eAVEncMPALayer_2;
		CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncMPALayer, &variant));

		if (this->m_audBitrate > 0)
		{
			//Nastavenie bitrate pre audio - najde najlepsiu hodnotu z povolenych
			DWORD bestElement = 0;
			UINT32 bestRatio = System::UInt32::MaxValue;

			CHECK_HR(hr = codecApi->GetParameterValues(&CODECAPI_AVEncAudioMeanBitRate, &vVal, &vValCount));
			for (ULONG i = 0; i < vValCount; i++)
			{
				UINT32 actBitrate = abs((INT32)vVal[i].ulVal - (INT32)this->m_audBitrate);
				if (actBitrate < bestRatio)
				{
					bestRatio = actBitrate;
					bestElement = i;
				}
			}

			variant.ulVal = vVal[bestElement].ulVal;
			CHECK_HR(hr = codecApi->SetValue(&CODECAPI_AVEncAudioMeanBitRate, &variant));
		}

		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));

		if (videoPin != NULL)
		{
			tempPin = DSEncoder::GetFirstPin(codecFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, tempPin));
			SAFE_RELEASE(tempPin);
		}

		if (audioPin != NULL)
		{
			tempPin = DSEncoder::GetPin(codecFilter, PINDIR_INPUT, 1);
			CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, tempPin));
			SAFE_RELEASE(tempPin);
		}

		tempPin = DSEncoder::GetFirstPin(codecFilter, PINDIR_OUTPUT);
		CHECK_SUCCEED(hr = graphBuilder->Connect(tempPin, writerPin));
		SAFE_RELEASE(tempPin);

	done:

		if (vVal != NULL)
		{
			for (ULONG i = 0; i < vValCount; i++)
				VariantClear(&vVal[i]);
			CoTaskMemFree(vVal);
		}
		VariantClear(&variant);
		SAFE_RELEASE(tempPin);
		SAFE_RELEASE(codecApi);
		SAFE_RELEASE(codecFilter);

		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);
		SAFE_RELEASE(subtitlePin);
		
		return hr;
	}

	HRESULT ContainerWEBM::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin)
	{
		HRESULT hr = S_OK;

		IPin * inputPin = NULL;
		IPin * outputPin = NULL;
		IUnknown * unknown = NULL;
		IVP8Encoder * webmEnc = NULL;
		IWebmMux * webmMux = NULL;
		IBaseFilter * webmMuxFilter = NULL;
		IBaseFilter * webmVideoFilter = NULL;
		IBaseFilter * webmAudioFilter = NULL;
		IVorbisEncodeSettings * webmAudio = NULL;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		CHECK_HR(hr = CoCreateInstance(CLSID_VorbisEncodeFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&webmAudioFilter));
		//CHECK_HR(hr = CoCreateInstance(CLSID_WebmVideo, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&webmVideoFilter));
		//CHECK_HR(hr = CoCreateInstance(CLSID_WebmMux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&webmMuxFilter));

		CHECK_HR(hr = g_dllmanager.CreateWebmmux(CLSID_WebmMux, &unknown));
		CHECK_HR(hr = unknown->QueryInterface(IID_IBaseFilter, (void **)&webmMuxFilter));
		SAFE_RELEASE(unknown);

		CHECK_HR(hr = g_dllmanager.CreateVP8Encoder(CLSID_WebmVideo, &unknown));
		CHECK_HR(hr = unknown->QueryInterface(IID_IBaseFilter, (void **)&webmVideoFilter));
		SAFE_RELEASE(unknown);

		CHECK_HR(hr = webmMuxFilter->QueryInterface(__uuidof(IWebmMux), (void **)&webmMux));
		CHECK_HR(hr = webmVideoFilter->QueryInterface(__uuidof(IVP8Encoder), (void **)&webmEnc));
		CHECK_HR(hr = webmAudioFilter->QueryInterface(CLSID_IVorbisEncodeSettings, (void **)&webmAudio));

		CHECK_HR(hr = webmMux->SetMuxMode(this->m_muxMode));

		CHECK_SUCCEED(hr = graphBuilder->AddFilter(webmMuxFilter, NULL));

		if (this->m_bitrateMode == BitrateMode::CBR)
		{
			//Konstantny bitrate
			CHECK_HR(hr = webmEnc->SetEndUsage(kEndUsageCBR));
		}
		else
		{
			//Variabilny bitrate
			CHECK_HR(hr = webmEnc->SetEndUsage(kEndUsageVBR));
		}

		if (this->m_vidBitrate > 0)
			CHECK_HR(hr = webmEnc->SetTargetBitrate(this->m_vidBitrate));

		if (this->m_percentQuality < 51)
		{
			CHECK_HR(hr = webmEnc->SetDeadline(kDeadlineRealtime));
		}
		else if (this->m_percentQuality < 70)
		{
			CHECK_HR(hr = webmEnc->SetDeadline(kDeadlineGoodQuality));
		}
		else
		{
			CHECK_HR(hr = webmEnc->SetDeadline(kDeadlineBestQuality));
		}

		CHECK_SUCCEED(hr = webmEnc->ApplySettings());

		if (this->m_audBitrate != 0)
			CHECK_HR(hr = webmAudio->setBitrateQualityMode(this->m_audBitrate));

		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));

		if (videoPin != NULL)
		{
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(webmVideoFilter, NULL));

			inputPin = DSEncoder::GetFirstPin(webmVideoFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, inputPin));
			SAFE_RELEASE(inputPin);

			outputPin = DSEncoder::GetFirstPin(webmVideoFilter, PINDIR_OUTPUT);
			inputPin = DSEncoder::GetFirstPin(webmMuxFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, inputPin));
			SAFE_RELEASE(outputPin);
			SAFE_RELEASE(inputPin);
		}

		if (audioPin != NULL)
		{
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(webmAudioFilter, NULL));

			inputPin = DSEncoder::GetFirstPin(webmAudioFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, inputPin));
			SAFE_RELEASE(inputPin);

			outputPin = DSEncoder::GetFirstPin(webmAudioFilter, PINDIR_OUTPUT);
			inputPin = DSEncoder::GetPin(webmMuxFilter, PINDIR_INPUT, 1);
			CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, inputPin));
			SAFE_RELEASE(outputPin);
			SAFE_RELEASE(inputPin);
		}

		outputPin = DSEncoder::GetFirstPin(webmMuxFilter, PINDIR_OUTPUT);
		CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, writerPin));
		SAFE_RELEASE(outputPin);

	done:

		SAFE_RELEASE(outputPin);
		SAFE_RELEASE(inputPin);
		SAFE_RELEASE(webmVideoFilter);
		SAFE_RELEASE(webmAudioFilter);
		SAFE_RELEASE(webmMuxFilter);
		SAFE_RELEASE(webmEnc);
		SAFE_RELEASE(webmAudio);
		SAFE_RELEASE(webmMux);
		SAFE_RELEASE(unknown);

		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);
		SAFE_RELEASE(subtitlePin);
		
		return hr;
	}

	GUID ContainerWEBM::GetSubtype()
	{ 
		return CLSID_WebmOut; 
	}

	HRESULT ContainerWMV::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin)
	{
		HRESULT hr = S_OK;

		IPin * inputPin = NULL;
		IWMProfile * profile = NULL;
		IConfigAsfWriter * config = NULL;
		IWMMediaProps * streamProp = NULL;
		IWMStreamConfig * streamConf = NULL;
		IWMVideoMediaProps * videoStreamProp = NULL;
		PIN_INFO pinInfo;
		pinInfo.pFilter = NULL;
		BYTE * wmTypeByte = NULL;

		DWORD iStream = 0, wmTypeCount;
		INT32 iAudioStream = -1, iVideoStream = -1;
		GUID streamType;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		CHECK_HR(hr = writerPin->QueryPinInfo(&pinInfo));
		CHECK_HR(hr = pinInfo.pFilter->QueryInterface(IID_IConfigAsfWriter, (void **)&config));
		CHECK_HR(hr = config->GetCurrentProfile(&profile));

		while (profile->GetStream(iStream, &streamConf) == S_OK)
		{
			CHECK_HR(hr = streamConf->QueryInterface(IID_IWMMediaProps, (void **)&streamProp));

			CHECK_HR(hr = streamConf->GetStreamType(&streamType));

			CHECK_HR(hr = streamProp->GetMediaType(NULL, &wmTypeCount));
			wmTypeByte = new BYTE[wmTypeCount];
			if (wmTypeByte == NULL)
				CHECK_HR(hr = E_OUTOFMEMORY);
			CHECK_HR(hr = streamProp->GetMediaType((WM_MEDIA_TYPE *)wmTypeByte, &wmTypeCount));
			WM_MEDIA_TYPE * wmType = (WM_MEDIA_TYPE*)wmTypeByte;

			if (streamType == WMMEDIATYPE_Video && videoPin != NULL && iVideoStream < 0)
			{
				VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)wmType->pbFormat;
				CHECK_HR(hr = streamConf->QueryInterface(IID_IWMVideoMediaProps, (void **)&videoStreamProp));

				CHECK_HR(hr = videoStreamProp->SetQuality(this->m_percentQuality));

				if (this->m_videoSubtype == WMVideoSubtype::WMMEDIASUBTYPE_WMV2)
				{
					wmType->subtype = WMMEDIASUBTYPE_WMV2;
					vih->bmiHeader.biCompression = MAKEFOURCC('W','M','V','2');
				}
				else if (this->m_videoSubtype == WMVideoSubtype::WMMEDIASUBTYPE_WMV3)
				{
					wmType->subtype = WMMEDIASUBTYPE_WMV3;
					vih->bmiHeader.biCompression = MAKEFOURCC('W','M','V','3');
				}

				RECT rcResolution;
				SetRect(&rcResolution, 0, 0, this->m_width, this->m_height);
				vih->rcSource = rcResolution;
				vih->rcTarget = rcResolution;
				vih->dwBitRate = this->m_vidBitrate;
				vih->AvgTimePerFrame = UNITS / this->m_fps;
				vih->bmiHeader.biWidth = rcResolution.right;
				vih->bmiHeader.biHeight = rcResolution.bottom;

				CHECK_HR(hr = streamConf->SetBitrate(this->m_vidBitrate));
				CHECK_HR(hr = streamProp->SetMediaType(wmType));
				CHECK_HR(hr = profile->ReconfigStream(streamConf));

				SAFE_RELEASE(videoStreamProp);
				iVideoStream = iStream;
				iStream++;
			}
			else if (streamType == WMMEDIATYPE_Audio && audioPin != NULL && iAudioStream < 0)
			{
				WAVEFORMATEX * wav = (WAVEFORMATEX*)wmType->pbFormat;

				CHECK_HR(hr = streamConf->SetBitrate(this->m_audBitrate));

				CHECK_HR(hr = streamProp->SetMediaType(wmType));
				CHECK_HR(hr = profile->ReconfigStream(streamConf));

				iAudioStream = iStream;
				iStream++;
			}
			else
			{
				CHECK_HR(hr = profile->RemoveStream(streamConf));
			}

			delete [] wmTypeByte;
			wmTypeByte = NULL;
			SAFE_RELEASE(streamConf);
		}

		CHECK_HR(hr = config->ConfigureFilterUsingProfile(profile));

		//Konfiguracia ffmpeg bez nastavenia vysky a sirky a fps
		UINT32 width = this->m_width, height = this->m_height, fps = this->m_fps;
		this->m_width = this->m_height = this->m_fps = 0;
		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));
		this->m_width = width;
		this->m_height = height;
		this->m_fps = fps;

		if (iVideoStream >= 0)
		{
			inputPin = DSEncoder::GetPin(pinInfo.pFilter, PINDIR_INPUT, iVideoStream);
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, inputPin));
			SAFE_RELEASE(inputPin);
		}

		if (iAudioStream >= 0)
		{
			inputPin = DSEncoder::GetPin(pinInfo.pFilter, PINDIR_INPUT, iAudioStream);
			CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, inputPin));
			SAFE_RELEASE(inputPin);
		}

	done:

		SAFE_RELEASE(inputPin);
		SAFE_RELEASE(profile);
		SAFE_RELEASE(config);
		SAFE_RELEASE(streamProp);
		SAFE_RELEASE(streamConf);
		SAFE_RELEASE(videoStreamProp);
		SAFE_RELEASE(pinInfo.pFilter);
		if (wmTypeByte == NULL)
			delete [] wmTypeByte;

		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);
		SAFE_RELEASE(subtitlePin);
		
		return hr;
	}

	HRESULT ContainerWMV::GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter)
	{
		HRESULT hr = S_OK;

		IBaseFilter * writer = NULL;
		IServiceProvider * provider = NULL;
		IWMWriterSink  * writerSink = NULL;
		IFileSinkFilter * fileSink = NULL;
		IWMWriterAdvanced2 * advWriter = NULL;
		TCHAR tempPath[MAX_PATH];

		CHECK_HR(hr = CoCreateInstance(CLSID_WMAsfWriter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&writer));
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(writer, NULL));

		CHECK_HR(hr = writer->QueryInterface(IID_IFileSinkFilter, (void**)&fileSink));

		//Vygeneruje prazdny temp subor - vzdy bude velkosti 0
		GetTempPath(MAX_PATH, tempPath);
		wcscat_s(tempPath, MAX_PATH, L"wmtempfile.tmp");
		CHECK_HR(hr = fileSink->SetFileName(tempPath, NULL));

		CHECK_HR(hr = writer->QueryInterface(IID_IServiceProvider, (void **)&provider));
		CHECK_HR(hr = provider->QueryService(IID_IWMWriterAdvanced2, IID_IWMWriterAdvanced2, (void **)&advWriter));

		while (advWriter->GetSink(0, &writerSink) == S_OK)
		{
			CHECK_HR(hr = advWriter->RemoveSink(writerSink));
			SAFE_RELEASE(writerSink);
		}

		writerSink = new WMSinkWriter(outputStream);
		if (writerSink == NULL)
		{
			CHECK_HR(hr = E_OUTOFMEMORY);
		}

		CHECK_HR(hr = advWriter->AddSink(writerSink));

		*writerFilter = writer;
		writer = NULL;

	done: 

		SAFE_RELEASE(fileSink);
		SAFE_RELEASE(writerSink);
		SAFE_RELEASE(provider);
		SAFE_RELEASE(advWriter);
		SAFE_RELEASE(writer);

		return hr;
	}

	HRESULT ContainerHMC::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin)
	{
		HRESULT hr = S_OK;

		IPin * tempPin = NULL;
		IUnknown * unknown = NULL;
		IBaseFilter * muxFilter = NULL;
		IHMCEncoder * encoderProp = NULL;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		CHECK_HR(hr = g_dllmanager.CreateHMCEncoder(CLSID_HMCEncoder, &unknown));
		CHECK_HR(hr = unknown->QueryInterface(IID_IBaseFilter, (void **)&muxFilter));
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(muxFilter, NULL));

		CHECK_HR(hr = muxFilter->QueryInterface(IID_IHMCEncoder, (void**)&encoderProp));

		CHECK_HR(hr = encoderProp->SetContainer(this->m_container));
		CHECK_HR(hr = encoderProp->SetStreamable(this->m_streamable));
		
		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));

		if (videoPin != NULL && this->m_container != Container_MP3)
		{
			if (this->m_vidBitrate > 0)
			{
				if (this->m_bitrateMode == BitrateMode::CBR)
				{
					//Konstantny bitrate
					CHECK_HR(hr = encoderProp->SetVideoCBR(this->m_vidBitrate));
				}
				else
				{
					//Variabilny bitrate
					CHECK_HR(hr = encoderProp->SetVideoVBR(this->m_vidBitrate, this->m_vidBitrate + 1000000, this->m_vidBitrate - 1000000, 
						this->m_percentQuality));
				}
			}

			CHECK_HR(hr = encoderProp->SetVideoInterlace(this->m_scanType == ScanType::Interlaced));

			tempPin = DSEncoder::GetPin(muxFilter, PINDIR_INPUT, 0);
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, tempPin));
			SAFE_RELEASE(tempPin);
		}

		if (audioPin != NULL)
		{
			if (this->m_audBitrate > 0)
			{
				if (this->m_bitrateMode == BitrateMode::CBR)
				{
					//Konstantny bitrate
					CHECK_HR(hr = encoderProp->SetAudioCBR(this->m_audBitrate));
				}
				else
				{
					//Variabilny bitrate
					CHECK_HR(hr = encoderProp->SetAudioVBR(this->m_audBitrate, this->m_audBitrate + 32, this->m_audBitrate - 32, 
						this->m_percentQuality));
				}
			}

			tempPin = DSEncoder::GetPin(muxFilter, PINDIR_INPUT, 1);
			CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, tempPin));
			SAFE_RELEASE(tempPin);
		}

		tempPin = DSEncoder::GetFirstPin(muxFilter, PINDIR_OUTPUT);
		CHECK_SUCCEED(hr = graphBuilder->Connect(tempPin, writerPin));

	done:

		SAFE_RELEASE(tempPin);
		SAFE_RELEASE(muxFilter);
		SAFE_RELEASE(encoderProp);
		SAFE_RELEASE(unknown);

		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);
		SAFE_RELEASE(subtitlePin);
		
		return hr;
	}
}
