#pragma once

#if !defined(PROGRESSCHANGEEVENTARGS_DSWRAPPER_INCLUDED)
#define PROGRESSCHANGEEVENTARGS_DSWRAPPER_INCLUDED

namespace DSWrapper 
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