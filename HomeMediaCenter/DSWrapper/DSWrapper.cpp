#include "stdafx.h"
#include "DSWrapper.h"
#include "DSException.h"
#include <InitGuid.h>

#pragma warning(disable : 4995)
#include <list>

//Struktura pre titulky integrovane v kontajneri
struct SUBTITLEINFO 
{
	DWORD dwOffset; // size of the structure/pointer to codec init data
	CHAR IsoLang[4]; // three letter lang code + terminating zero
	WCHAR TrackName[256];
};
DEFINE_GUID(MEDIATYPE_Subtitle, 0xE487EB08, 0x6B26, 0x4be9, 0x9D, 0xD3, 0x99, 0x34, 0x34, 0xD3, 0x13, 0xFD);
DEFINE_GUID(SubtitleInfo, 0xA33D2F7D, 0x96BC, 0x4337, 0xB2, 0x3B, 0xA8, 0xB9, 0xFB, 0xC2, 0x95, 0xE9);

namespace DSWrapper 
{
	DSEncoder::DSEncoder(void) : m_filterGraph(NULL), m_outputStream(nullptr), m_sourceFilter(NULL), m_demuxFilter(NULL), m_containerType(nullptr),
		m_continueEncode(FALSE), m_startTime(0), m_endTime(0)
	{
		this->m_sourcePins = gcnew System::Collections::Generic::List<PinInfoItem^>();
	}

	DSEncoder::~DSEncoder(void)
	{
		Dispose(TRUE);
		System::GC::SuppressFinalize(this);
	}

	DSEncoder::!DSEncoder(void)
	{
		Dispose(FALSE);
	}

	void DSEncoder::SetInput(System::String ^ filePath)
	{
		SetInput(filePath, false);
	}

	void DSEncoder::SetInput(System::String ^ filePath, System::Boolean reqSeeking)
	{
		HRESULT hr = S_OK;

		System::Uri^ uri = nullptr;
		System::String^ extension = nullptr;

		IBaseFilter * sourceFilter = NULL;
		IGraphBuilder * graphBuilder = NULL;
		IFilterGraph * filterGraph = NULL;
		IFileSourceFilter * fileSource = NULL;

		IBaseFilter * demuxFilter = NULL;
		IPin * outputPin = NULL;

		try 
		{ 
			uri = gcnew System::Uri(filePath); 
			extension = uri->Segments[uri->Segments->Length - 1];
			extension = System::IO::Path::GetExtension(extension)->TrimStart('.')->ToLower();
		}
		catch (System::Exception^) 
		{
			hr = E_FAIL;
			goto done;
		}

		if (extension == "wma" || extension == "wmv" || extension == "asf")
		{
			//Pre windows media format treba specialny reader/demultiplexed
			pin_ptr<const wchar_t> pFilePath = PtrToStringChars(uri->LocalPath);

			CHECK_HR(hr = CoCreateInstance(CLSID_WMAsfReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&demuxFilter));
			CHECK_HR(hr = demuxFilter->QueryInterface(IID_IFileSourceFilter, (void **)&fileSource));
			CHECK_HR(hr = fileSource->Load(pFilePath, NULL));
		}
		else if (uri->IsFile)
		{
			//Zdroj je suborovy system
			pin_ptr<const wchar_t> pFilePath = PtrToStringChars(uri->LocalPath);

			CHECK_HR(hr = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&sourceFilter));
			CHECK_HR(hr = sourceFilter->QueryInterface(IID_IFileSourceFilter, (void **)&fileSource));
			CHECK_HR(hr = fileSource->Load(pFilePath, NULL));
		}
		else
		{
			//Zdroj je URL adresa
			pin_ptr<const wchar_t> pFilePath = PtrToStringChars(uri->AbsoluteUri);

			CHECK_HR(hr = CoCreateInstance(CLSID_URLReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&sourceFilter));
			CHECK_HR(hr = sourceFilter->QueryInterface(IID_IFileSourceFilter, (void **)&fileSource));
			CHECK_HR(hr = fileSource->Load(pFilePath, NULL));
		}

