#pragma once

#if !defined(ENUMS_DSWRAPPER_INCLUDED)
#define ENUMS_DSWRAPPER_INCLUDED

namespace DSWrapper 
{
	public enum class PinMediaType
	{
		Audio, Video, Subtitle, Unknown
	};

	public enum class ScanType
	{
		Interlaced, Progressive
	};

	public enum class BitrateMode
	{
		CBR, VBR
	};

	public enum class MpaLayer
	{
		Layer1, Layer2
	};

	public enum class WMVideoSubtype
	{
		WMMEDIASUBTYPE_WMV3, WMMEDIASUBTYPE_WMV2
	};
}

#endif //ENUMS_DSWRAPPER_INCLUDED