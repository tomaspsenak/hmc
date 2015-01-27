#include "StdAfx.h"
#include "ContainerType.h"
#include "DSWrapper.h"
#include "Extern/IffdshowBase.h"
#include <Ks.h>
#include <codecapi.h>
#include "Extern/IffDecoder.h"
#include "HMCEncoder_i.c"
#include "FileWriterFilter.h"
#include <wmsdk.h>
#include <dshowasf.h>
#include "WMSinkWriter.h"
#include "DSException.h"
#include "DSBufferedStream.h"
#include <InitGuid.h>
#include "DLLManager.h"
#include "WMPreFilter.h"
#include <wmcodecdsp.h>

DEFINE_GUID(CLSID_Mpeg2Encoder, 0x5F5AFF4A, 0x2F7F, 0x4279, 0x88, 0xC2, 0xCD, 0x88, 0xEB, 0x39, 0xD1, 0x44);
DEFINE_GUID(CLSID_FFDSHOWRaw, 0x0B390488, 0xD80F, 0x4A68, 0x84, 0x08, 0x48, 0xDC, 0x19, 0x9F, 0x0E, 0x97);
DEFINE_GUID(CLSID_LAVSplit, 0x171252A0, 0x8820, 0x4AFE, 0x9D, 0xF8, 0x5C, 0x92, 0xB2, 0xD6, 0x6B, 0x04);

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
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
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
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
		}
	}

	ContainerType ^ ContainerType::MPEG2_TS_H264(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
		UINT32 percentQuality, UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, 
		UINT32 audBitrate)
	{
		return gcnew ContainerHMC(Container_MPEG2TSH264, true, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
	}

	ContainerType ^ ContainerType::HLS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
		UINT32 percentQuality, UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio,
		UINT32 audBitrate, System::String ^ playlistUrl, System::String ^ fileUrl, UINT32 segmentTime)
	{
		return gcnew ContainerHMC(Container_MPEG2TSH264, true, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, playlistUrl, fileUrl, segmentTime);
	}
	
	ContainerType ^ ContainerType::WEBM(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate, bool streamable)
	{
		return gcnew ContainerHMC(Container_WEBM, streamable, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
				intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
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
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
	}

	ContainerType ^ ContainerType::MP4(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate, bool streamable)
	{
		return gcnew ContainerHMC(Container_MP4, streamable, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
	}

	ContainerType ^ ContainerType::MP3(BitrateMode bitrateMode, UINT32 audBitrate, UINT32 percentQuality, bool streamable)
	{
		return gcnew ContainerHMC(Container_MP3, streamable, 0, 0, bitrateMode, 0, percentQuality, 0, ScanType::Interlaced, false, nullptr, 
			false, audBitrate, nullptr, nullptr, 0);
	}

	ContainerType ^ ContainerType::FLV(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate, bool streamable)
	{
		return gcnew ContainerHMC(Container_FLV, streamable, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
	}

	ContainerType ^ ContainerType::FLV_H264(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
		UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate, bool streamable)
	{
		return gcnew ContainerHMC(Container_FLVH264, streamable, width, height, bitrateMode, vidBitrate, percentQuality, fps, ScanType::Progressive, 
			intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate, nullptr, nullptr, 0);
	}

	System::Boolean ContainerType::IsMPEG2Installed(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_Mpeg2Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
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
		HRESULT hr = CoCreateInstance(CLSID_FFDSHOW, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	System::Boolean ContainerType::IsHMCInstalled(void)
	{
		IUnknown * filter = NULL;
		HRESULT hr = DLLManager::GetManager().CreateHMCEncoder(CLSID_HMCEncoder, &filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	System::Boolean ContainerType::IsLAVSplitInstalled(void)
	{
		IBaseFilter * filter = NULL;
		HRESULT hr = CoCreateInstance(CLSID_LAVSplit, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter);
		SAFE_RELEASE(filter);

		return hr == S_OK;
	}

	HRESULT ContainerType::ConfigureDecoder(IGraphBuilder * graphBuilder, IPin ** pVideoPin, IPin ** pAudioPin, IPin ** pSubtitlePin)
	{
		HRESULT hr = S_OK;

		IPin * tempPin = NULL;
		IUnknown * framerateUnk = NULL;
		IBaseFilter * ffVideoDecoder = NULL;
		IBaseFilter * framerateFilter = NULL;
		IffdshowBaseW * ffVideoConfig = NULL;
		IHMCFrameRate * framerateParams = NULL;
		IHMCDesktopSource * desktopSourceParams = NULL;

		if ((*pVideoPin) != NULL)
		{
			if ((*pVideoPin)->QueryInterface(IID_IHMCDesktopSource, (void **)&desktopSourceParams) == S_OK)
			{
				if (this->m_width > 0)
					CHECK_HR(hr = desktopSourceParams->SetWidth(this->m_width));

				if (this->m_height > 0)
					CHECK_HR(hr = desktopSourceParams->SetHeight(this->m_height));

				CHECK_HR(hr = desktopSourceParams->SetAspectRatio(this->m_keepAspectRatio));
			}
			else if (this->m_intSubtitles || this->m_width > 0 || this->m_height > 0)
			{
				//CLSID_FFDSHOW nepodporuje typy ako RGB32,.. - vtedy treba pouzit CLSID_FFDSHOWRaw
				CHECK_HR(hr = CoCreateInstance(CLSID_FFDSHOW, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&ffVideoDecoder));
				CHECK_SUCCEED(hr = graphBuilder->AddFilter(ffVideoDecoder, NULL));

				if (this->m_intSubtitles)
				{
					//Problem s novsim ffdshow - titulkovy pin funguje iba ked su titulky zapnute
					#ifdef _M_X64
						Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow64\\default", "isSubtitles", 1);
					#else
						Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow\\default", "isSubtitles", 1);
					#endif
				}

				tempPin = DSEncoder::GetFirstPin(ffVideoDecoder, PINDIR_INPUT);
				if(graphBuilder->ConnectDirect(*pVideoPin, tempPin, NULL) >= 0)
				{
					//pouzity bude CLSID_FFDSHOW
					SAFE_RELEASE(tempPin);
				}
				else
				{
					//pouzity bude CLSID_FFDSHOWRaw
					CHECK_HR(hr = graphBuilder->RemoveFilter(ffVideoDecoder));
					SAFE_RELEASE(ffVideoDecoder);
					SAFE_RELEASE(tempPin);

					CHECK_HR(hr = CoCreateInstance(CLSID_FFDSHOWRaw, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&ffVideoDecoder));
					CHECK_SUCCEED(hr = graphBuilder->AddFilter(ffVideoDecoder, NULL));

					if (this->m_intSubtitles)
					{
						//Problem s novsim ffdshow - titulkovy pin funguje iba ked su titulky zapnute
						#ifdef _M_X64
							Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow64_raw\\default", "isSubtitles", 1);
						#else
							Microsoft::Win32::Registry::SetValue(L"HKEY_CURRENT_USER\\Software\\GNU\\ffdshow_raw\\default", "isSubtitles", 1);
						#endif
					}

					tempPin = DSEncoder::GetFirstPin(ffVideoDecoder, PINDIR_INPUT);
					CHECK_SUCCEED(hr = graphBuilder->Connect(*pVideoPin, tempPin));
					SAFE_RELEASE(tempPin);
				}
			
				CHECK_HR(hr = ffVideoDecoder->QueryInterface(IID_IffdshowBaseW, (void **)&ffVideoConfig));

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
				else
				{
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_isSubtitles, 0));
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
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeMode, 0));
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeIf, 0));
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDx, this->m_width));
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDy, this->m_height));
					CHECK_HR(hr = ffVideoConfig->putParam(IDFF_resizeDy_real, this->m_height));
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
		}

		if (this->m_fps > 0 && (*pVideoPin) != NULL)
		{
			//Nastavenie snimkov za sekundu ak je hodnota vacsia ako 0, inak povodna hodnota
			CHECK_HR(hr = DLLManager::GetManager().CreateHMCEncoder(CLSID_HMCFrameRate, &framerateUnk));
			CHECK_HR(hr = framerateUnk->QueryInterface(IID_IBaseFilter, (void **)&framerateFilter));
			CHECK_HR(hr = framerateUnk->QueryInterface(IID_IHMCFrameRate, (void **)&framerateParams));

			CHECK_HR(hr = framerateParams->SetFrameRate(this->m_fps));

			CHECK_SUCCEED(hr = graphBuilder->AddFilter(framerateFilter, NULL));

			tempPin = DSEncoder::GetFirstPin(framerateFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(*pVideoPin, tempPin));
			SAFE_RELEASE(tempPin);

			SAFE_RELEASE(*pVideoPin);
			*pVideoPin = DSEncoder::GetFirstPin(framerateFilter, PINDIR_OUTPUT);
		}

	done:

		SAFE_RELEASE(tempPin);
		SAFE_RELEASE(framerateUnk);
		SAFE_RELEASE(ffVideoDecoder);
		SAFE_RELEASE(framerateFilter);
		SAFE_RELEASE(ffVideoConfig);
		SAFE_RELEASE(framerateParams);
		SAFE_RELEASE(desktopSourceParams);

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

	HRESULT ContainerMPEG2_PS::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, 
		IPin * writerPin, IMediaSeeking ** mediaSeekingMux)
	{
		HRESULT hr = S_OK;
		ULONG vValCount = 0;

		VARIANT variant;
		VARIANT * vVal = NULL;
		IPin * tempPin = NULL;
		ICodecAPI * codecApi = NULL;
		IBaseFilter * codecFilter = NULL;

		VariantInit(&variant);
		variant.vt = VT_UI4;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		//Najprv nastavit piny az potom pridat enkoder do grafu (piny sa pokusaju na vsetky filtre v grafe)
		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));

		CHECK_HR(hr = CoCreateInstance(CLSID_Mpeg2Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&codecFilter));
		CHECK_HR(hr = codecFilter->QueryInterface(IID_ICodecAPI, (void **)&codecApi));
		codecFilter->QueryInterface(IID_IMediaSeeking, (void**)mediaSeekingMux);

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

	HRESULT ContainerWMV::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, 
		IPin * writerPin, IMediaSeeking ** mediaSeekingMux)
	{
		HRESULT hr = S_OK;

		IPin * inputPin = NULL;
		IPin * outputPin = NULL;
		PreFilter * preFilter = NULL;
		IWMProfile * profile = NULL;
		IConfigAsfWriter * config = NULL;
		IWMStreamConfig * streamConf = NULL;
		IWMMediaProps * mediaProp = NULL;
		IWMVideoMediaProps * videoStreamProp = NULL;
		IWMProfileManager * profileManager = NULL;
		IWMCodecInfo * codecInfo = NULL;
		PIN_INFO pinInfo;
		pinInfo.pFilter = NULL;

		WM_MEDIA_TYPE * pWmMediaType = NULL;
		AM_MEDIA_TYPE mediaType;
		ZeroMemory(&mediaType, sizeof(AM_MEDIA_TYPE));

		DWORD iStream = 0;
		INT32 iAudioStream = -1, iVideoStream = -1;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		//Najprv nastavit piny az potom pridat enkoder do grafu (piny sa pokusaju na vsetky filtre v grafe)
		//Konfiguracia ffmpeg bez nastavenia vysky a sirky a fps
		UINT32 width = this->m_width, height = this->m_height, fps = this->m_fps;
		this->m_width = this->m_height = this->m_fps = 0;
		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));
		this->m_width = width;
		this->m_height = height;
		this->m_fps = fps;

		CHECK_HR(hr = WMCreateProfileManager(&profileManager));
		CHECK_HR(hr = profileManager->QueryInterface(IID_IWMCodecInfo, (void **)&codecInfo));

		CHECK_HR(hr = profileManager->CreateEmptyProfile(WMT_VER_9_0, &profile));
		CHECK_HR(hr = profile->SetName(L"profile"));

		if (videoPin != NULL)
		{
			//Vytvorenie video pinu - ak je potrebny
			CHECK_HR(hr = profile->CreateNewStream(WMMEDIATYPE_Video, &streamConf));
			CHECK_HR(hr = streamConf->QueryInterface(IID_IWMMediaProps, (void **)&mediaProp));
			CHECK_HR(hr = streamConf->QueryInterface(IID_IWMVideoMediaProps, (void **)&videoStreamProp));

			CHECK_HR(hr = streamConf->SetStreamName(L"video stream"));
			CHECK_HR(hr = streamConf->SetStreamNumber((WORD)iStream + 1));
			CHECK_HR(hr = streamConf->SetBitrate(this->m_vidBitrate));
			CHECK_HR(hr = streamConf->SetBufferWindow(5000));
			CHECK_HR(hr = videoStreamProp->SetQuality(this->m_percentQuality));
			CHECK_HR(hr = videoStreamProp->SetMaxKeyFrameSpacing(100000000));

			WMVIDEOINFOHEADER * videoInfo = (WMVIDEOINFOHEADER *)CoTaskMemAlloc(sizeof(WMVIDEOINFOHEADER));
			if (videoInfo == NULL)
			{
				CHECK_HR(hr = E_OUTOFMEMORY);
			}
			ZeroMemory(videoInfo, sizeof(WMVIDEOINFOHEADER));

			mediaType.majortype = WMMEDIATYPE_Video;
			mediaType.subtype = (this->m_videoSubtype == WMVideoSubtype::WMMEDIASUBTYPE_WMV2) ? WMMEDIASUBTYPE_WMV2 : WMMEDIASUBTYPE_WMV3;
			mediaType.bFixedSizeSamples = FALSE;
			mediaType.bTemporalCompression = TRUE;
			mediaType.lSampleSize = 0;
			mediaType.formattype = WMFORMAT_VideoInfo;
			mediaType.cbFormat = sizeof(WMVIDEOINFOHEADER);
			mediaType.pbFormat = (BYTE *)videoInfo;
			videoInfo->bmiHeader.biWidth = videoInfo->rcTarget.right = videoInfo->rcSource.right = this->m_width;
			videoInfo->bmiHeader.biHeight = videoInfo->rcTarget.bottom = videoInfo->rcSource.bottom = this->m_height;
			videoInfo->dwBitRate = this->m_vidBitrate;
			videoInfo->AvgTimePerFrame = UNITS / this->m_fps;
			videoInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			videoInfo->bmiHeader.biPlanes = 1;
			videoInfo->bmiHeader.biBitCount = 24;
			videoInfo->bmiHeader.biCompression = (this->m_videoSubtype == WMVideoSubtype::WMMEDIASUBTYPE_WMV2) ? MAKEFOURCC('W','M','V','2') : MAKEFOURCC('W','M','V','3');
			videoInfo->bmiHeader.biSizeImage = 0;

			CHECK_HR(hr = mediaProp->SetMediaType((WM_MEDIA_TYPE *)&mediaType));

			CHECK_HR(hr = profile->AddStream(streamConf));
			iVideoStream = iStream++;

			FreeMediaType(mediaType);
			SAFE_RELEASE(videoStreamProp);
			SAFE_RELEASE(mediaProp);
			SAFE_RELEASE(streamConf);
		}

		if (audioPin != NULL)
		{
			//Vytvorenie audio pinu - ak je potrebny
			DWORD cCodecs = 0, bestRatio = UINT_MAX, bestCodec = 0, bestFormat = 0;
			GUID audioSubtype = (this->m_videoSubtype == WMVideoSubtype::WMMEDIASUBTYPE_WMV2) ? MEDIASUBTYPE_WMAUDIO2 : MEDIASUBTYPE_WMAUDIO3;
			CHECK_HR(hr = codecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &cCodecs));

			//Prejdenie vsetkych podporovanych konfiguracii a najdenie najlepsej vhodnej
			for (DWORD i = 0; i < cCodecs; i++)
			{
				DWORD cFormat = 0, bufferLen = 0;
				CHECK_HR(hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, i, &cFormat));
				if (cFormat == 0)
					continue;

				for (DWORD j = 0; j < cFormat; j++)
				{
					CHECK_HR(hr = codecInfo->GetCodecFormat(WMMEDIATYPE_Audio, i, j, &streamConf));
					CHECK_HR(hr = streamConf->QueryInterface(IID_IWMMediaProps, (void **)&mediaProp));

					CHECK_HR(hr = mediaProp->GetMediaType(NULL, &bufferLen));
					pWmMediaType = (WM_MEDIA_TYPE *)CoTaskMemAlloc(bufferLen);
					CHECK_HR(hr = mediaProp->GetMediaType(pWmMediaType, &bufferLen));
					WAVEFORMATEX * waveFormat = (WAVEFORMATEX *)pWmMediaType->pbFormat;

					if (pWmMediaType->subtype == audioSubtype)
					{
						if (waveFormat->nChannels == 2 && waveFormat->nSamplesPerSec == 44100)
						{
							DWORD bitrate = 0;
							CHECK_HR(hr = streamConf->GetBitrate(&bitrate));
							bitrate = abs((int)bitrate - (int)this->m_audBitrate);

							if (bitrate < bestRatio)
							{
								bestRatio = bitrate;
								bestCodec = i;
								bestFormat = j;
							}
						}
					}
					else
					{
						cFormat = 0;
					}

					CoTaskMemFree(pWmMediaType);
					pWmMediaType = NULL;
					SAFE_RELEASE(mediaProp);
					SAFE_RELEASE(streamConf);
				}
			}
			
			//Nastavenie najlepsej vhodnej konfiguracie
			CHECK_HR(hr = codecInfo->GetCodecFormat(WMMEDIATYPE_Audio, bestCodec, bestFormat, &streamConf));
			CHECK_HR(hr = streamConf->SetStreamName(L"audio stream"));
			CHECK_HR(hr = streamConf->SetStreamNumber((WORD)iStream + 1));
			CHECK_HR(hr = streamConf->SetBufferWindow(5000));

			CHECK_HR(hr = profile->AddStream(streamConf));
			iAudioStream = iStream++;

			SAFE_RELEASE(streamConf);
		}

		CHECK_HR(hr = writerPin->QueryPinInfo(&pinInfo));
		CHECK_HR(hr = pinInfo.pFilter->QueryInterface(IID_IConfigAsfWriter, (void **)&config));
		pinInfo.pFilter->QueryInterface(IID_IMediaSeeking, (void**)mediaSeekingMux);

		CHECK_HR(hr = config->ConfigureFilterUsingProfile(profile));

		if (iVideoStream >= 0)
		{
			//WMVideoPreFilter nastavi biPlanes na 1 - ine hodnoty nie su podporovane cez WMAsfWriter (bug)
			preFilter = new WMVideoPreFilter(NULL, &hr);
			if (preFilter == NULL)
				hr = E_OUTOFMEMORY;
			else
				preFilter->AddRef();
			CHECK_HR(hr);

			CHECK_SUCCEED(hr = graphBuilder->AddFilter(preFilter, NULL));

			inputPin = DSEncoder::GetFirstPin(preFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, inputPin));
			SAFE_RELEASE(inputPin);

			outputPin = DSEncoder::GetFirstPin(preFilter, PINDIR_OUTPUT);
			inputPin = DSEncoder::GetPin(pinInfo.pFilter, PINDIR_INPUT, iVideoStream);
			CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, inputPin));
			//CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, inputPin));
			SAFE_RELEASE(outputPin);
			SAFE_RELEASE(inputPin);
			SAFE_RELEASE(preFilter);
		}

		if (iAudioStream >= 0)
		{
			//WMAudioPreFilter nastavi wFormatTag na WAVE_FORMAT_PCM
			//Ine typy niekedy presli ale boli nekorektne spracovane
			preFilter = new WMAudioPreFilter(NULL, &hr);
			if (preFilter == NULL)
				hr = E_OUTOFMEMORY;
			else
				preFilter->AddRef();
			CHECK_HR(hr);

			CHECK_SUCCEED(hr = graphBuilder->AddFilter(preFilter, NULL));

			inputPin = DSEncoder::GetFirstPin(preFilter, PINDIR_INPUT);
			CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, inputPin));
			SAFE_RELEASE(inputPin);

			outputPin = DSEncoder::GetFirstPin(preFilter, PINDIR_OUTPUT);
			inputPin = DSEncoder::GetPin(pinInfo.pFilter, PINDIR_INPUT, iAudioStream);
			CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, inputPin));
			//CHECK_SUCCEED(hr = graphBuilder->Connect(audioPin, inputPin));
			SAFE_RELEASE(outputPin);
			SAFE_RELEASE(inputPin);
			SAFE_RELEASE(preFilter);
		}

	done:

		SAFE_RELEASE(profileManager);
		SAFE_RELEASE(inputPin);
		SAFE_RELEASE(outputPin);
		SAFE_RELEASE(preFilter);
		SAFE_RELEASE(profile);
		SAFE_RELEASE(config);
		SAFE_RELEASE(streamConf);
		SAFE_RELEASE(mediaProp);
		SAFE_RELEASE(videoStreamProp);
		SAFE_RELEASE(pinInfo.pFilter);
		SAFE_RELEASE(codecInfo);

		FreeMediaType(mediaType);
		CoTaskMemFree(pWmMediaType);

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

		if (outputStream == nullptr)
			return E_FAIL;

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

	HRESULT ContainerHMC::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, 
		IPin * writerPin, IMediaSeeking ** mediaSeekingMux)
	{
		HRESULT hr = S_OK;

		IPin * tempPin = NULL;
		IUnknown * unknown = NULL;
		IBaseFilter * muxFilter = NULL;
		IHMCEncoder * encoderProp = NULL;

		char * pHlsFileUrl = NULL;
		char * pHlsPlaylistUrl = NULL;

		//Pridat referenciu - tam kde sa hodnota prepise, treba SAFE_RELEASE
		SAFE_ADDREF(videoPin);
		SAFE_ADDREF(audioPin);
		SAFE_ADDREF(subtitlePin);

		//Najprv nastavit piny az potom pridat enkoder do grafu (piny sa pokusaju na vsetky filtre v grafe)
		CHECK_HR(hr = ConfigureDecoder(graphBuilder, &videoPin, &audioPin, &subtitlePin));

		CHECK_HR(hr = DLLManager::GetManager().CreateHMCEncoder(CLSID_HMCEncoder, &unknown));
		CHECK_HR(hr = unknown->QueryInterface(IID_IBaseFilter, (void **)&muxFilter));
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(muxFilter, NULL));

		CHECK_HR(hr = muxFilter->QueryInterface(IID_IHMCEncoder, (void**)&encoderProp));
		muxFilter->QueryInterface(IID_IMediaSeeking, (void**)mediaSeekingMux);

		if (this->m_hlsSegmentTime > 0 && this->m_hlsPlaylistUrl != nullptr && this->m_hlsFileUrl != nullptr)
		{
			pin_ptr<const wchar_t> pPtr;
			size_t convertedChars = 0;
			size_t sizeInBytes;
			errno_t err;
			
			pPtr = PtrToStringChars(this->m_hlsPlaylistUrl);
			sizeInBytes = ((this->m_hlsPlaylistUrl->Length + 1) * 2);
			pHlsPlaylistUrl = (char *)malloc(sizeInBytes);
			if (pHlsPlaylistUrl == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto done;
			}
			err = wcstombs_s(&convertedChars, pHlsPlaylistUrl, sizeInBytes, pPtr, sizeInBytes);
			if (err != 0)
			{
				hr = E_FAIL;
				goto done;
			}

			pPtr = PtrToStringChars(this->m_hlsFileUrl);
			sizeInBytes = ((this->m_hlsFileUrl->Length + 1) * 2);
			pHlsFileUrl = (char *)malloc(sizeInBytes);
			if (pHlsFileUrl == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto done;
			}
			err = wcstombs_s(&convertedChars, pHlsFileUrl, sizeInBytes, pPtr, sizeInBytes);
			if (err != 0)
			{
				hr = E_FAIL;
				goto done;
			}

			CHECK_HR(hr = encoderProp->SetHlsSegmenter(pHlsPlaylistUrl, pHlsFileUrl, this->m_hlsSegmentTime, TRUE));
		}
		else
		{
			CHECK_HR(hr = encoderProp->SetContainer(this->m_container));
			CHECK_HR(hr = encoderProp->SetStreamable(this->m_streamable));
		}

		if (videoPin != NULL && this->m_container != Container_MP3)
		{
			if (this->m_vidBitrate > 0)
			{
				if (this->m_bitrateMode == BitrateMode::CBR)
				{
					//Konstantny bitrate
					CHECK_HR(hr = encoderProp->SetVideoCBR(this->m_vidBitrate));
					CHECK_HR(hr = encoderProp->SetVideoQuality(this->m_percentQuality));
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

		free(pHlsFileUrl);
		free(pHlsPlaylistUrl);
		
		return hr;
	}

	HRESULT ContainerJPEG::GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter)
	{
		HRESULT hr = S_OK;

		IUnknown * unknown = NULL;
		IBaseFilter * writer = NULL;
		IHMCFrameWriter * writerProp = NULL;
		IFileSinkFilter * writerSink = NULL;

		if (this->m_imgPath == nullptr)
			return E_FAIL;
		pin_ptr<const wchar_t> pImgPath = PtrToStringChars(this->m_imgPath);

		CHECK_HR(hr = DLLManager::GetManager().CreateHMCEncoder(CLSID_HMCFrameWriter, &unknown));
		CHECK_HR(hr = unknown->QueryInterface(IID_IBaseFilter, (void **)&writer));
		CHECK_HR(hr = unknown->QueryInterface(IID_IHMCFrameWriter, (void**)&writerProp));
		CHECK_HR(hr = unknown->QueryInterface(IID_IFileSinkFilter, (void **)&writerSink));
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(writer, NULL));
		
		CHECK_HR(hr = writerSink->SetFileName(pImgPath, NULL));

		CHECK_HR(hr = writerProp->SetFormat(ImageFormat_JPEG));
		CHECK_HR(hr = writerProp->SetWidth(this->m_width));
		CHECK_HR(hr = writerProp->SetHeight(this->m_height));
		CHECK_HR(hr = writerProp->SetBitrate(this->m_vidBitrate));

		*writerFilter = writer;
		writer = NULL;

	done: 

		SAFE_RELEASE(writerSink);
		SAFE_RELEASE(writerProp);
		SAFE_RELEASE(writer);
		SAFE_RELEASE(unknown);

		return hr;
	}

	HRESULT ContainerJPEG::ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, 
		IPin * writerPin, IMediaSeeking ** mediaSeekingMux)
	{
		HRESULT hr = S_OK;

		if (videoPin != NULL)
		{
			CHECK_SUCCEED(hr = graphBuilder->Connect(videoPin, writerPin));
		}

	done:
		return hr;
	}
}
