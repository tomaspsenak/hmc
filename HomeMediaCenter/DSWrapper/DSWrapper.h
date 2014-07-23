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

					void SetInput(System::String ^ filePath, System::Boolean reqSeeking, 
						System::Collections::Generic::IEnumerable<System::Guid>^ preferedDemultiplexor);

					void SetInput(InputType ^ inputType);

					void SetOutput(ContainerType ^ containerType);

					void SetOutput(ContainerType ^ containerType, System::Int64 startTime, System::Int64 endTime);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType);

					void SetOutput(System::IO::Stream ^ outputStream, ContainerType ^ containerType, System::Int64 startTime, System::Int64 endTime);

					void StartEncode(void);

					void StartEncode(DWORD msTimeout);

					void StopEncode(void);

					System::Int64 GetDuration(void);

					property System::Collections::Generic::IEnumerable<PinInfoItem^>^ SourcePins
					{
						System::Collections::Generic::IEnumerable<PinInfoItem^>^ get() { return m_sourcePins; }
					}

					event System::EventHandler<ProgressChangeEventArgs^>^ ProgressChange;

		protected:	virtual void Dispose(BOOL disposing);

		internal:	static IPin * GetFirstPin(IBaseFilter * filter, PIN_DIRECTION direction);
					static IPin * GetPin(IBaseFilter * filter, PIN_DIRECTION direction, DWORD index);
					static BOOL IsConnected(IPin * pin);
					static System::Collections::Generic::List<PinInfoItem^>^ GetPinsInfo(IBaseFilter * filter, PIN_DIRECTION direction);
					static PinInfoItem^ GetPinInfo(DWORD index, BOOL selected, AM_MEDIA_TYPE * mediaType, PinInfoItem ^ streamPin);
					static DWORD GetTypesArray(IPin * pin, GUID * typesArray, DWORD maxLength);
					static HRESULT ClearGraphFrom(IFilterGraph * filterGraph, IBaseFilter * fromFilter);
					static HRESULT ClearGraphFrom(IFilterGraph * filterGraph, IBaseFilter * fromFilter, BOOL removeFilter);
					static HRESULT SaveGraphFile(IGraphBuilder * graph, WCHAR * wszPath);

		private:	void OnProgressChange(ProgressChangeEventArgs ^ arg);
					static HRESULT FindDemultiplexor(IBaseFilter ** demultiplexor, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking,
						System::Collections::Generic::IEnumerable<System::Guid>^ preferedDemultiplexor);
					static HRESULT ConnectDemultiplexor(IBaseFilter * demuxFilter, IGraphBuilder * graphBuilder, IPin * outputPin, BOOL reqSeeking);
					HRESULT GetSourcePins(IPin ** videoPin, IPin ** audioPin, IPin ** subtitlePin);

					System::Collections::Generic::List<PinInfoItem^>^ m_sourcePins;
					ContainerType ^ m_containerType;
					BOOL m_continueEncode;
					LONGLONG m_startTime;
					LONGLONG m_endTime;

					IGraphBuilder * m_graphBuilder;
					IBaseFilter * m_sourceFilter;
					IBaseFilter * m_demuxFilter;
					System::IO::Stream ^ m_outputStream;
	};
}

#endif //DSENCODER_DSWRAPPER_INCLUDED