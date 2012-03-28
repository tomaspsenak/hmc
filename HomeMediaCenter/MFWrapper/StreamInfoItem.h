#pragma once

#if !defined(STREAMINFOITEM_MFWRAPPER_INCLUDED)
#define STREAMINFOITEM_MFWRAPPER_INCLUDED

#include "Enums.h"
#include "VideoStream.h"
#include "AudioStream.h"

namespace MFWrapper 
{
	public ref class StreamInfoItem
	{
		public:		property DWORD Index
					{
						DWORD get() { return m_index; }
					}

					property StreamMediaType MediaType
					{
						StreamMediaType get() { return m_mediaType; }
					}

					property System::Boolean IsSelected
					{
						System::Boolean get() { return m_selected ? true : false; }
						void set(System::Boolean value) { m_selected = (value) ? TRUE : FALSE; }
					}

					property VideoStream^ Video
					{
						VideoStream^ get() { return m_videoStream; }
					}

					property AudioStream^ Audio
					{
						AudioStream^ get() { return m_audioStream; }
					}

					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0} stream ({1}) - {2} - {3}", m_mediaType, m_index, IsSelected, 
							m_videoStream != nullptr ? m_videoStream->ToString() : 
							(m_audioStream != nullptr ? m_audioStream->ToString() : "No info"));
					}

		internal:	StreamInfoItem(DWORD index, BOOL selected) : m_index(index), m_selected(selected), 
						m_mediaType(StreamMediaType::Unknown), m_videoStream(nullptr) { };

					StreamInfoItem(DWORD index, BOOL selected, VideoStream ^ stream) : m_index(index), m_selected(selected), 
						m_mediaType(StreamMediaType::Video), m_videoStream(stream) { };

					StreamInfoItem(DWORD index, BOOL selected, AudioStream ^ stream) : m_index(index), m_selected(selected), 
						m_mediaType(StreamMediaType::Audio), m_audioStream(stream) { };
			
					property DWORD OutputIndex
					{
						DWORD get() { return m_outputIndex; }
						void set(DWORD value) { m_outputIndex = value; }
					}

					property BOOL IsSelectedInternal
					{
						BOOL get() { return m_selected; }
					}

		private:	StreamMediaType m_mediaType;
					BOOL m_selected;
					DWORD m_index;
					DWORD m_outputIndex;
					VideoStream ^ m_videoStream;
					AudioStream ^ m_audioStream;

	};
}

#endif //STREAMINFOITEM_MFWRAPPER_INCLUDED

