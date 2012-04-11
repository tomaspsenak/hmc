#include "StdAfx.h"
#include "DSPlayer.h"
#include <Bdaiface.h>
#include <dvdmedia.h>
#include "SourceFilter.h"
#include "DSWrapper.h"

namespace DSWrapper 
{
	DSPlayer::DSPlayer(void)
	{
	}

	void DSPlayer::PlayMPEG2_TS(System::IO::Stream ^ stream, System::Boolean video, System::Boolean audio)
	{
		HRESULT hr = S_OK;
		ULONG pid;

		IGraphBuilder * graphBuilder = NULL;
		IFilterGraph * filterGraph = NULL;
		IMediaControl * mediaControl = NULL;
		IMediaEvent * mediaEvent = NULL;
		IBaseFilter * filter = NULL;

		IMpeg2Demultiplexer * pDemux;
		IMPEG2PIDMap * pPidMap;

		IPin * inputPin = NULL;
		IPin * outputPin = NULL;

		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));

		if (stream == nullptr)
			CHECK_HR(hr = E_INVALIDARG);

		//Vytvorenie zakladnych objektov
		CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IFilterGraph, (void **)&filterGraph));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IMediaControl, (void **)&mediaControl));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IMediaEvent, (void **)&mediaEvent));

		//Vytvorenie zdrojoveho filtra
		filter = new SourceFilter(NULL, &hr, stream);
		if (filter == NULL)
		{
			CHECK_HR(hr = E_OUTOFMEMORY);
		}
		else
		{
			filter->AddRef();
		}
		CHECK_HR(hr);

		CHECK_SUCCEED(hr = graphBuilder->AddFilter(filter, NULL));
		outputPin = DSEncoder::GetFirstPin(filter, PINDIR_OUTPUT);
		SAFE_RELEASE(filter);

		CHECK_HR(hr = CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&filter));
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(filter, NULL));
		inputPin = DSEncoder::GetFirstPin(filter, PINDIR_INPUT);

		CHECK_SUCCEED(hr = graphBuilder->Connect(outputPin, inputPin));
		SAFE_RELEASE(outputPin);
		SAFE_RELEASE(inputPin);
		
		CHECK_HR(hr = filter->QueryInterface(IID_IMpeg2Demultiplexer, (void**)&pDemux));

		if (video)
		{
			//Nastavenie video pin-u
			mt.majortype = MEDIATYPE_Video;
			mt.subtype = MEDIASUBTYPE_MPEG2_VIDEO;
			mt.formattype = FORMAT_MPEG2Video;
			mt.cbFormat = sizeof(MPEG2VIDEOINFO);
			mt.pbFormat = (BYTE*)CoTaskMemAlloc(mt.cbFormat);
			if (mt.pbFormat == NULL)
			{
				CHECK_HR(hr = E_OUTOFMEMORY);
			}
			ZeroMemory(mt.pbFormat, mt.cbFormat);

			//Priradenie id streamu
			pid = 0x1000;
			CHECK_HR(hr = pDemux->CreateOutputPin(&mt, L"Video Pin", &outputPin));
			CHECK_HR(hr = outputPin->QueryInterface(__uuidof(IMPEG2PIDMap), (void**)&pPidMap));
			CHECK_HR(hr = pPidMap->MapPID(1, &pid, MEDIA_ELEMENTARY_STREAM));
			CHECK_HR(hr = graphBuilder->Render(outputPin));

			SAFE_RELEASE(pPidMap);
			SAFE_RELEASE(outputPin);
			FreeMediaType(mt);
		}

		if (audio)
		{
			//Nastavenie audio pin-u
			pid = 0x1001;
			mt.majortype = MEDIATYPE_Audio;
			mt.subtype = MEDIASUBTYPE_MPEG2_AUDIO;
			mt.formattype = FORMAT_WaveFormatEx;
			mt.cbFormat = sizeof(WAVEFORMATEX);
			mt.pbFormat = (BYTE*)CoTaskMemAlloc(mt.cbFormat);
			if (mt.pbFormat == NULL)
			{
				CHECK_HR(hr = E_OUTOFMEMORY);
			}
			ZeroMemory(mt.pbFormat, mt.cbFormat);

			//Priradenie id streamu
			CHECK_HR(hr = pDemux->CreateOutputPin(&mt, L"Audio Pin", &outputPin));
			CHECK_HR(hr = outputPin->QueryInterface(__uuidof(IMPEG2PIDMap), (void**)&pPidMap));
			CHECK_HR(hr = pPidMap->MapPID(2, &pid, MEDIA_ELEMENTARY_STREAM));
			CHECK_HR(hr = graphBuilder->Render(outputPin));
		}
		
		//Spustenie prehravania
		CHECK_SUCCEED(hr = mediaControl->Run());

		long evCode;
		LONG_PTR param1, param2;
		BOOL continuePlay = TRUE;

		while (continuePlay)
		{
			if (mediaEvent->GetEvent(&evCode, &param1, &param2, 1000) == S_OK)
			{
				switch(evCode) 
				{ 
					case EC_COMPLETE:
					case EC_USERABORT:
						continuePlay = FALSE;
					case EC_ERRORABORT:
					case EC_ERRORABORTEX:
						hr = param1;
						mediaEvent->FreeEventParams(evCode, param1, param2);
						goto done;
					default: break;
				}
				CHECK_HR(hr = mediaEvent->FreeEventParams(evCode, param1, param2));
			}
		}

	done:

		if (mediaControl != NULL)
			mediaControl->Stop();

		SAFE_RELEASE(pDemux);
		SAFE_RELEASE(pPidMap);

		SAFE_RELEASE(inputPin);
		SAFE_RELEASE(outputPin);

		FreeMediaType(mt);

		SAFE_RELEASE(graphBuilder);
		SAFE_RELEASE(filterGraph);
		SAFE_RELEASE(mediaControl);
		SAFE_RELEASE(mediaEvent);
		SAFE_RELEASE(filter);
	}
}
