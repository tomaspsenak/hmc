#pragma once

#if !defined(MFEXCEPTION_MFWRAPPER_INCLUDED)
#define MFEXCEPTION_MFWRAPPER_INCLUDED

namespace MFWrapper 
{
	public ref class MFException : public System::Exception
	{
		public:		MFException(System::String ^ message, HRESULT hr) : Exception(message), m_hr(hr) { }
		
					property System::String ^ Message
					{
						virtual System::String ^ get(void) override 
						{ 
							return System::String::Format(L"{0} HRESULT=0x{1:x16}", System::Exception::Message, m_hr); 
						}
					}

					property HRESULT Result
					{
						HRESULT get(void)
						{ 
							return m_hr; 
						}
					}

		private:	HRESULT m_hr;
	};
}

#endif //MFEXCEPTION_MFWRAPPER_INCLUDED

