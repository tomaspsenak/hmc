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
					static HRESULT GetValue(IO::FileInfo ^ file, OLECHAR * propName, VARIANT * vValue);
					static String ^ GetValue(IO::FileInfo ^ file, Int32 iValue, String ^ defaultValue);
					static void PrintKeys(IO::FileInfo ^ file, Int32 maxKeys);

		public:		static HRESULT GetVideoInfo(IO::FileInfo ^ file, String ^ thumbnailPath,
						[Runtime::InteropServices::Out] TimeSpan% duration,
						[Runtime::InteropServices::Out] String ^% resolution,
						[Runtime::InteropServices::Out] Int32% audioStreamsCount,
						[Runtime::InteropServices::Out] System::Collections::Generic::List<String^> ^% subtitlesStreams);
					static HRESULT GetAudioDuration(IO::FileInfo ^ file,
						[Runtime::InteropServices::Out] TimeSpan% duration,
						[Runtime::InteropServices::Out] Int32% audioStreamsCount);
					static Boolean GetImageResolution(IO::FileInfo ^ file, [Runtime::InteropServices::Out] String ^% resolution);
					static String ^ GetAudioArtist(IO::FileInfo ^ file);
					static String ^ GetAudioGenre(IO::FileInfo ^ file);
					static String ^ GetAudioAlbum(IO::FileInfo ^ file);
					static Boolean GetAudioThumbnail(IO::FileInfo ^ file, String ^ thumbnailPath);
					
	};
}

#endif //MEDIAFILE_DSWRAPPER_INCLUDED
