#include "stdafx.h"
#include "DSWrapper.h"
#include "DSException.h"
#include <InitGuid.h>
#include <dvdmedia.h>

//Struktura pre titulky integrovane v kontajneri
struct SUBTITLEINFO 
{
	DWORD dwOffset; // size of the structure/pointer to codec init data
	CHAR IsoLang[4]; // three letter lang code + terminating zero
	WCHAR TrackName[256];
};
DEFINE_GUID(MEDIATYPE_Subtitle, 0xE487EB08, 0x6B26, 0x4be9, 0x9D, 0xD3, 0x99, 0x34, 0x34, 0xD3, 0x13, 0xFD);
DEFINE_GUID(SubtitleInfo, 0xA33D2F7D, 0x96BC, 0x4337, 0xB2, 0x3B, 0xA8, 0xB9, 0xFB, 0xC2, 0x95, 0xE9);
DEFINE_GUID(CLSID_LAVSource, 0xB98D13E7, 0x55DB, 0x4385, 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38);

namespace DSWrapper 
{
	DSEncoder::DSEncoder(void) : m_graphBuilder(NULL), m_outputStream(nullptr), m_sourceFilter(NULL), m_demuxFilter(NULL), m_containerType(nullptr),
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
		SetInput(filePath, reqSeeking, nullptr);
	}

	void DSEncoder::SetInput(System::String ^ filePath, System::Boolean reqSeeking, System::Collections::Generic::IEnumerable<System::Guid>^ preferedDemultiplexor)
	{
		HRESULT hr = S_OK;

		System::Uri^ uri = nullptr;
		System::String^ extension = nullptr;

		IBaseFilter * sourceFilter = NULL;
		IGraphBuilder * graphBuilder = NULL;
		IFileSourceFilter * fileSource = NULL;

		System::Collections::Generic::List<PinInfoItem^>^ sourcePins = nullptr;
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
		else if (extension == "iso" || !uri->IsFile)
		{
			//Zdroj je ISO subor alebo URL adresa
			pin_ptr<const wchar_t> pFilePath = uri->IsFile ? PtrToStringChars(uri->LocalPath) : PtrToStringChars(uri->AbsoluteUri);

			CHECK_HR(hr = CoCreateInstance(CLSID_LAVSource, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&demuxFilter));
			CHECK_HR(hr = demuxFilter->QueryInterface(IID_IFileSourceFilter, (void **)&fileSource));
			CHECK_HR(hr = fileSource->Load(pFilePath, NULL));
		}
		else
		{
			//Zdroj je suborovy system
			pin_ptr<const wchar_t> pFilePath = PtrToStringChars(uri->LocalPath);

			CHECK_HR(hr = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&sourceFilter));
			CHECK_HR(hr = sourceFilter->QueryInterface(IID_IFileSourceFilter, (void **)&fileSource));
			CHECK_HR(hr = fileSource->Load(pFilePath, NULL));
		}

		//Vytvorenie manazera grafu
		CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder));

		if (demuxFilter == NULL)
		{
			BOOL hasAV = FALSE;

			//Ak nebol priradeny demultiplexor
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(sourceFilter, NULL));
			outputPin = GetFirstPin(sourceFilter, PINDIR_OUTPUT);

			while (!hasAV)
			{
				//Najprv sa pokusi najst demultiplexor s podporou seekovania
				if (reqSeeking)
				{
					if (FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, TRUE, preferedDemultiplexor) != S_OK)
						CHECK_HR(hr = FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, FALSE, preferedDemultiplexor));
				}
				else
				{
					CHECK_HR(hr = FindDemultiplexor(&demuxFilter, graphBuilder, outputPin, FALSE, preferedDemultiplexor));
				}

				sourcePins = GetPinsInfo(demuxFilter, PINDIR_OUTPUT);
				for each (PinInfoItem^ item in sourcePins)
				{
					if (item->MediaType != PinMediaType::Unknown)
					{
						hasAV = TRUE;
						break;
					}
				}

				if (!hasAV)
				{
					SAFE_RELEASE(outputPin);
					outputPin = GetFirstPin(demuxFilter, PINDIR_OUTPUT);
					SAFE_RELEASE(demuxFilter);
				}
			}
		}
		else
		{
			CHECK_SUCCEED(hr = graphBuilder->AddFilter(demuxFilter, NULL));
			sourcePins = GetPinsInfo(demuxFilter, PINDIR_OUTPUT);
		}

		this->m_sourcePins = sourcePins;

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IGraphBuilder*> pGraphBuilder = &this->m_graphBuilder;
		SafeRelease<IGraphBuilder>(pGraphBuilder);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IBaseFilter*> pDemuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(pDemuxFilter);

		this->m_graphBuilder = graphBuilder;
		graphBuilder = NULL;
		this->m_sourceFilter = sourceFilter;
		sourceFilter = NULL;
		this->m_demuxFilter = demuxFilter;
		demuxFilter = NULL;

	done:

		SAFE_RELEASE(sourceFilter);
		SAFE_RELEASE(fileSource);
		SAFE_RELEASE(graphBuilder);

		SAFE_RELEASE(demuxFilter);
		SAFE_RELEASE(outputPin);

		if(hr != S_OK)
			throw gcnew DSException(L"Can not read from a file", hr);
	}

	void DSEncoder::SetInput(InputType ^ inputType)
	{
		HRESULT hr = S_OK;

		IGraphBuilder * graphBuilder = NULL;
		IBaseFilter * sourceFilter = NULL;
		IAMGraphStreams * graphStreams = NULL;

		if (inputType == nullptr)
			CHECK_HR(hr = E_INVALIDARG);

		//Vytvorenie zakladnych objektov
		CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder));
		CHECK_HR(hr = graphBuilder->QueryInterface(IID_IAMGraphStreams, (void **)&graphStreams));
		//Nastavenie synchronizacie pre live sources
		graphStreams->SyncUsingStreamOffset(TRUE);

		CHECK_HR(hr = inputType->GetInputFilter(&sourceFilter));
		
		CHECK_SUCCEED(hr = graphBuilder->AddFilter(sourceFilter, NULL));

		this->m_sourcePins = GetPinsInfo(sourceFilter, PINDIR_OUTPUT);

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IGraphBuilder*> pGraphBuilder = &this->m_graphBuilder;
		SafeRelease<IGraphBuilder>(pGraphBuilder);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IBaseFilter*> pDemuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(pDemuxFilter);

		this->m_graphBuilder = graphBuilder;
		graphBuilder = NULL;
		this->m_demuxFilter = sourceFilter;
		sourceFilter = NULL;

	done:

		SAFE_RELEASE(graphBuilder);
		SAFE_RELEASE(sourceFilter);
		SAFE_RELEASE(graphStreams);

		if(hr != S_OK)
			throw gcnew DSException(L"Unable to initialize input", hr);
	}

	void DSEncoder::SetOutput(ContainerType ^ containerType)
	{
		SetOutput(nullptr, containerType, 0, 0);
	}

	void DSEncoder::SetOutput(ContainerType ^ containerType, System::Int64 startTime, System::Int64 endTime)
	{
		SetOutput(nullptr, containerType, startTime, endTime);
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

		//Vstupm su milisekundy
		this->m_startTime = startTime * 10000;
		this->m_endTime = endTime * 10000;

		this->m_outputStream = outputStream;

		this->m_containerType = containerType;

	done: 

		if(hr != S_OK)
			throw gcnew DSException(L"Unable to create output", hr);
	}

	void DSEncoder::StartEncode(void)
	{
		StartEncode(0);
	}

	void DSEncoder::StartEncode(DWORD msTimeout)
	{
		HRESULT hr = S_OK;
		DWORD phase = 0;

		//Obmedzenie maximalneho casu enkodovania - ak je msTimeout nenulove
		DWORD encStartTick = GetTickCount();
		DWORD getEventTimeout = (msTimeout) ? min(msTimeout, 1000) : 1000;

		IMediaControl * mediaControl = NULL;
		IMediaEvent * mediaEvent = NULL;
		IMediaSeeking * mediaSeeking = NULL;
		IMediaSeeking * mediaSeekingMux = NULL;
		IBaseFilter * writerFilter = NULL;
		IAMStreamSelect * streamSelect = NULL;

		IPin * videoPin = NULL;
		IPin * audioPin = NULL;
		IPin * subtitlePin = NULL;
		IPin * writerPin = NULL;

		CHECK_HR(hr = (this->m_containerType == nullptr) ? E_FAIL : S_OK);
		CHECK_HR(hr = (this->m_graphBuilder == NULL) ? E_FAIL : S_OK);
		CHECK_HR(hr = (this->m_demuxFilter == NULL) ? E_FAIL : S_OK);

		CHECK_HR(hr = this->m_graphBuilder->QueryInterface(IID_IMediaControl, (void **)&mediaControl));
		CHECK_HR(hr = this->m_graphBuilder->QueryInterface(IID_IMediaEvent, (void **)&mediaEvent));
		CHECK_HR(hr = this->m_graphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking));		
		
		//Vycisti graf a zisti audio a video pin podla nastavenia a demultiplexora
		CHECK_HR(hr = ClearGraphFrom(this->m_graphBuilder, this->m_demuxFilter));
		CHECK_HR(hr = GetSourcePins(&videoPin, &audioPin, &subtitlePin));

		//Ziska writerFilter podla kontajnera, prida ho do grafu a zisti jeho vstupny pin
		CHECK_HR(hr = this->m_containerType->GetWriter(this->m_outputStream, this->m_graphBuilder, &writerFilter));
		writerPin = GetFirstPin(writerFilter, PINDIR_INPUT);

		phase++;
		//Nastavi spojenie decoder -> coder -> muxer -> writer
		CHECK_HR(hr = this->m_containerType->ConfigureContainer(this->m_graphBuilder, videoPin, audioPin, subtitlePin, writerPin, &mediaSeekingMux));
		if (mediaSeekingMux == NULL)
		{
			mediaSeekingMux = mediaSeeking;
			mediaSeekingMux->AddRef();
		}

		//Ak demultiplexor podporuje IAMStreamSelect, aktivuju sa streami
		//Pri vacsine demultiplexeroch funguje az po pripojeni pinov
		if (this->m_demuxFilter->QueryInterface(IID_IAMStreamSelect, (void **)&streamSelect) == S_OK)
		{
			for each (PinInfoItem ^ pii in this->m_sourcePins)
			{
				if (pii->IsStream && pii->IsSelected)
				{
					//AMSTREAMSELECTENABLE_ENABLE - Enable only this stream within the given group and disable all others.
					streamSelect->Enable(pii->Index, AMSTREAMSELECTENABLE_ENABLE);
				}
			}
		}

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

		phase++;
		CHECK_SUCCEED(hr = mediaControl->Run());
		phase++;

		this->m_continueEncode = TRUE;
		while (this->m_continueEncode)
		{
			if (mediaEvent->GetEvent(&evCode, &param1, &param2, getEventTimeout) == S_OK)
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
			else
			{
				//GetEvent vracia E_ABORT - vyprsal timeout
			}

			if (msTimeout && (GetTickCount() - encStartTick) >= msTimeout)
			{
				//Ak je msTimeout nastaveny a vyprsal cas na ukoncenie operacie
				hr = RPC_E_TIMEOUT;
				goto done;
			}

			//Kontrola, ci stream nie je uz zatvoreny
			if (this->m_outputStream != nullptr && !this->m_outputStream->CanWrite)
			{
				hr = E_FAIL;
				goto done;
			}

			//Zisti priblizne (hlavne pri variabile bitrate) spracovanie
			mediaSeekingMux->GetCurrentPosition(&position);
			progress = (System::Double)(position - startTime) / (System::Double)(endTime - startTime);
			progress = progress > 1.0 ? System::Double::PositiveInfinity : progress;
			OnProgressChange(gcnew ProgressChangeEventArgs(progress));
		}

		OnProgressChange(gcnew ProgressChangeEventArgs(1.0));

	done:

		if (mediaControl != NULL)
			mediaControl->Stop();

		SAFE_RELEASE(writerFilter);
		SAFE_RELEASE(mediaControl);
		SAFE_RELEASE(mediaEvent);
		SAFE_RELEASE(mediaSeeking);
		SAFE_RELEASE(mediaSeekingMux);
		SAFE_RELEASE(streamSelect);

		SAFE_RELEASE(writerPin);
		SAFE_RELEASE(subtitlePin);
		SAFE_RELEASE(videoPin);
		SAFE_RELEASE(audioPin);

		if (hr != S_OK)
		{
			if (hr == VFW_E_CANNOT_CONNECT)
				throw gcnew DSException(L"No combination of intermediate filters could be found to make the connection", hr);
			if (hr == RPC_E_TIMEOUT)
				throw gcnew DSException(L"This operation returned because the time-out period expired", hr);
			if (phase == 0)
				throw gcnew DSException(L"Exception while initializing filter graph", hr);
			if (phase == 1)
				throw gcnew DSException(L"Exception while configuring filter graph", hr);
			if (phase == 2)
				throw gcnew DSException(L"Failed to run filter graph", hr);
			throw gcnew DSException(L"Unable to recode entire input", hr);
		}
	}

	void DSEncoder::StopEncode(void)
	{
		this->m_continueEncode = FALSE;
	}

	System::Int64 DSEncoder::GetDuration(void)
	{
		HRESULT hr = S_OK;
		LONGLONG val = 0;

		IPin * retPin = NULL;
		IEnumPins * enumPins = NULL;
		IMediaSeeking * mediaSeeking = NULL;

		if (this->m_demuxFilter == NULL)
			CHECK_HR(hr = E_POINTER);

		if (this->m_demuxFilter->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking) < 0)
		{
			CHECK_HR(hr = this->m_graphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking));

			CHECK_HR(hr = this->m_demuxFilter->EnumPins(&enumPins));
			CHECK_HR(hr = enumPins->Reset());

			while (enumPins->Next(1, &retPin, NULL) == S_OK)
			{
				PIN_DIRECTION pinDir;
				CHECK_HR(hr = retPin->QueryDirection(&pinDir));

				if (pinDir == PINDIR_OUTPUT)
				{
					if (this->m_graphBuilder->Render(retPin) >= 0)
						break;
				}

				SAFE_RELEASE(retPin);
			}
		}

		CHECK_HR(hr = mediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME));
		CHECK_HR(hr = mediaSeeking->GetDuration(&val));

		//Premena jednotiek na milisekundy so zaokruhlenim
		val = (val + 5555) / 10000;

	done:

		SAFE_RELEASE(retPin);
		SAFE_RELEASE(enumPins);
		SAFE_RELEASE(mediaSeeking);

		return val;
	}

	//*************** Protected ***************\\

	void DSEncoder::Dispose(BOOL disposing)
	{
		pin_ptr<IBaseFilter*> demuxFilter = &this->m_demuxFilter;
		SafeRelease<IBaseFilter>(demuxFilter);
		pin_ptr<IBaseFilter*> pSourceFilter = &this->m_sourceFilter;
		SafeRelease<IBaseFilter>(pSourceFilter);
		pin_ptr<IGraphBuilder*> pGraphBuilder = &this->m_graphBuilder;
		SafeRelease<IGraphBuilder>(pGraphBuilder);
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
				//Viacej PinInfoItem moze mat rovnaky PinIndex - obsahuju streami
				//Prehladava piny s rovnakym indexom a najde prvy ktory je selected
				if (pii->PinIndex == pinIdx && pii->IsSelected)
				{
					item = pii;
					break;
				}
			}

			if (item != nullptr)
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

	HRESULT DSEncoder::FindDemultiplexor(IBaseFilter ** demultiplexor, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking,
		System::Collections::Generic::IEnumerable<System::Guid>^ preferedDemultiplexor)
	{
		CheckPointer(outputPin, E_POINTER);
		HRESULT hr = S_OK;

		IBaseFilter * demuxFilter = NULL;

		IFilterMapper2 * filterMapper = NULL;
		IEnumMoniker * enumMoniker = NULL;
		IPropertyBag * propBag = NULL;
		IMoniker * moniker = NULL;

		DWORD arrayLength = 20;
		GUID arrayInTypes[20 * 2]; //velkost musi byt arrayLength * 2
		arrayLength = GetTypesArray(outputPin, arrayInTypes, arrayLength);

		if (preferedDemultiplexor != nullptr)
		{
			//Ako prve sa pokusi pripojit preferovane demultiplexori
			for each (System::Guid guid in preferedDemultiplexor)
			{
				array<System::Byte>^ guidData = guid.ToByteArray();
				pin_ptr<System::Byte> data = &(guidData[0]);
				IID iid = *((IID *)data);

				if (CoCreateInstance(iid, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&demuxFilter) == S_OK)
				{
					if (ConnectDemultiplexor(demuxFilter, graphBuilder, outputPin, reqSeeking) == S_OK)
						break;

					SAFE_RELEASE(demuxFilter);
				}
			}
		}

		if (demuxFilter == NULL)
		{
			//Preferovany demultiplexor sa nepodarilo pouzit
			CHECK_HR(hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC, IID_IFilterMapper2, (void **)&filterMapper));
			//nezvysovat merit - problem s LAV filtrom
			CHECK_HR(hr = filterMapper->EnumMatchingFilters(&enumMoniker, 0, TRUE, MERIT_DO_NOT_USE, TRUE, arrayLength, arrayInTypes, 
				NULL, NULL, FALSE, TRUE, 0, NULL, NULL, NULL));
		
			//Najdenie vhodneho demultiplexora
			while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
			{
				if (moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propBag) >= 0)
				{
					if (moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&demuxFilter) == S_OK)
					{
						if (ConnectDemultiplexor(demuxFilter, graphBuilder, outputPin, reqSeeking) == S_OK)
							break;

						SAFE_RELEASE(demuxFilter);
					}
					SAFE_RELEASE(propBag);
				}
				SAFE_RELEASE(moniker);
			}
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

		SAFE_RELEASE(demuxFilter);

		return hr;
	}

	HRESULT DSEncoder::ConnectDemultiplexor(IBaseFilter * demuxFilter, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking)
	{
		HRESULT hr = E_FAIL;
		IPin * inputPin = NULL;
		IMediaSeeking * mediaSeeking = NULL;

		if (graphBuilder->AddFilter(demuxFilter, NULL) >= 0)
		{
			inputPin = GetFirstPin(demuxFilter, PINDIR_INPUT);

			if (graphBuilder->ConnectDirect(outputPin, inputPin, NULL) >= 0)
			{
				if (reqSeeking)
				{
					if (demuxFilter->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking) >= 0)
					{
						hr = S_OK;
						goto done;
					}
				}
				else
				{
					hr = S_OK;
					goto done;
				}
			}

			graphBuilder->RemoveFilter(demuxFilter);
		}

	done:

		SAFE_RELEASE(mediaSeeking);
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

	System::Collections::Generic::List<PinInfoItem^>^ DSEncoder::GetPinsInfo(IBaseFilter * filter, PIN_DIRECTION direction)
	{
		HRESULT hr = S_OK;
		DWORD pinIdx = 0, streamCount = 0, streamFlags = 0, audioPinCount = 0, videoPinCount = 0, subtitlePinCount = 0;

		IPin * retPin = NULL;
		IPin * connPin = NULL;
		IEnumPins * enumPins = NULL;
		AM_MEDIA_TYPE * mediaType = NULL;
		IEnumMediaTypes * enumTypes = NULL;
		IAMStreamSelect * streamSelect = NULL;

		PinInfoItem ^ audioPin = nullptr, ^ videoPin = nullptr, ^ subtitlePin = nullptr;
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

					item = GetPinInfo(pinIdx, connPin != NULL, mediaType, nullptr);

					DeleteMediaType(mediaType);
					mediaType = NULL;

					if (item->MediaType == PinMediaType::Video)
					{
						videoPinCount++;
						videoPin = item;
						break;
					}
					else if (item->MediaType == PinMediaType::Audio)
					{
						audioPinCount++;
						audioPin = item;
						break;
					}
					else if (item->MediaType == PinMediaType::Subtitle)
					{
						subtitlePinCount++;
						subtitlePin = item;
						break;
					}
				}

				if (item != nullptr)
					pinInfo->Add(item);

				SAFE_RELEASE(enumTypes);
				SAFE_RELEASE(connPin);
			}

			pinIdx++;
			SAFE_RELEASE(retPin);
		}

		//IAMStreamSelect umoznuje vyber streamu. Jeden pin moze mat viacej streamov (audio,...)
		//Vyberaju sa iba tie piny, ktorych typy sa uz neopakuju - daju sa sparovat so streamami
		if ((audioPinCount == 1 || videoPinCount == 1 || subtitlePinCount == 1) &&
			(filter->QueryInterface(IID_IAMStreamSelect, (void **)&streamSelect) == S_OK))
		{
			if (streamSelect->Count(&streamCount) != S_OK)
				streamCount = 0;

			for (DWORD i = 0; i < streamCount; i++)
			{
				if (streamSelect->Info(i, &mediaType, &streamFlags, NULL, NULL, NULL, NULL, NULL) != S_OK)
					break;

				if (mediaType->majortype == MEDIATYPE_Video)
				{
					if (videoPinCount == 1)
					{
						pinInfo->Remove(videoPin);
						pinInfo->Add(GetPinInfo(i, streamFlags != 0, mediaType, videoPin));
					}
				}
				else if (mediaType->majortype == MEDIATYPE_Audio)
				{
					if (audioPinCount == 1)
					{
						pinInfo->Remove(audioPin);
						pinInfo->Add(GetPinInfo(i, streamFlags != 0, mediaType, audioPin));
					}
				}
				else if (mediaType->majortype == MEDIATYPE_Subtitle)
				{
					if (subtitlePinCount == 1)
					{
						pinInfo->Remove(subtitlePin);
						pinInfo->Add(GetPinInfo(i, streamFlags != 0, mediaType, subtitlePin));
					}
				}

				DeleteMediaType(mediaType);
				mediaType = NULL;
			}
		}

	done:

		SAFE_RELEASE(enumTypes);
		SAFE_RELEASE(enumPins);
		SAFE_RELEASE(retPin);
		SAFE_RELEASE(connPin);
		SAFE_RELEASE(streamSelect);
		DeleteMediaType(mediaType);
		
		return pinInfo;
	}

	PinInfoItem^ DSEncoder::GetPinInfo(DWORD index, BOOL selected, AM_MEDIA_TYPE * mediaType, PinInfoItem ^ streamPin)
	{
		if (mediaType->majortype == MEDIATYPE_Video)
		{
			BITMAPINFOHEADER bitmap;
			ZeroMemory(&bitmap, sizeof(BITMAPINFOHEADER));

			if (mediaType->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER * header = (VIDEOINFOHEADER *)mediaType->pbFormat;
				bitmap = header->bmiHeader;
			}
			else if (mediaType->formattype == FORMAT_VideoInfo2)
			{
				VIDEOINFOHEADER2 * header = (VIDEOINFOHEADER2 *)mediaType->pbFormat;
				bitmap = header->bmiHeader;
			}
			else if (mediaType->formattype == FORMAT_MPEG2_VIDEO)
			{
				MPEG2VIDEOINFO * header = (MPEG2VIDEOINFO *)mediaType->pbFormat;
				bitmap = header->hdr.bmiHeader;
			}

			return gcnew PinVideoItem(index, selected, streamPin, bitmap.biWidth, bitmap.biHeight);
		}
		else if (mediaType->majortype == MEDIATYPE_Audio)
		{
			return gcnew PinInfoItem(index, selected, PinMediaType::Audio, streamPin);
		}
		else if (mediaType->majortype == MEDIATYPE_Subtitle)
		{
			System::String ^ langName = System::String::Empty;
			if (mediaType->formattype == SubtitleInfo)
			{
				SUBTITLEINFO * info = (SUBTITLEINFO *)mediaType->pbFormat;
				langName = System::Runtime::InteropServices::Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(info->IsoLang));
			}

			return gcnew PinSubtitleItem(index, selected, streamPin, langName);
		}

		return gcnew PinInfoItem(index, selected, PinMediaType::Unknown, streamPin);
	}

	DWORD DSEncoder::GetTypesArray(IPin * pin, GUID * typesArray, DWORD maxLength)
	{
		HRESULT hr = S_OK;
		DWORD actIndex = 0;

		AM_MEDIA_TYPE * mediaType = NULL;
		IEnumMediaTypes * enumTypes = NULL;

		CHECK_HR(hr = pin->EnumMediaTypes(&enumTypes));
		CHECK_HR(hr = enumTypes->Reset());

		while (enumTypes->Next(1, &mediaType, NULL) == S_OK)
		{
			typesArray[actIndex * 2] = mediaType->majortype;
			typesArray[(actIndex * 2 ) + 1] = mediaType->subtype;

			if (++actIndex >= maxLength)
				break;

			DeleteMediaType(mediaType);
			mediaType = NULL;
		}

	done:

		SAFE_RELEASE(enumTypes);
		DeleteMediaType(mediaType);

		return actIndex;
	}

	HRESULT DSEncoder::ClearGraphFrom(IFilterGraph * filterGraph, IBaseFilter * fromFilter)
	{
		return ClearGraphFrom(filterGraph, fromFilter, TRUE);
	}

	HRESULT DSEncoder::ClearGraphFrom(IFilterGraph * filterGraph, IBaseFilter * fromFilter, BOOL removeFilter)
	{
		HRESULT hr = S_OK;

		IPin * retPin = NULL;
		IPin * connPin = NULL;
		IEnumPins * enumPins = NULL;

		CHECK_HR(hr = fromFilter->EnumPins(&enumPins));
		CHECK_HR(hr = enumPins->Reset());

		while (enumPins->Next(1, &retPin, NULL) == S_OK)
		{
			PIN_DIRECTION pinDir;
			CHECK_HR(hr = retPin->QueryDirection(&pinDir));

			if (pinDir == PINDIR_OUTPUT)
			{
				retPin->ConnectedTo(&connPin);
				if (connPin != NULL)
				{
					PIN_INFO info;
					if (connPin->QueryPinInfo(&info) >= 0)
					{
						ClearGraphFrom(filterGraph, info.pFilter, TRUE);

						if (removeFilter)
							filterGraph->RemoveFilter(info.pFilter);

						SAFE_RELEASE(info.pFilter);
					}

					SAFE_RELEASE(connPin);
				}
			}

			SAFE_RELEASE(retPin);
		}

	done:

		SAFE_RELEASE(enumPins);
		SAFE_RELEASE(retPin);
		SAFE_RELEASE(connPin);
		
		return hr;
	}

	HRESULT DSEncoder::SaveGraphFile(IGraphBuilder * graph, WCHAR * wszPath)
	{
		const WCHAR wszStreamName[] = L"ActiveMovieGraph";
		IPersistStream * pPersist = NULL;
		IStorage * pStorage = NULL;
		IStream * pStream = NULL;
		HRESULT hr;
    
		CHECK_HR(hr = StgCreateDocfile(wszPath, STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage));

		CHECK_HR(hr = pStorage->CreateStream(wszStreamName, STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream));

		CHECK_HR(hr = graph->QueryInterface(IID_IPersistStream, (void**)&pPersist));

		CHECK_HR(hr = pPersist->Save(pStream, TRUE));
		
		CHECK_HR(hr = pStorage->Commit(STGC_DEFAULT));

	done:

		SAFE_RELEASE(pPersist);
		SAFE_RELEASE(pStorage);
		SAFE_RELEASE(pStream);

		return hr;
	}
}