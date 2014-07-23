#include "StdAfx.h"
#include "DesktopSourcePage.h"
#include "resource.h"

DesktopSourcePage::DesktopSourcePage(LPUNKNOWN punk, HRESULT * phr) :
	CBasePropertyPage(L"HMCDesktopSourcePage", punk, IDD_DESKTOPSOURCE_PAGE, IDS_DESKTOPSOURCE_PAGE), m_desktopSourceParams(NULL),
	m_oldFps(0), m_oldWidth(0), m_oldHeight(0), m_oldKeepAspectRatio(FALSE), m_oldCaptureCursor(FALSE), m_oldVideoQualityPosition(0)
{
}

DesktopSourcePage::~DesktopSourcePage(void)
{
	SAFE_RELEASE(this->m_desktopSourceParams);
}

//*********** DesktopSourcePage ************\\

CUnknown * WINAPI DesktopSourcePage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    DesktopSourcePage * page = new DesktopSourcePage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}

//*********** CBasePropertyPage ************\\

HRESULT DesktopSourcePage::OnConnect(IUnknown * pUnknown)
{
	CheckPointer(pUnknown, E_POINTER);
	HRESULT hr = S_OK;

	if (this->m_desktopSourceParams != NULL)
		return E_FAIL;

	CHECK_HR(hr = pUnknown->QueryInterface(IID_IHMCDesktopSource, (void **)&this->m_desktopSourceParams));

done:
	return hr;
}

HRESULT DesktopSourcePage::OnDisconnect(void)
{
	SAFE_RELEASE(this->m_desktopSourceParams);

	return S_OK;
}

HRESULT DesktopSourcePage::OnActivate(void)
{
	RefreshValues();

	return S_OK;
}

HRESULT DesktopSourcePage::OnApplyChanges(void)
{
	if (this->m_bDirty && this->m_desktopSourceParams != NULL)
	{
		this->m_desktopSourceParams->SetFrameRate(GetValue(IDC_EDIT_RATE));

		this->m_desktopSourceParams->SetWidth(GetValue(IDC_EDIT_WIDTH));

		this->m_desktopSourceParams->SetHeight(GetValue(IDC_EDIT_HEIGHT));

		this->m_desktopSourceParams->SetAspectRatio(Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_KEEPASPECT)));

		this->m_desktopSourceParams->SetCaptureCursor(Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_CAPTURECURSOR)));

		this->m_desktopSourceParams->SetVideoQuality(GetSliderPercentage(IDC_SLIDER_QUALITY));

		RefreshValues();
	}

	return S_OK;
}

INT_PTR DesktopSourcePage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND)
	{
		int dlgItem = LOWORD(wParam);

		switch (dlgItem)
		{
			case IDC_EDIT_RATE:
				if (this->m_oldFps != GetValue(IDC_EDIT_RATE))
					SetDirty();
				break;

			case IDC_EDIT_WIDTH:
				if (this->m_oldWidth != GetValue(IDC_EDIT_WIDTH))
					SetDirty();
				break;

			case IDC_EDIT_HEIGHT:
				if (this->m_oldHeight != GetValue(IDC_EDIT_HEIGHT))
					SetDirty();
				break;

			case IDC_CHECK_KEEPASPECT:
				if (this->m_oldKeepAspectRatio != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_KEEPASPECT)))
					SetDirty();
				break;

			case IDC_CHECK_CAPTURECURSOR:
				if (this->m_oldCaptureCursor != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_CAPTURECURSOR)))
					SetDirty();
				break;
		}
	}
	else if (uMsg == WM_HSCROLL)
	{
		if ((HWND)lParam == GetDlgItem(this->m_Dlg, IDC_SLIDER_QUALITY))
		{
			if (this->m_oldVideoQualityPosition != GetSliderPosition(IDC_SLIDER_QUALITY))
				SetDirty();
		}
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

void DesktopSourcePage::SetDirty(void)
{
	this->m_bDirty = TRUE;
	if (this->m_pPageSite)
	{
		this->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}

int DesktopSourcePage::GetValue(int dlgItem)
{
	WCHAR text[10];
	if (GetDlgItemText(this->m_Dlg, dlgItem, text, 10))
	{
		return _wtoi(text);
	}

	return 0;
}

void DesktopSourcePage::RefreshValues(void)
{
	CString str;
	BYTE oldVideoQuality = 0;

	if (this->m_desktopSourceParams != NULL)
	{
		this->m_desktopSourceParams->GetFrameRate(&this->m_oldFps);
		this->m_desktopSourceParams->GetWidth(&this->m_oldWidth);
		this->m_desktopSourceParams->GetHeight(&this->m_oldHeight);
		this->m_desktopSourceParams->GetAspectRatio(&this->m_oldKeepAspectRatio);
		this->m_desktopSourceParams->GetCaptureCursor(&this->m_oldCaptureCursor);
		this->m_desktopSourceParams->GetVideoQuality(&oldVideoQuality);
	}

	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETRANGE, 0, MAKELPARAM(60, 1));
	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETPOS, 0, this->m_oldFps);

	str.Format(L"%d", this->m_oldWidth);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_WIDTH), str);

	str.Format(L"%d", this->m_oldHeight);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_HEIGHT), str);

	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_KEEPASPECT), this->m_oldKeepAspectRatio);

	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_CAPTURECURSOR), this->m_oldCaptureCursor);

	SendDlgItemMessage(this->m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELPARAM(0, 2));
	this->m_oldVideoQualityPosition = SetSliderPosition(IDC_SLIDER_QUALITY, oldVideoQuality);
}

BYTE DesktopSourcePage::GetSliderPercentage(int dlgItem)
{
	return GetSliderPosition(dlgItem) * 33;
}

BYTE DesktopSourcePage::SetSliderPosition(int dlgItem, BYTE percentage)
{
	BYTE position = min(percentage / 33, 2);
	SendDlgItemMessage(this->m_Dlg, dlgItem, TBM_SETPOS, TRUE, position);
	return position;
}

BYTE DesktopSourcePage::GetSliderPosition(int dlgItem)
{
	return (BYTE)SendDlgItemMessage(this->m_Dlg, dlgItem, TBM_GETPOS, 0, 0);
}