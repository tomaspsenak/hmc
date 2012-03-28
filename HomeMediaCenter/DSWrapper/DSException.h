#pragma once

#if !defined(DSEXCEPTION_DSWRAPPER_INCLUDED)
#define DSEXCEPTION_DSWRAPPER_INCLUDED

namespace DSWrapper 
{
	public ref class DSException : public System::Exception
	{
		public:		DSException(System::String ^ message, HRESULT hr) : Exception(message), m_hr(hr) { }

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

#endif //DSEXCEPTION_DSWRAPPER_INCLUDED

