#include "StdAfx.h"
#include "FrameWriterPage.h"
#include "resource.h"

FrameWriterPage::FrameWriterPage(LPUNKNOWN punk, HRESULT * phr) :
	CBasePropertyPage(L"HMCFrameWriterFilterPage", punk, IDD_FRAMEWRITER_PAGE, IDS_FRAMEWRITER_PAGE), m_frameWriterParams(NULL),
	m_oldImageFormat(ImageFormat_BMP), m_oldWidth(0), m_oldHeight(0), m_oldImageBitrate(0)
{
}

FrameWriterPage::~FrameWriterPage(void)
{
	SAFE_RELEASE(this->m_frameWriterParams);
}

//************ FrameWriterPage *************\\

CUnknown * WINAPI FrameWriterPage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    FrameWriterPage * page = new FrameWriterPage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}

//*********** CBasePropertyPage ************\\

HRESULT FrameWriterPage::OnConnect(IUnknown * pUnknown)
{
	CheckPointer(pUnknown, E_POINTER);
	HRESULT hr = S_OK;

	if (this->m_frameWriterParams != NULL)
		return E_FAIL;

	CHECK_HR(hr = pUnknown->QueryInterface(IID_IHMCFrameWriter, (void **)&this->m_frameWriterParams));

done:
	return hr;
}

HRESULT FrameWriterPage::OnDisconnect(void)
{
	SAFE_RELEASE(this->m_frameWriterParams);

	return S_OK;
}

HRESULT FrameWriterPage::OnActivate(void)
{
	RefreshValues();

	return S_OK;
}

HRESULT FrameWriterPage::OnApplyChanges(void)
{
	if (this->m_bDirty && this->m_frameWriterParams != NULL)
	{
		this->m_frameWriterParams->SetFormat((ImageFormat)ComboBox_GetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT)));

		this->m_frameWriterParams->SetWidth(GetValue(IDC_EDIT_WIDTH));

		this->m_frameWriterParams->SetHeight(GetValue(IDC_EDIT_HEIGHT));

		this->m_frameWriterParams->SetBitrate(GetValue(IDC_EDIT_IMGBITRATE));

		RefreshValues();
	}

	return S_OK;
}

INT_PTR FrameWriterPage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND)
	{
		int dlgItem = LOWORD(wParam);

		switch (dlgItem)
		{
			case IDC_COMBO_IMGFORMAT:
			{
				ImageFormat imageFormat = (ImageFormat)ComboBox_GetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT));

				if (this->m_oldImageFormat != imageFormat)
					SetDirty();

				Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_IMGBITRATE), imageFormat == ImageFormat_JPEG);

				break;
			}
			case IDC_EDIT_WIDTH:
				if (this->m_oldWidth != GetValue(IDC_EDIT_WIDTH))
					SetDirty();
				break;

			case IDC_EDIT_HEIGHT:
				if (this->m_oldHeight != GetValue(IDC_EDIT_HEIGHT))
					SetDirty();
				break;

			case IDC_EDIT_IMGBITRATE:
				if (this->m_oldImageBitrate != GetValue(IDC_EDIT_IMGBITRATE))
					SetDirty();
				break;
		}
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

void FrameWriterPage::SetDirty(void)
{
	this->m_bDirty = TRUE;
	if (this->m_pPageSite)
	{
		this->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}

int FrameWriterPage::GetValue(int dlgItem)
{
	WCHAR text[10];
	if (GetDlgItemText(this->m_Dlg, dlgItem, text, 10))
	{
		return _wtoi(text);
	}

	return 0;
}

void FrameWriterPage::RefreshValues(void)
{
	CString str;

	if (this->m_frameWriterParams != NULL)
	{
		this->m_frameWriterParams->GetFormat(&this->m_oldImageFormat);
		this->m_frameWriterParams->GetWidth(&this->m_oldWidth);
		this->m_frameWriterParams->GetHeight(&this->m_oldHeight);
		this->m_frameWriterParams->GetBitrate(&this->m_oldImageBitrate);
	}

	ComboBox_ResetContent(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT));
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT), L"BMP");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT), L"JPEG");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT), L"PNG");
	ComboBox_SetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_IMGFORMAT), this->m_oldImageFormat);

	str.Format(L"%d", this->m_oldWidth);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_WIDTH), str);

	str.Format(L"%d", this->m_oldHeight);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_HEIGHT), str);

	str.Format(L"%d", this->m_oldImageBitrate);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_IMGBITRATE), str);
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_IMGBITRATE), this->m_oldImageFormat == ImageFormat_JPEG);
}