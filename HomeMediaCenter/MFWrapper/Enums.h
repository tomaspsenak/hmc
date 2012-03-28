#pragma once

#if !defined(ENUMS_MFWRAPPER_INCLUDED)
#define ENUMS_MFWRAPPER_INCLUDED

namespace MFWrapper 
{
	public enum class ContainerType
	{ 
		MF_ASF, MF_MP4, MF_3GP
	};

	public enum class StreamMediaType
	{
		Audio, Video, Unknown
	};

	public enum class VideoFormat
	{
		H264, WMV1, WMV2, WMV3, WVC1
	};

	public enum class InterlaceMode
	{
		Unknown, Progressive, FieldInterleavedUpperFirst, FieldInterleavedLowerFirst, FieldSingleUpper, FieldSingleLower, MixedInterlaceOrProgressive
	};

	public enum class AudioFormat
	{
		AAC, WMAudioV8, WMAudioV9
	};
}

#endif //ENUMS_MFWRAPPER_INCLUDED

