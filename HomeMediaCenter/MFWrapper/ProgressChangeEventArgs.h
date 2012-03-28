#pragma once

#if !defined(PROGRESSCHANGEEVENTARGS_MFWRAPPER_INCLUDED)
#define PROGRESSCHANGEEVENTARGS_MFWRAPPER_INCLUDED

namespace MFWrapper 
{
	public ref class ProgressChangeEventArgs : System::EventArgs
	{
		public:		ProgressChangeEventArgs(System::Double progress) : m_progress(progress) { }

					property System::Double Progress
					{
						System::Double get() { return m_progress; }
					}

		private:	System::Double m_progress;
	};
}

#endif //PROGRESSCHANGEEVENTARGS_DSWRAPPER_INCLUDED