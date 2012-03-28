#include "stdafx.h"
#include "MFEncoder.h"
#include "MFException.h"
#include "MFStream.h"
#include <propvarutil.h>
#include <intsafe.h>

namespace MFWrapper 
{
	MFEncoder::MFEncoder(void) : m_isMFStarted(FALSE), m_sourceReader(NULL), m_sinkWriter(NULL), m_continueEncode(FALSE), m_hardwareTransform(FALSE),
		m_sourceStreams(gcnew System::Collections::Generic::List<StreamInfoItem^>()), m_startTime(0), m_endTime(0), m_streamsCountSel(0)
	{
		HRESULT hr = S_OK;

		CHECK_HR(hr = MFStartup(MF_VERSION));
		this->m_isMFStarted = TRUE;

	done:
		if (!this->m_isMFStarted)
			throw gcnew MFException(L"Media Foundation initialization failed", hr);
	}

	MFEncoder::~MFEncoder(void)
	{
		Dispose(TRUE);
		System::GC::SuppressFinalize(this);
	}

	MFEncoder::!MFEncoder(void)
	{
		Dispose(FALSE);
	}

	void MFEncoder::SetInput(System::String ^ inputPath)
	{
		SetInput((System::Object^)inputPath);
	}

	void MFEncoder::SetInput(System::IO::Stream ^ inputStream)
	{
		SetInput((System::Object^)inputStream);
	}

	void MFEncoder::SetOutput(System::IO::Stream ^ outputStream, ContainerType outputContainer, VideoStream ^ videoFormat, AudioStream ^ audioFormat)
	{
		SetOutput(outputStream, outputContainer, videoFormat, audioFormat, 0, 0);
	}

