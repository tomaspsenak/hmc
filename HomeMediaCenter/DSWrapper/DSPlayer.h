#pragma once

#if !defined(DSPLAYER_DSWRAPPER_INCLUDED)
#define DSPLAYER_DSWRAPPER_INCLUDED

namespace DSWrapper 
{
	public ref class DSPlayer
	{
		public:		static void PlayMPEG2_TS(System::IO::Stream ^ stream, System::Boolean video, System::Boolean audio);

		private:	DSPlayer(void);
	};
}

#endif //DSPLAYER_DSWRAPPER_INCLUDED
