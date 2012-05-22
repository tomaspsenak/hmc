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

					property PinMediaType MediaType
					{
						PinMediaType get() { return m_mediaType; }
					}

					property System::Boolean IsSelected
					{
						System::Boolean get() { return m_selected ? true : false; }
						void set(System::Boolean value) { m_selected = (value) ? TRUE : FALSE; }
					}

					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0} pin ({1}) - {2}", m_mediaType, m_index, IsSelected);
					}

		internal:	PinInfoItem(DWORD index, BOOL selected, PinMediaType mediaType) : m_index(index), m_selected(selected), 
						m_mediaType(mediaType) { };

		private:	PinMediaType m_mediaType;
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

		internal:	PinSubtitleItem(DWORD index, BOOL selected, System::String^ langName) : 
						PinInfoItem(index, selected, PinMediaType::Subtitle), m_langName(langName)  { };

		private:	System::String^ m_langName;
	};
}

#endif //PININFOITEM_DSWRAPPER_INCLUDED

