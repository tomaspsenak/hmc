#pragma once

#if !defined(DSENCODER_DSWRAPPER_INCLUDED)
#define DSENCODER_DSWRAPPER_INCLUDED

#include "PinInfoItem.h"
#include "ContainerType.h"
#include "InputType.h"
#include "ProgressChangeEventArgs.h"

namespace DSWrapper 
{
	public ref class DSEncoder : public System::IDisposable
	{
		public:		DSEncoder(void);

					~DSEncoder(void);
					!DSEncoder(void);

					void SetInput(System::String ^ filePath);

					void SetInput(System::String ^ filePath, System::Boolean reqSeeking);

					void SetInput(InputType ^ inputType);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType, System::Int64 startTime, System::Int64 endTime);

					void StartEncode(void);

					void StopEncode(void);

					property System::Collections::Generic::IEnumerable<PinInfoItem^>^ SourcePins
					{
						System::Collections::Generic::IEnumerable<PinInfoItem^>^ get() { return m_sourcePins; }
					}

					event System::EventHandler<ProgressChangeEventArgs^>^ ProgressChange;

		protected:	virtual void Dispose(BOOL disposing);

		internal:	static IPin * GetFirstPin(IBaseFilter * filter, PIN_DIRECTION direction);
					static IPin * GetPin(IBaseFilter * filter, PIN_DIRECTION direction, DWORD index);
					static BOOL IsConnected(IPin * pin);
					static System::Collections::Generic::List<PinInfoItem^>^ GetPinInfo(IBaseFilter * filter, PIN_DIRECTION direction);

		private:	void OnProgressChange(ProgressChangeEventArgs ^ arg);
					HRESULT FindDemultiplexor(IBaseFilter ** demultiplexor, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking);
					HRESULT GetSourcePins(IPin ** videoPin, IPin ** audioPin, IPin ** subtitlePin);
					HRESULT ClearGraph(void);

					System::Collections::Generic::List<PinInfoItem^>^ m_sourcePins;
					ContainerType ^ m_containerType;
					BOOL m_continueEncode;
					LONGLONG m_startTime;
					LONGLONG m_endTime;

					IFilterGraph * m_filterGraph;
					IBaseFilter * m_sourceFilter;
					IBaseFilter * m_demuxFilter;
					System::IO::Stream ^ m_outputStream;
	};
}

#endif //DSENCODER_DSWRAPPER_INCLUDED