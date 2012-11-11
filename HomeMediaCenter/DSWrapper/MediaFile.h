#pragma once

#if !defined(MEDIAFILE_DSWRAPPER_INCLUDED)
#define MEDIAFILE_DSWRAPPER_INCLUDED

#include <ShlDisp.h>

namespace DSWrapper 
{
	using namespace System;
	using namespace System::Linq;
	using namespace System::Text::RegularExpressions; 

	public ref class MediaFile
	{
		private:	MediaFile(void);
					static String ^ GetValue(IO::FileInfo ^ file, Int32 iValue, String ^ defaultValue);
					static void PrintKeys(IO::FileInfo ^ file, Int32 maxKeys);

		public:		static Boolean GetVideoInfo(IO::FileInfo ^ file, [Runtime::InteropServices::Out] TimeSpan% duration, [Runtime::InteropServices::Out] String ^% resolution);
					static Boolean GetAudioDuration(IO::FileInfo ^ file, [Runtime::InteropServices::Out] TimeSpan% duration);
					static String ^ GetImageResolution(IO::FileInfo ^ file);
					static String ^ GetAudioArtist(IO::FileInfo ^ file);
					static String ^ GetAudioGenre(IO::FileInfo ^ file);
					static String ^ GetAudioAlbum(IO::FileInfo ^ file);
					
	};
}

#endif //MEDIAFILE_DSWRAPPER_INCLUDED
