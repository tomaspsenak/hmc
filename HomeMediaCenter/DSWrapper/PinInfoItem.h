#pragma once

#if !defined(PININFOITEM_DSWRAPPER_INCLUDED)
#define PININFOITEM_DSWRAPPER_INCLUDED

#include "Enums.h"

namespace DSWrapper 
{
	public ref class PinInfoItem
	{
		public:		property DWORD Index
					{
						DWORD get() { return m_index; }
					}

					property DWORD PinIndex
					{
						DWORD get() { return m_streamPin == nullptr ? m_index : m_streamPin->m_index; }
					}
			
					property PinMediaType MediaType
					{
						PinMediaType get() { return m_mediaType; }
					}

					property System::Boolean IsSelected
					{
						System::Boolean get() { return m_selected ? true : false; }
						void set(System::Boolean value) { m_selected = (value) ? TRUE : FALSE; }
					}

					property System::Boolean IsStream
					{
						System::Boolean get() { return m_streamPin == nullptr ? false : true; }
					}

					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0} pin ({1}) - {2}", m_mediaType, PinIndex, IsSelected);
					}

		internal:	PinInfoItem(DWORD index, BOOL selected, PinMediaType mediaType, PinInfoItem ^ streamPin) : m_index(index), 
						m_selected(selected), m_mediaType(mediaType), m_streamPin(streamPin) { };

		private:	PinInfoItem ^ m_streamPin;
					PinMediaType m_mediaType;
					BOOL m_selected;
					DWORD m_index;
	};

	public ref class PinSubtitleItem : public PinInfoItem
	{
		public:		property System::String^ LangName
					{
						System::String^ get() { return m_langName; }
					}
			
					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0} - {1}", PinInfoItem::ToString(), m_langName);
					}

		internal:	PinSubtitleItem(DWORD index, BOOL selected, PinInfoItem ^ streamPin, System::String^ langName) : 
						PinInfoItem(index, selected, PinMediaType::Subtitle, streamPin), m_langName(langName) { };

		private:	System::String^ m_langName;
	};

	public ref class PinVideoItem : public PinInfoItem
	{
		public:		property LONG Width
					{
						LONG get() { return m_width; }
					}

					property LONG Height
					{
						LONG get() { return m_height; }
					}
			
					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0} - {1}x{2}", PinInfoItem::ToString(), m_width, m_height);
					}

		internal:	PinVideoItem(DWORD index, BOOL selected, PinInfoItem ^ streamPin, LONG width, LONG height) : 
						PinInfoItem(index, selected, PinMediaType::Video, streamPin), m_width(width), m_height(height) { };

		private:	LONG m_width;
					LONG m_height;
	};
}

#endif //PININFOITEM_DSWRAPPER_INCLUDED

