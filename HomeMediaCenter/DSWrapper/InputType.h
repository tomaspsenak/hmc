#pragma once

#if !defined(INPUTTYPE_DSWRAPPER_INCLUDED)
#define INPUTTYPE_DSWRAPPER_INCLUDED

namespace DSWrapper 
{
	public ref class InputType abstract
	{
		public:		static InputType ^ Desktop(UINT32 fps);

					static InputType ^ Webcam(System::String ^ camName);

		internal:	InputType(void) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) = 0;
	};

	private ref class DesktopInput : InputType
	{
		internal:	DesktopInput(UINT32 fps) : InputType(), m_fps(fps) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) override;

		private:	UINT32 m_fps;
	};

	public ref class WebcamInput : InputType
	{
		public:		static System::Collections::Generic::List<System::String^> ^ GetWebcamNames(void);

		internal:	WebcamInput(System::String ^ camName) : InputType(), m_camName(camName) { }

					virtual HRESULT GetInputFilter(IBaseFilter ** inputFilter) override;

		private:	System::String ^ m_camName;
	};
}

#endif //INPUTTYPE_DSWRAPPER_INCLUDED

