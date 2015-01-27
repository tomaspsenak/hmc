#pragma once

#if !defined(INPUTTYPE_DSWRAPPER_INCLUDED)
#define INPUTTYPE_DSWRAPPER_INCLUDED

#include "SinkFilterGraph.h"

namespace DSWrapper 
{
	ref class InputDictionary;

	public ref class InputType abstract
	{
		public:		static InputType ^ Static(UINT32 fps, array<System::Byte> ^ bitmapData);
			
					static InputType ^ Desktop(UINT32 fps);

					static InputType ^ Webcam(System::String ^ videoName, System::String ^ audioName);

		internal:	InputType(void) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) = 0;
	};

	private ref class StaticInput : InputType
	{
		internal:	StaticInput(UINT32 fps, array<System::Byte> ^ bitmapData) : InputType(), m_fps(fps), m_bitmapData(bitmapData) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) override;

		private:	UINT32 m_fps;
					array<System::Byte> ^ m_bitmapData;
	};

	private ref class DesktopInput : InputType
	{
		internal:	DesktopInput(UINT32 fps) : InputType(), m_fps(fps) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) override;

		private:	UINT32 m_fps;
	};

	public ref class WebcamInput : InputType
	{
		public:		static System::Collections::Generic::List<System::String^> ^ GetVideoInputNames(void)
					{
						return GetInputDeviceNames(CLSID_VideoInputDeviceCategory);
					}

					static System::Collections::Generic::List<System::String^> ^ GetAudioInputNames(void)
					{
						return GetInputDeviceNames(CLSID_AudioInputDeviceCategory);
					}

		internal:	WebcamInput(System::String ^ videoName, System::String ^ audioName) : InputType(), m_videoName(videoName), m_audioName(audioName) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) override;

		private:	static System::Collections::Generic::List<System::String^> ^ GetInputDeviceNames(REFCLSID category);
					static IBaseFilter * GetInputDevice(System::String ^ name, REFCLSID category);

					System::String ^ m_videoName;
					System::String ^ m_audioName;
	};

	private ref class InputDictionary : public System::Collections::Generic::Dictionary<System::String ^, System::IntPtr> { };

	public class InputDictionaryHelper
	{
		public:		InputDictionaryHelper(void);
					~InputDictionaryHelper(void);

					InputDictionary ^ GetDictionary(void)
					{
						return this->m_dictionary;
					}

		private:	gcroot<InputDictionary ^> m_dictionary;
	};
}

#endif //INPUTTYPE_DSWRAPPER_INCLUDED