	void MFEncoder::SetOutput(System::IO::Stream ^ outputStream, ContainerType outputContainer, VideoStream ^ videoFormat, 
		AudioStream ^ audioFormat, System::Int64 startTime, System::Int64 endTime)
	{
		HRESULT hr = S_OK;

		IMFReadWriteClassFactory * classFactory = NULL;
		IMFSinkWriter * sinkWriter = NULL;
		IMFAttributes * writerAttributes = NULL;
		MFStream * writerStream = NULL;

		if (startTime < 0 || endTime < 0 || (endTime != 0 && startTime > endTime))
			CHECK_HR(hr = E_INVALIDARG);

		this->m_startTime = startTime;
		this->m_endTime = endTime;

		CHECK_HR(hr = MFCreateAttributes(&writerAttributes, 2));
		CHECK_HR(hr = writerAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, this->m_hardwareTransform));
		CHECK_HR(hr = writerAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, GetOutputContainerGUID(outputContainer)));

		writerStream = new MFStream(&hr, outputStream);
		CHECK_HR(hr);

		CHECK_HR(hr = CoCreateInstance(CLSID_MFReadWriteClassFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&classFactory)));

		CHECK_HR(hr = classFactory->CreateInstanceFromObject(CLSID_MFSinkWriter, writerStream, writerAttributes, IID_PPV_ARGS(&sinkWriter)));

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IMFSinkWriter*> pSinkWriter = &this->m_sinkWriter;
		SafeRelease<IMFSinkWriter>(pSinkWriter);

		this->m_sinkWriter = sinkWriter;
		sinkWriter = NULL;

		this->m_videoFormat = videoFormat;
		this->m_audioFormat = audioFormat;

	done:

		SAFE_RELEASE(classFactory);
		SAFE_RELEASE(sinkWriter);
		SAFE_RELEASE(writerAttributes);
		SAFE_RELEASE(writerStream);

		if(hr != S_OK)
			throw gcnew MFException(L"Unable to create output", hr);
	}

	void MFEncoder::StartEncode(void)
	{
		if (!this->m_isMFStarted)
			throw gcnew MFException(L"Media Foundation is not initialized", 0);
		if (this->m_sourceReader == NULL)
			throw gcnew MFException(L"Unknown input", 0);
		if (this->m_sinkWriter == NULL)
			throw gcnew MFException(L"Unknown output", 0);

		HRESULT hr;

		hr = ConfigureStreams();
		if(hr != S_OK)
			throw gcnew MFException(L"Input file does not contain supported audio / video codec", hr);
		hr = ProcessSamples();
		if(hr != S_OK)
			throw gcnew MFException(L"Unable to recode entire input", hr);
	}

	void MFEncoder::StopEncode(void)
	{
		this->m_continueEncode = FALSE;
	}

	//*************** Protected ***************\\

	void MFEncoder::Dispose(BOOL disposing)
	{
		pin_ptr<IMFSourceReader*> sourceReader = &this->m_sourceReader;
		SafeRelease<IMFSourceReader>(sourceReader);
		pin_ptr<IMFSinkWriter*> sinkWriter = &this->m_sinkWriter;
		SafeRelease<IMFSinkWriter>(sinkWriter);

		if (this->m_isMFStarted)
		{
			MFShutdown();
			this->m_isMFStarted = FALSE;
		}
	}

	//**************** Private ****************\\

	void MFEncoder::SetInput(System::Object ^ inputObject)
	{
		HRESULT hr = S_OK;

		IMFReadWriteClassFactory * classFactory = NULL;
		IMFSourceReader * sourceReader = NULL;
		IMFAttributes * readerAttributes = NULL;
		MFStream * readerStream = NULL;

		CHECK_HR(hr = MFCreateAttributes(&readerAttributes, 1));
		CHECK_HR(hr = readerAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, this->m_hardwareTransform));

		CHECK_HR(hr = CoCreateInstance(CLSID_MFReadWriteClassFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&classFactory)));

		if (dynamic_cast<System::IO::Stream^>(inputObject) != nullptr)
		{
			readerStream = new MFStream(&hr, (System::IO::Stream^)inputObject);
			CHECK_HR(hr);

			CHECK_HR(hr = classFactory->CreateInstanceFromObject(CLSID_MFSourceReader, readerStream, readerAttributes, IID_PPV_ARGS(&sourceReader)));
		}
		else if (dynamic_cast<System::String^>(inputObject) != nullptr)
		{
			pin_ptr<const wchar_t> pFilePath = PtrToStringChars((System::String^)inputObject);

			CHECK_HR(hr = classFactory->CreateInstanceFromURL(CLSID_MFSourceReader, pFilePath, readerAttributes, IID_PPV_ARGS(&sourceReader)));
		}

		CHECK_HR(hr = InitSourceStreams(sourceReader));

		//Vycisti predchadzajuci nastaveny objekt, priradi novy
		pin_ptr<IMFSourceReader*> pSourceReader = &this->m_sourceReader;
		SafeRelease<IMFSourceReader>(pSourceReader);

		this->m_sourceReader = sourceReader;
		sourceReader = NULL;

	done:

		SAFE_RELEASE(classFactory);
		SAFE_RELEASE(sourceReader);
		SAFE_RELEASE(readerAttributes);
		SAFE_RELEASE(readerStream);

		if(hr != S_OK)
			throw gcnew MFException(L"Can not read from a file", hr);
	}

	GUID MFEncoder::GetOutputContainerGUID(ContainerType container)
	{
		switch (container)
		{
			case ContainerType::MF_ASF:	return MFTranscodeContainerType_ASF;
			case ContainerType::MF_MP4:	return MFTranscodeContainerType_MPEG4;
			case ContainerType::MF_3GP:	return MFTranscodeContainerType_3GP;
		}
		return MFTranscodeContainerType_MPEG4;
	}

	HRESULT MFEncoder::InitSourceStreams(IMFSourceReader * sourceReader)
	{
		HRESULT hr = S_OK;
		IMFMediaType * nativeMediaType = NULL;

		this->m_sourceStreams->Clear();

		for (DWORD i = 0; ; i++)
		{
			BOOL selected;
			GUID majorType;

			hr = sourceReader->GetStreamSelection(i, &selected);
			if(MF_E_INVALIDSTREAMNUMBER == hr)
			{
				hr = S_OK;
				break;
			}
			CHECK_HR(hr);

			CHECK_HR(hr = sourceReader->GetNativeMediaType(i, 0, &nativeMediaType));

			CHECK_HR(hr = nativeMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType));

			if (majorType == MFMediaType_Video)
			{
				CHECK_HR(hr = AddVideoSourceStream(i, selected, nativeMediaType));
			}
			else if (majorType == MFMediaType_Audio)
			{
				CHECK_HR(hr = AddAudioSourceStream(i, selected, nativeMediaType));
			}
			else
			{
				this->m_sourceStreams->Insert(i, gcnew StreamInfoItem(i, selected));
			}

			SAFE_RELEASE(nativeMediaType);
		}

	done:

		SAFE_RELEASE(nativeMediaType);

		return hr;
	}

	HRESULT MFEncoder::AddVideoSourceStream(DWORD index, BOOL selected, IMFMediaType * nativeMediaType)
	{
		HRESULT hr = S_OK;
		GUID subType;
		UINT32 width;
		UINT32 height;
		UINT32 numerator;
		UINT32 denominator;
		UINT32 aspectX;
		UINT32 aspectY;
		UINT32 interlaceMode;
		UINT32 bitrate;

		CHECK_HR(hr = nativeMediaType->GetGUID(MF_MT_SUBTYPE, &subType));

		CHECK_HR(hr = MFGetAttributeSize(nativeMediaType, MF_MT_FRAME_SIZE, &width, &height)); 

		CHECK_HR(hr = MFGetAttributeRatio(nativeMediaType, MF_MT_FRAME_RATE, &numerator, &denominator));

		if (MFGetAttributeRatio(nativeMediaType, MF_MT_PIXEL_ASPECT_RATIO, &aspectX, &aspectY) != S_OK)
			aspectX = aspectY = 1;

		interlaceMode = MFGetAttributeUINT32(nativeMediaType, MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);

		bitrate = MFGetAttributeUINT32(nativeMediaType, MF_MT_AVG_BITRATE, 0);

		this->m_sourceStreams->Insert(index, gcnew StreamInfoItem(index, selected, gcnew VideoStream(subType, width, height, numerator,
			denominator, aspectX, aspectY, interlaceMode, bitrate)));

	done: 
		return hr;
	}

	HRESULT MFEncoder::AddAudioSourceStream(DWORD index, BOOL selected, IMFMediaType * nativeMediaType)
	{
		HRESULT hr = S_OK;
		GUID subType;
		UINT32 numChannels;
		UINT32 sampleRate;
		UINT32 bitrate;

		CHECK_HR(hr = nativeMediaType->GetGUID(MF_MT_SUBTYPE, &subType));

		CHECK_HR(hr = nativeMediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &numChannels));

		CHECK_HR(hr = nativeMediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate));

		CHECK_HR(hr = nativeMediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &bitrate));

		this->m_sourceStreams->Insert(index, gcnew StreamInfoItem(index, selected, gcnew AudioStream(subType, numChannels, sampleRate, bitrate)));

	done:
		return hr;
	}

	HRESULT MFEncoder::ConfigureStreams(void)
	{
		HRESULT hr = S_OK;
		IMFMediaType * nativeMediaType = NULL;
		IMFMediaType * targetMediaType = NULL;

		this->m_streamsCountSel = 0;

		for (DWORD i = 0; i < (DWORD)this->m_sourceStreams->Count; i++)
		{
			StreamInfoItem ^ item = this->m_sourceStreams[i];
			DWORD outputIndex;

			CHECK_HR(hr = this->m_sourceReader->SetStreamSelection(item->Index, item->IsSelectedInternal));
			if (item->IsSelected)
				this->m_streamsCountSel++;

			if (!item->IsSelectedInternal)
				continue;

			CHECK_HR(hr = this->m_sourceReader->GetNativeMediaType(item->Index, 0, &nativeMediaType));
			
			if (item->MediaType == StreamMediaType::Video)
			{
				targetMediaType = this->m_videoFormat == nullptr ? NULL : this->m_videoFormat->CreateMediaType();
			}
			else if (item->MediaType == StreamMediaType::Audio)
			{
				targetMediaType = this->m_audioFormat == nullptr ? NULL : this->m_audioFormat->CreateMediaType();
			}

			CHECK_HR(hr = m_sinkWriter->AddStream(targetMediaType == NULL ? nativeMediaType : targetMediaType, &outputIndex));
			item->OutputIndex = outputIndex;

			if (item->MediaType == StreamMediaType::Video)
			{
				CHECK_HR(hr = NegotiateStreamFormat(item, MFMediaType_Video, m_negotiateVideoFormats));
			}
			else if (item->MediaType == StreamMediaType::Audio)
			{
				CHECK_HR(hr = NegotiateStreamFormat(item, MFMediaType_Audio, m_negotiateAudioFormats));
			}

			SAFE_RELEASE(nativeMediaType);
			SAFE_RELEASE(targetMediaType);
		}

	done:

		SAFE_RELEASE(nativeMediaType);
		SAFE_RELEASE(targetMediaType);

		return hr;
	}

	HRESULT MFEncoder::NegotiateStreamFormat(StreamInfoItem ^ item, REFGUID majorType, array<const GUID*>^ formats)
	{
		HRESULT hr = S_OK;
		BOOL configured = FALSE;
		IMFMediaType * partialMediaType = NULL;
		IMFMediaType * fullMediaType = NULL;

		CHECK_HR(hr = MFCreateMediaType(&partialMediaType));
		CHECK_HR(hr = partialMediaType->SetGUID(MF_MT_MAJOR_TYPE, majorType));

		for(System::Int32 i = 0; i < formats->Length; i++)
		{
			SAFE_RELEASE(fullMediaType);

			CHECK_HR(hr = partialMediaType->SetGUID(MF_MT_SUBTYPE, *formats[i]));

			hr = this->m_sourceReader->SetCurrentMediaType(item->Index, NULL, partialMediaType);
			if(hr != S_OK)
			{
				hr = S_OK;
				continue;
			}

			CHECK_HR(hr = this->m_sourceReader->GetCurrentMediaType(item->Index, &fullMediaType));

			hr = this->m_sinkWriter->SetInputMediaType(item->OutputIndex, fullMediaType, NULL);
			if(hr != S_OK)
			{
				hr = S_OK;
				continue;
			}

			configured = TRUE;
			break;
		}

		if(!configured)
			hr = MF_E_INVALIDMEDIATYPE;

	done:

		SAFE_RELEASE(partialMediaType);
		SAFE_RELEASE(fullMediaType);

		return hr;
	}

	HRESULT MFEncoder::ProcessSamples(void)
	{
		HRESULT hr = S_OK;
		DWORD streamIndex, flags, streamsCount, progressInt = 0;
		LONGLONG timestamp, adjustTimestamp = 0;
		System::Double progress = 0.0;
		ULONGLONG endTime = 0;

		PROPVARIANT vEndTime;
		IMFSample * sample = NULL;
		IMFMediaType * mediaType = NULL;
		BOOL * discontWrite = NULL;

		//Zisti dlzku suboru ak je to mozne
		PropVariantInit(&vEndTime);
		this->m_sourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &vEndTime);
		if (vEndTime.vt == VT_UI8)
		{
			//Dlzka je minimum z nastavenej (ak je nastavena) a zistenej dlzky
			if (this->m_endTime == 0)
				endTime = vEndTime.uhVal.QuadPart;
			else
				endTime = min(vEndTime.uhVal.QuadPart, (ULONGLONG)this->m_endTime);
		}
		else
		{
			//Ak sa nepodarilo zistit dlzku, prirad nastavenu
			endTime = this->m_endTime;
		}

		//Inicializacia pola pre zapisanie diskontinuity - pre kazdy stream
		discontWrite = new BOOL[this->m_sourceStreams->Count];
		if(NULL == discontWrite)
			CHECK_HR(hr = E_OUTOFMEMORY);
		ZeroMemory(discontWrite, sizeof(BOOL) * this->m_sourceStreams->Count);

		//Nastavenie zaciatku
		if (this->m_startTime > 0)
			CHECK_HR(hr = ConfigureStartPosition(&adjustTimestamp));

		//Zacatie zapisu
		CHECK_HR(hr = this->m_sinkWriter->BeginWriting());

		this->m_continueEncode = TRUE;
		streamsCount = this->m_streamsCountSel;
		//Pracuj pokial nie su ukoncene vsetky streami
		while (streamsCount > 0 && this->m_continueEncode)
		{
			CHECK_HR(hr = this->m_sourceReader->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, &streamIndex, &flags, &timestamp, &sample));
			CHECK_HR(hr = flags & MF_SOURCE_READERF_ERROR);

			StreamInfoItem ^ item = this->m_sourceStreams[streamIndex];

			if(flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
			{
				CHECK_HR(hr = this->m_sourceReader->GetCurrentMediaType(streamIndex, &mediaType));
				CHECK_HR(hr = this->m_sinkWriter->SetInputMediaType(item->OutputIndex, mediaType, NULL));
				SAFE_RELEASE(mediaType);
			}

			if(flags & MF_SOURCE_READERF_STREAMTICK)
			{
				CHECK_HR(hr = this->m_sinkWriter->SendStreamTick(item->OutputIndex, timestamp - adjustTimestamp));
			}

			if(sample != NULL && (this->m_startTime == 0 || timestamp >= adjustTimestamp))
			{
                if(!discontWrite[streamIndex])
                {
					//Zapisanie diskontinuity pre zaciatok kazdeho streamu
                    CHECK_HR(hr = sample->SetUINT32(MFSampleExtension_Discontinuity, TRUE));
					discontWrite[streamIndex] = TRUE;
                }
				CHECK_HR(hr = sample->SetSampleTime(timestamp - adjustTimestamp));
				CHECK_HR(hr = this->m_sinkWriter->WriteSample(item->OutputIndex, sample));
			}
			SAFE_RELEASE(sample);

			//Informacia o zmene - udalost
			if (progressInt == 100)
			{
				progress = (System::Double)(timestamp - this->m_startTime) / (System::Double)(endTime - this->m_startTime);
				progress = progress > 1.0 ? System::Double::PositiveInfinity : progress;
				OnProgressChange(gcnew ProgressChangeEventArgs(progress));
				progressInt = 0;
			}
			progressInt++;

			if(flags & MF_SOURCE_READERF_ENDOFSTREAM || (this->m_endTime > 0 && this->m_endTime <= timestamp))
			{
				CHECK_HR(hr = this->m_sinkWriter->NotifyEndOfSegment(item->OutputIndex));
				CHECK_HR(hr = this->m_sourceReader->SetStreamSelection(streamIndex, FALSE));
				streamsCount--;
			}
		}

		CHECK_HR(hr = this->m_sinkWriter->Finalize());

	done:

		if (discontWrite != NULL)
			SAFE_ARRAY_DELETE(discontWrite);
		SAFE_RELEASE(sample);
		SAFE_RELEASE(mediaType);
		PropVariantClear(&vEndTime);

		return hr;
	}

	HRESULT MFEncoder::ConfigureStartPosition(LONGLONG * adjustTimestamp)
	{
		HRESULT hr = S_OK;
		DWORD streamIndex, flags, streamsCount;
		LONGLONG timestamp, minTimestamp = LONGLONG_MAX;

		PROPVARIANT varPosition;
		IMFSample *sample = NULL;
		TimestampInfo * timestampInfo = NULL;

		//Nastavi pribliznu poziciu
		InitPropVariantFromInt64(this->m_startTime, &varPosition);
		CHECK_HR(hr = this->m_sourceReader->SetCurrentPosition(GUID_NULL, varPosition));

		timestampInfo = new TimestampInfo[this->m_sourceStreams->Count];
		if(NULL == timestampInfo)
			CHECK_HR(hr = E_OUTOFMEMORY);
		ZeroMemory(timestampInfo, sizeof(TimestampInfo) * this->m_sourceStreams->Count);

		streamsCount = this->m_streamsCountSel;
		while(streamsCount > 0)
		{
			SAFE_RELEASE(sample);

			CHECK_HR(hr = this->m_sourceReader->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, &streamIndex, &flags, &timestamp, &sample));
			if(timestampInfo[streamIndex].done)
				continue;

			if(sample != NULL)
			{
				if(timestampInfo[streamIndex].numSamples == 0)
				{
					//Ak bol prvy sample - nastavi timestamp
					timestampInfo[streamIndex].lastTimestamp = timestamp;
				}
				else
				{
					//Porovnava predchadzaju a akutalny timestamp pokial nenajde cas, ktory najviac sedi pozadovanemu casu
					//Porovnavanie sa vykonava pre kazdy stream
					LONGLONG previousDelta = _abs64(this->m_startTime - timestampInfo[streamIndex].lastTimestamp);
					LONGLONG currentDelta = _abs64(this->m_startTime - timestamp);

					if(currentDelta < previousDelta )
					{
						timestampInfo[streamIndex].lastTimestamp = timestamp;
					}
					else
					{
						timestampInfo[streamIndex].done = TRUE;
						streamsCount--;
					}
				}

				timestampInfo[streamIndex].numSamples++;
			}

			//Ak nastal koniec streamu
			if(( MF_SOURCE_READERF_ENDOFSTREAM & flags ) != 0)
			{
				timestampInfo[streamIndex].done = TRUE;
				streamsCount--;
			}
		}

		//Zobere sa najmensi timestamp zo vsetkych streamov
		for(DWORD i = 0; i < (DWORD)this->m_sourceStreams->Count; i++)
		{
			if(!timestampInfo[i].done)
				continue;

			if(timestampInfo[i].lastTimestamp < minTimestamp)
				minTimestamp = timestampInfo[i].lastTimestamp;
		}

		if(minTimestamp == LONGLONG_MAX)
			CHECK_HR(hr = E_FAIL);
		*adjustTimestamp = minTimestamp;
    
		//Znovu nastavi poziciu lebo sa mohla zmenit
		CHECK_HR(hr = this->m_sourceReader->SetCurrentPosition(GUID_NULL, varPosition));

	done:

		if (timestampInfo != NULL)
			SAFE_ARRAY_DELETE(timestampInfo);
		PropVariantClear(&varPosition);
		SAFE_RELEASE(sample);

		return hr;
	}

	void MFEncoder::OnProgressChange(ProgressChangeEventArgs ^ arg)
	{
		//Neporovnavat na nullptr - event je defaultne prazdny
		this->ProgressChange(this, arg);
	}

}