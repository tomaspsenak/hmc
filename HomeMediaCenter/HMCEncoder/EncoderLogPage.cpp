#include "StdAfx.h"
#include "EncoderLogPage.h"
#include "resource.h"

#define UWM_LOG_MESSAGE (WM_APP + 1)

HWND EncoderLogPage::pageHandle = NULL;
CCritSec * EncoderLogPage::handleLock = new CCritSec();

EncoderLogPage::EncoderLogPage(LPUNKNOWN punk, HRESULT * phr) : 
	CBasePropertyPage(L"HMCEncoderLogPage", punk, IDD_ENCODER_LOG_PAGE, IDS_ENCODER_LOG_PAGE), m_maxWidth(0)
{
}

EncoderLogPage::~EncoderLogPage(void)
{
}

//*********** CBasePropertyPage ************\\

HRESULT EncoderLogPage::OnActivate(void)
{
	CAutoLock cAutoLock(handleLock);

	pageHandle = this->m_Dlg;
	av_log_set_callback(EncoderLogPage::LogCallback);

	return S_OK;
}

HRESULT EncoderLogPage::OnDeactivate(void)
{
	CAutoLock cAutoLock(handleLock);

	pageHandle = NULL;
	av_log_set_callback(av_log_default_callback);

	return S_OK;
}

INT_PTR EncoderLogPage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == UWM_LOG_MESSAGE && lParam)
	{
		CString * value = (CString *)lParam;
		TEXTMETRIC tm;
	
		CListBox * cListBox = (CListBox *)CListBox::FromHandle(GetDlgItem(pageHandle, IDC_LIST_LOG));
		CDC * cdc = cListBox->GetDC();
		
		CFont * font = cListBox->GetFont();
		CFont * oldFont = cdc->SelectObject(font);

		cdc->GetTextMetrics(&tm);
		CSize size = cdc->GetTextExtent(*value);
		size.cx += tm.tmAveCharWidth;

		cdc->SelectObject(oldFont);
		cListBox->ReleaseDC(cdc);

		int count = cListBox->GetCount();
		cListBox->InsertString(count, *value);
		cListBox->SetCurSel(count);

		if (size.cx > this->m_maxWidth)
		{
			this->m_maxWidth = size.cx;
			cListBox->SetHorizontalExtent(this->m_maxWidth);
		}
				
		if (count >= 500)
			cListBox->DeleteString(0);

		delete value;
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

//************ EncoderLogPage **************\\

void EncoderLogPage::LogCallback(void * avcl, int level, const char * fmt, va_list vl)
{
	CAutoLock cAutoLock(handleLock);

	if (pageHandle)
	{
		int length = _vscprintf(fmt, vl);
		if (length > 0)
		{
			length++;

			char * buffer = new char[length];
			if (buffer)
			{
				vsprintf_s(buffer, length, fmt, vl);

				CString * value = new CString(buffer);
				if (value)
					PostMessage(pageHandle, UWM_LOG_MESSAGE, 0L, (LPARAM)(LPCTSTR)value);

				delete [] buffer;
			}
		}
	}
}

CUnknown * WINAPI EncoderLogPage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    EncoderLogPage * page = new EncoderLogPage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}