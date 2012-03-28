#pragma once

#if !defined(VIDEOSTREAM_MFWRAPPER_INCLUDED)
#define VIDEOSTREAM_MFWRAPPER_INCLUDED

#include "Enums.h"
#include "Helpers.h"

namespace MFWrapper 
{
	public ref class VideoStream
	{
		public:		VideoStream(VideoFormat subtype, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator, 
						InterlaceMode interlace, UINT32 bitrate) : m_width(width), m_height(height), m_numerator(numerator),
						m_denominator(denominator), m_aspectX(1), m_aspectY(1), m_bitrate(bitrate)
					{
						switch (subtype)
						{
							case VideoFormat::H264:		m_subtype = FromGUID(MFVideoFormat_H264); break;
							case VideoFormat::WMV1:		m_subtype = FromGUID(MFVideoFormat_WMV1); break;
							case VideoFormat::WMV2:		m_subtype = FromGUID(MFVideoFormat_WMV2); break;
							case VideoFormat::WMV3:		m_subtype = FromGUID(MFVideoFormat_WMV3); break;
							case VideoFormat::WVC1:		m_subtype = FromGUID(MFVideoFormat_WVC1); break;
						}
						switch (interlace)
						{
							case InterlaceMode::Progressive:					m_interlace = MFVideoInterlace_Progressive; break;
							case InterlaceMode::FieldInterleavedUpperFirst:		m_interlace = MFVideoInterlace_FieldInterleavedUpperFirst; break;
							case InterlaceMode::FieldInterleavedLowerFirst:		m_interlace = MFVideoInterlace_FieldInterleavedLowerFirst; break;
							case InterlaceMode::FieldSingleUpper:				m_interlace = MFVideoInterlace_FieldSingleUpper; break;
							case InterlaceMode::FieldSingleLower:				m_interlace = MFVideoInterlace_FieldSingleLower; break;
							case InterlaceMode::MixedInterlaceOrProgressive:	m_interlace = MFVideoInterlace_MixedInterlaceOrProgressive; break;
							default:											m_interlace = MFVideoInterlace_Progressive; break;
						}
					}
			
					property System::Guid Subtype
					{
						System::Guid get() { return m_subtype; }
					}

					property UINT32 Width
					{
						UINT32 get() { return m_width; }
					}

					property UINT32 Height
					{
						UINT32 get() { return m_height; }
					}

					property UINT32 Numerator
					{
						UINT32 get() { return m_numerator; }
					}

					property UINT32 Denominator
					{
						UINT32 get() { return m_denominator; }
					}

					property UINT32 AspectX
					{
						UINT32 get() { return m_aspectX; }
					}

					property UINT32 AspectY
					{
						UINT32 get() { return m_aspectY; }
					}

					property InterlaceMode Interlace
					{
						InterlaceMode get() 
						{ 
							switch (m_interlace)
							{
								case MFVideoInterlace_Progressive:					return InterlaceMode::Progressive;
								case MFVideoInterlace_FieldInterleavedUpperFirst:	return InterlaceMode::FieldInterleavedUpperFirst;
								case MFVideoInterlace_FieldInterleavedLowerFirst:	return InterlaceMode::FieldInterleavedLowerFirst;
								case MFVideoInterlace_FieldSingleUpper:				return InterlaceMode::FieldSingleUpper;
								case MFVideoInterlace_FieldSingleLower:				return InterlaceMode::FieldSingleLower;
								case MFVideoInterlace_MixedInterlaceOrProgressive:	return InterlaceMode::MixedInterlaceOrProgressive;
								default:											return InterlaceMode::Unknown;
							}
						}
					}

					property UINT32 Bitrate
					{
						UINT32 get() { return m_bitrate; }
					}

					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0}x{1} - {2:#.##} fps - {3} bps - {4} - {5}", m_width, m_height, 
							(System::Double)(m_numerator) / m_denominator, m_bitrate, Interlace, m_subtype);
					}

		internal:	VideoStream(GUID subtype, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator, UINT32 aspectX, 
						UINT32 aspectY, UINT32 interlace, UINT32 bitrate) : 
						m_width(width), m_height(height), m_numerator(numerator), m_denominator(denominator), 
						m_aspectX(aspectX), m_aspectY(aspectY), m_interlace(interlace), m_bitrate(bitrate) 
					{
						m_subtype = FromGUID(subtype);
					}
			
					property UINT32 InterlaceInternal
					{
						UINT32 get() { return m_interlace; }
					}

					property GUID SubtypeInternal
					{
						GUID get() { return ToGUID(m_subtype); }
					}

					IMFMediaType * CreateMediaType(void)
					{
						HRESULT hr = S_OK;
						IMFMediaType * mediaType = NULL;

						CHECK_HR(hr = MFCreateMediaType(&mediaType));
						CHECK_HR(hr = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
						CHECK_HR(hr = mediaType->SetGUID(MF_MT_SUBTYPE, SubtypeInternal));
						CHECK_HR(hr = MFSetAttributeSize(mediaType, MF_MT_FRAME_SIZE, m_width, m_height));
						CHECK_HR(hr = MFSetAttributeRatio(mediaType, MF_MT_FRAME_RATE, m_numerator, m_denominator));
						CHECK_HR(hr = MFSetAttributeRatio(mediaType, MF_MT_PIXEL_ASPECT_RATIO, m_aspectX, m_aspectY));
						CHECK_HR(hr = mediaType->SetUINT32(MF_MT_INTERLACE_MODE, m_interlace));
						CHECK_HR(hr = mediaType->SetUINT32(MF_MT_AVG_BITRATE, m_bitrate));

					done:

						if (hr != S_OK)
							SAFE_RELEASE(mediaType);
						return mediaType;
					}

		private:	System::Guid m_subtype;
					UINT32 m_width;
					UINT32 m_height;
					UINT32 m_numerator;
					UINT32 m_denominator;
					UINT32 m_aspectX;
					UINT32 m_aspectY;
					UINT32 m_interlace;
					UINT32 m_bitrate;
	};
}

#endif //VIDEOSTREAM_MFWRAPPER_INCLUDED

