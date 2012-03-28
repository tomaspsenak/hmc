#pragma once

#if !defined(MFENCODER_MFWRAPPER_INCLUDED)
#define MFENCODER_MFWRAPPER_INCLUDED

#include "Enums.h"
#include "StreamInfoItem.h"
#include "ProgressChangeEventArgs.h"

namespace MFWrapper 
{
	public ref class MFEncoder : public System::IDisposable
	{
		public:		MFEncoder(void);

					~MFEncoder(void);
					!MFEncoder(void);

					void SetInput(System::IO::Stream ^ inputStream);

					void SetInput(System::String ^ inputPath);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType outputContainer, VideoStream ^ videoFormat, 
						AudioStream ^ audioFormat);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType outputContainer, VideoStream ^ videoFormat, 
						AudioStream ^ audioFormat, System::Int64 startTime, System::Int64 endTime);

					void StartEncode(void);

					void StopEncode(void);

					property System::Collections::Generic::IEnumerable<StreamInfoItem^>^ SourceStreams
					{
						System::Collections::Generic::IEnumerable<StreamInfoItem^>^ get() { return m_sourceStreams; }
					}

					property System::Boolean HardwareTransform
					{
						System::Boolean get() { return m_hardwareTransform == TRUE; }
						void set(System::Boolean value) { m_hardwareTransform = value == TRUE; }
					}

					event System::EventHandler<ProgressChangeEventArgs^>^ ProgressChange;

		protected:	virtual void Dispose(BOOL disposing);
					
		private:	void SetInput(System::Object ^ inputObject);
					HRESULT InitSourceStreams(IMFSourceReader * sourceReader);
					HRESULT AddVideoSourceStream(DWORD index, BOOL selected, IMFMediaType * nativeMediaType);
					HRESULT AddAudioSourceStream(DWORD index, BOOL selected, IMFMediaType * nativeMediaType);
					HRESULT NegotiateStreamFormat(StreamInfoItem ^ item, REFGUID majorType, array<const GUID*>^ formats);
					HRESULT ProcessSamples(void);
					HRESULT ConfigureStreams(void);
					HRESULT ConfigureStartPosition(LONGLONG * adjustTimestamp);
					GUID GetOutputContainerGUID(ContainerType container);
					void OnProgressChange(ProgressChangeEventArgs ^ arg); 
					
					System::Collections::Generic::List<StreamInfoItem^>^ m_sourceStreams;
					VideoStream ^ m_videoFormat;
					AudioStream ^ m_audioFormat;
					BOOL m_isMFStarted;
					BOOL m_continueEncode;
					BOOL m_hardwareTransform;
					DWORD m_streamsCountSel;
					LONGLONG m_startTime;
					LONGLONG m_endTime;

					IMFSourceReader * m_sourceReader;
					IMFSinkWriter * m_sinkWriter;

					static array<const GUID*>^ m_negotiateVideoFormats = { &MFVideoFormat_NV12, &MFVideoFormat_YUY2, &MFVideoFormat_YV12, &MFVideoFormat_RGB32, &MFVideoFormat_RGB565, &MFVideoFormat_RGB555 };
					static array<const GUID*>^ m_negotiateAudioFormats = { &MFAudioFormat_Float, &MFAudioFormat_PCM };
	};
}

struct TimestampInfo
{
    ULONGLONG numSamples;
    LONGLONG lastTimestamp;
    BOOL done;
};

#endif //MFENCODER_MFWRAPPER_INCLUDED