		//Vytvorenie zakladnych objektov
		CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IFilterGraph, (void **)&filterGraph));

		if (demuxFilter == NULL)
		{
			//Ak nebol priradeny demultiplexor
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(sourceFilter, NULL));
			outputPin = GetFirstPin(sourceFilter, PINDIR_OUTPUT);

			//Najprv sa pokusi najst demultiplexor s podporou seekovania
			if (reqSeeking)
			{
				if (FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, TRUE) != S_OK)
					CHECK_HR(hr = FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, FALSE));
			}
			else
			{
				CHECK_HR(hr = FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, FALSE));
			}
		}
		else
		{
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(demuxFilter, NULL));
		}

		this->m_sourcePins = GetPinInfo(demuxFilter, PINDIR_OUTPUT);

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IFilterGraph*> pFilterGraph = &this->m_filterGraph;
		SafeRelease<IFilterGraph>(pFilterGraph);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IBaseFilter*> pDemuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(pDemuxFilter);

		this->m_filterGraph = filterGraph;
		filterGraph = NULL;
		this->m_sourceFilter = sourceFilter;
		sourceFilter = NULL;
		this->m_demuxFilter = demuxFilter;
		demuxFilter = NULL;

	done:

		SAFE_RELEASE(sourceFilter);
		SAFE_RELEASE(fileSource);
		SAFE_RELEASE(graphBuilder);
		SAFE_RELEASE(filterGraph);

		SAFE_RELEASE(demuxFilter);
		SAFE_RELEASE(outputPin);

		if(hr != S_OK)
			throw gcnew DSException(L"Can not read from a file", hr);
	}

	void DSEncoder::SetInput(InputType ^ inputType)
	{
		HRESULT hr = S_OK;

		IGraphBuilder * graphBuilder = NULL;
		IFilterGraph * filterGraph = NULL;
		IBaseFilter * sourceFilter = NULL;
		IAMGraphStreams * graphStreams = NULL;

		if (inputType == nullptr)
			CHECK_HR(hr = E_INVALIDARG);

		//Vytvorenie zakladnych objektov
		CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IFilterGraph, (void **)&filterGraph));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IAMGraphStreams, (void **)&graphStreams));
		//Nastavenie synchronizacie pre live sources
		graphStreams->SyncUsingStreamOffset(TRUE);

		CHECK_HR(hr = inputType->GetInputFilter(&sourceFilter));
		
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(sourceFilter, NULL));

		this->m_sourcePins = GetPinInfo(sourceFilter, PINDIR_OUTPUT);

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IFilterGraph*> pFilterGraph = &this->m_filterGraph;
		SafeRelease<IFilterGraph>(pFilterGraph);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IBaseFilter*> pDemuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(pDemuxFilter);

		this->m_filterGraph = filterGraph;
		filterGraph = NULL;
		this->m_demuxFilter = sourceFilter;
		sourceFilter = NULL;

	done:

		SAFE_RELEASE(graphBuilder);
		SAFE_RELEASE(filterGraph);
		SAFE_RELEASE(sourceFilter);
		SAFE_RELEASE(graphStreams);

		if(hr != S_OK)
			throw gcnew DSException(L"Unable to initialize input", hr);
	}

	void DSEncoder::SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType)
	{
		SetOutput(outputStream, containerType, 0, 0);
	}

	void DSEncoder::SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType, System::Int64 startTime, System::Int64 endTime)
	{
		HRESULT hr = S_OK;

		if (startTime < 0 || endTime < 0 || (endTime != 0 && startTime > endTime))
			CHECK_HR(hr = E_INVALIDARG);

		this->m_startTime = startTime;
		this->m_endTime = endTime;

		this->m_outputStream = outputStream;

		this->m_containerType = containerType;

	done: 

		if(hr != S_OK)
			throw gcnew DSException(L"Unable to create output", hr);
	}

	void DSEncoder::StartEncode(void)
	{
		HRESULT hr = S_OK;

		IGraphBuilder * graphBuilder = NULL;
		IMediaControl * mediaControl = NULL;
		IMediaEvent * mediaEvent = NULL;
		IMediaSeeking * mediaSeeking = NULL;
		IBaseFilter * writerFilter = NULL;

		IPin * videoPin = NULL;
		IPin * audioPin = NULL;
		IPin * subtitlePin = NULL;
		IPin * writerPin = NULL;

		CHECK_HR(hr = (this->m_outputStream == nullptr) ? E_FAIL : S_OK);
		CHECK_HR(hr = (this->m_containerType == nullptr) ? E_FAIL : S_OK);
		CHECK_HR(hr = (this->m_filterGraph == NULL) ? E_FAIL : S_OK);
		CHECK_HR(hr = (this->m_demuxFilter == NULL) ? E_FAIL : S_OK);

		CHECK_HR(hr = this->m_filterGraph->QueryInterface(IID_IGraphBuilder, (void **)&graphBuilder));
		CHECK_HR(hr = this->m_filterGraph->QueryInterface(IID_IMediaControl, (void **)&mediaControl));
		CHECK_HR(hr = this->m_filterGraph->QueryInterface(IID_IMediaEvent, (void **)&mediaEvent));
		CHECK_HR(hr = this->m_filterGraph->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking));
		
		//Vycisti graf a zisti audio a video pin podla nastavenia a demultiplexora
		CHECK_HR(hr = ClearGraph());
		CHECK_HR(hr = GetSourcePins(&videoPin, &audioPin, &subtitlePin));

		//Ziska writerFilter podla kontajnera, prida ho do grafu a zisti jeho vstupny pin
		CHECK_HR(hr = this->m_containerType->GetWriter(this->m_outputStream, graphBuilder, &writerFilter));
		writerPin = GetFirstPin(writerFilter, PINDIR_INPUT);

		//Nastavi spojenie decoder -> coder -> muxer -> writer
		CHECK_HR(hr = this->m_containerType->ConfigureContainer(graphBuilder, videoPin, audioPin, subtitlePin, writerPin));

		long evCode;
		LONG_PTR param1, param2;
		LONGLONG startTime = this->m_startTime;
		LONGLONG endTime = this->m_endTime;
		LONGLONG position = 0;
		System::Double progress = 0.0;
		
		//Nastavenie zaciatocneho a konecneho casu
		hr = mediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
		hr = mediaSeeking->SetPositions(&startTime, (startTime > 0) ? AM_SEEKING_AbsolutePositioning : AM_SEEKING_NoPositioning, 
			&endTime, (endTime > 0) ? AM_SEEKING_AbsolutePositioning : AM_SEEKING_NoPositioning);
		//Ak neni nastaveny endTime, nastavi sa celkova dlzka
		if (endTime <= 0)
			mediaSeeking->GetDuration(&endTime);

		CHECK_SUCCEED(hr = mediaControl->Run());

		this->m_continueEncode = TRUE;
		while (this->m_continueEncode)
		{
			if (mediaEvent->GetEvent(&evCode, &param1, &param2, 1000) == S_OK)
			{
				switch(evCode) 
				{ 
					case EC_COMPLETE:
					case EC_USERABORT:
						this->m_continueEncode = FALSE;
					case EC_ERRORABORT:
					case EC_ERRORABORTEX:
						hr = (HRESULT)param1;
						mediaEvent->FreeEventParams(evCode, param1, param2);
						goto done;
					default: break;
				}
				CHECK_HR(hr = mediaEvent->FreeEventParams(evCode, param1, param2));
			}

			//Kontrola, ci stream nie je uz zatvoreny
			if (!this->m_outputStream->CanWrite)
			{
				hr = E_FAIL;
				goto done;
			}

			//Zisti priblizne (hlavne pri variabile bitrate) spracovanie
			mediaSeeking->GetCurrentPosition(&position);
			progress = (System::Double)(position - startTime) / (System::Double)(endTime - startTime);
			progress = progress > 1.0 ? System::Double::PositiveInfinity : progress;
			OnProgressChange(gcnew ProgressChangeEventArgs(progress));
		}

		OnProgressChange(gcnew ProgressChangeEventArgs(1.0));

	done:

		if (mediaControl != NULL)
			mediaControl->Stop();

		SAFE_RELEASE(writerFilter);
		SAFE_RELEASE(graphBuilder);
		SAFE_RELEASE(mediaControl);
		SAFE_RELEASE(mediaEvent);
		SAFE_RELEASE(mediaSeeking);

		SAFE_RELEASE(writerPin);
		SAFE_RELEASE(subtitlePin);
		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);

		if(hr != S_OK)
			throw gcnew DSException(L"Unable to recode entire input", hr);
	}

	void DSEncoder::StopEncode(void)
	{
		this->m_continueEncode = FALSE;
	}

	//*************** Protected ***************\\

	void DSEncoder::Dispose(BOOL disposing)
	{
		pin_ptr<IBaseFilter*> demuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(demuxFilter);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IFilterGraph*> pFilterGraph = &this->m_filterGraph;
		SafeRelease<IFilterGraph>(pFilterGraph);
	}

	//*************** Private ***************\\

	void DSEncoder::OnProgressChange(ProgressChangeEventArgs ^ arg)
	{
		//Neporovnavat na nullptr - event je defaultne prazdny
		this->ProgressChange(this, arg);
	}

	HRESULT DSEncoder::GetSourcePins(IPin ** videoPin, IPin ** audioPin, IPin ** subtitlePin)
	{
		HRESULT hr = S_OK;
		DWORD pinIdx = 0;

		IPin * retPin = NULL;
		IEnumPins * enumPins = NULL;

		CHECK_HR(hr = this->m_demuxFilter->EnumPins(&enumPins));
		CHECK_HR(hr = enumPins->Reset());

		while (enumPins->Next(1, &retPin, NULL) == S_OK)
		{
			PinInfoItem ^ item = nullptr;

			for each (PinInfoItem ^ pii in this->m_sourcePins)
			{
				if (pii->Index == pinIdx)
				{
					item = pii;
					break;
				}
			}

			if (item != nullptr && item->IsSelected)
			{
				if (item->MediaType == PinMediaType::Video && (*videoPin) == NULL)
				{
					*videoPin = retPin;
					retPin = NULL;
				}
				else if (item->MediaType == PinMediaType::Audio && (*audioPin) == NULL)
				{
					*audioPin = retPin;
					retPin = NULL;
				}
				else if (item->MediaType == PinMediaType::Subtitle && (*subtitlePin) == NULL)
				{
					*subtitlePin = retPin;
					retPin = NULL;
				}
			}

			pinIdx++;
			SAFE_RELEASE(retPin);
		}

	done:

		SAFE_RELEASE(enumPins);
		SAFE_RELEASE(retPin);
		
		return hr;
	}

	HRESULT DSEncoder::ClearGraph(void)
	{
		HRESULT hr = S_OK;

		IEnumFilters * enumFil = NULL;
		IBaseFilter * retFil = NULL;

		using namespace std;
		list<IBaseFilter*> filters;

		CHECK_HR(hr = this->m_filterGraph->EnumFilters(&enumFil));
		CHECK_HR(hr = enumFil->Reset());

		while (enumFil->Next(1, &retFil, NULL) == S_OK)
		{
			if (retFil != this->m_sourceFilter && retFil != this->m_demuxFilter)
			{
				filters.push_back(retFil);
			}
			else
			{
				SAFE_RELEASE(retFil);
			}
		}

		for each (IBaseFilter * filter in filters)
		{
			this->m_filterGraph->RemoveFilter(filter);
			SAFE_RELEASE(filter);
		}

	done:

		SAFE_RELEASE(enumFil);
		SAFE_RELEASE(retFil);
		
		return hr;
	}

	HRESULT DSEncoder::FindDemultiplexor(IBaseFilter ** demultiplexor, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking)
	{
		HRESULT hr = S_OK;

		IMediaSeeking * mediaSeeking = NULL;
		IBaseFilter * demuxFilter = NULL;
		IPin * inputPin = NULL;

		IFilterMapper2 * filterMapper = NULL;
		IEnumMoniker * enumMoniker = NULL;
		IPropertyBag * propBag = NULL;
		IMoniker * moniker = NULL;

		GUID arrayInTypes[2];
		arrayInTypes[0] = MEDIATYPE_Stream;
		arrayInTypes[1] = GUID_NULL;

		CHECK_HR(hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC, IID_IFilterMapper2, (void **)&filterMapper));
		CHECK_HR(hr = filterMapper->EnumMatchingFilters(&enumMoniker, 0, TRUE, MERIT_DO_NOT_USE + 1, TRUE, 1, arrayInTypes, 
			NULL, NULL, FALSE, TRUE, 0, NULL, NULL, NULL));
		
		//Najdenie vhodneho demultiplexora
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			if (moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propBag) >= 0)
			{
				if (moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&demuxFilter) == S_OK)
				{
					if (graphBuilder->AddFilter(demuxFilter, NULL) >= 0)
					{
						inputPin = GetFirstPin(demuxFilter, PINDIR_INPUT);

						if (graphBuilder->ConnectDirect(outputPin, inputPin, NULL) >= 0)
						{
							if (reqSeeking)
							{
								if (demuxFilter->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking) >= 0)
								{
									SAFE_RELEASE(mediaSeeking);
									break;
								}
							}
							else
							{
								break;
							}
						}

						SAFE_RELEASE(inputPin);
						CHECK_HR(hr = graphBuilder->RemoveFilter(demuxFilter));
					}
					SAFE_RELEASE(demuxFilter);
				}
				SAFE_RELEASE(propBag);
			}
			SAFE_RELEASE(moniker);
		}

		if (demuxFilter != NULL)
		{
			*demultiplexor = demuxFilter;
			demuxFilter = NULL;
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}

	done:

		SAFE_RELEASE(filterMapper);
		SAFE_RELEASE(enumMoniker);
		SAFE_RELEASE(propBag);
		SAFE_RELEASE(moniker);

		SAFE_RELEASE(mediaSeeking);
		SAFE_RELEASE(demuxFilter);
		SAFE_RELEASE(inputPin);

		return hr;
	}

	//*************** Internal Static ***************\\

	BOOL DSEncoder::IsConnected(IPin * pin)
	{
		IPin * connPin = NULL;
		pin->ConnectedTo(&connPin);

		if (connPin == NULL)
		{
			return FALSE;
		}
		else
		{
			SAFE_RELEASE(connPin);
			return TRUE;
		}
	}

	IPin * DSEncoder::GetFirstPin(IBaseFilter * filter, PIN_DIRECTION direction)
	{
		return GetPin(filter, direction, 0);
	}

	IPin * DSEncoder::GetPin(IBaseFilter * filter, PIN_DIRECTION direction, DWORD index)
	{
		HRESULT hr = S_OK;
		DWORD actIndex = 0;

		IPin * retPin = NULL;
		IEnumPins * enumPins = NULL;

		CHECK_HR(hr = filter->EnumPins(&enumPins));
		CHECK_HR(hr = enumPins->Reset());

		while (enumPins->Next(1, &retPin, NULL) == S_OK)
		{
			PIN_DIRECTION pinDir;
			CHECK_HR(hr = retPin->QueryDirection(&pinDir));
			if (pinDir == direction)
			{
				if (actIndex == index)
					break;
				else
				{
					actIndex++;
					SAFE_RELEASE(retPin);
				}
			}
			else
				SAFE_RELEASE(retPin);
		}

	done:

		SAFE_RELEASE(enumPins);
		if (hr != S_OK)
			SAFE_RELEASE(retPin);
		
		return retPin;
	}

	System::Collections::Generic::List<PinInfoItem^>^ DSEncoder::GetPinInfo(IBaseFilter * filter, PIN_DIRECTION direction)
	{
		HRESULT hr = S_OK;
		DWORD pinIdx = 0;

		IPin * retPin = NULL;
		IPin * connPin = NULL;
		IEnumPins * enumPins = NULL;
		AM_MEDIA_TYPE * mediaType = NULL;
		IEnumMediaTypes * enumTypes = NULL;
		System::Collections::Generic::List<PinInfoItem^>^ pinInfo = gcnew System::Collections::Generic::List<PinInfoItem^>();

		if (filter == NULL)
			goto done;

		CHECK_HR(hr = filter->EnumPins(&enumPins));
		CHECK_HR(hr = enumPins->Reset());

		while (enumPins->Next(1, &retPin, NULL) == S_OK)
		{
			PIN_DIRECTION pinDir;
			CHECK_HR(hr = retPin->QueryDirection(&pinDir));

			if (pinDir == direction)
			{
				PinInfoItem ^ item = nullptr;

				retPin->ConnectedTo(&connPin);
				CHECK_HR(hr = retPin->EnumMediaTypes(&enumTypes));
				CHECK_HR(hr = enumTypes->Reset());

				while (true)
				{
					if (enumTypes->Next(1, &mediaType, NULL) != 0)
						break;

					if (mediaType->majortype == MEDIATYPE_Video)
					{
						item = gcnew PinInfoItem(pinIdx, connPin != NULL, PinMediaType::Video);
					}
					else if (mediaType->majortype == MEDIATYPE_Audio)
					{
						item = gcnew PinInfoItem(pinIdx, connPin != NULL, PinMediaType::Audio);
					}
					else if (mediaType->majortype == MEDIATYPE_Subtitle)
					{
						System::String ^ langName = System::String::Empty;
						if (mediaType->formattype == SubtitleInfo)
						{
							SUBTITLEINFO * info = (SUBTITLEINFO *)mediaType->pbFormat;
							langName = System::Runtime::InteropServices::Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(info->IsoLang));
						}

						item = gcnew PinSubtitleItem(pinIdx, connPin != NULL, langName);
					}

					DeleteMediaType(mediaType);
					mediaType = NULL;

					if (item != nullptr)
						break;
				}

				if (item == nullptr)
					pinInfo->Add(gcnew PinInfoItem(pinIdx, connPin != NULL, PinMediaType::Unknown));
				else
					pinInfo->Add(item);

				SAFE_RELEASE(enumTypes);
				SAFE_RELEASE(connPin);
			}

			pinIdx++;
			SAFE_RELEASE(retPin);
		}

	done:

		SAFE_RELEASE(enumTypes);
		SAFE_RELEASE(enumPins);
		SAFE_RELEASE(retPin);
		SAFE_RELEASE(connPin);
		DeleteMediaType(mediaType);
		
		return pinInfo;
	}
}