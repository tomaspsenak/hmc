#pragma once

#if !defined(DSPLAYER_DSWRAPPER_INCLUDED)
#define DSPLAYER_DSWRAPPER_INCLUDED

namespace DSWrapper 
{
	public ref class DSPlayer : public System::IDisposable
	{
		public:		static DSPlayer ^ PlayMPEG2_TS(System::IO::Stream ^ stream, System::Boolean video, System::Boolean audio);

					~DSPlayer(void);
					!DSPlayer(void);

		protected:	virtual void Dispose(BOOL disposing);

		private:	DSPlayer(IMediaControl * mediaControl, System::IO::Stream ^ stream);
					IMediaControl * m_mediaControl;
					System::IO::Stream ^ m_stream;
	};
}

#endif //DSPLAYER_DSWRAPPER_INCLUDED
