#include "StdAfx.h"
#include "FrameRatePage.h"
#include "resource.h"

FrameRatePage::FrameRatePage(LPUNKNOWN punk, HRESULT * phr) :
	CBasePropertyPage(L"HMCFrameRateFilterPage", punk, IDD_FRAMERATE_PAGE, IDS_FRAMERATE_PAGE), m_frameRateParams(NULL), m_oldFps(0)
{
}

FrameRatePage::~FrameRatePage(void)
{
	SAFE_RELEASE(this->m_frameRateParams);
}

//************* FrameRatePage **************\\

CUnknown * WINAPI FrameRatePage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    FrameRatePage * page = new FrameRatePage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}

//*********** CBasePropertyPage ************\\

HRESULT FrameRatePage::OnConnect(IUnknown * pUnknown)
{
	CheckPointer(pUnknown, E_POINTER);
	HRESULT hr = S_OK;

	if (this->m_frameRateParams != NULL)
		return E_FAIL;

	CHECK_HR(hr = pUnknown->QueryInterface(IID_IHMCFrameRate, (void **)&this->m_frameRateParams));

done:
	return hr;
}

HRESULT FrameRatePage::OnDisconnect(void)
{
	SAFE_RELEASE(this->m_frameRateParams);

	return S_OK;
}

HRESULT FrameRatePage::OnActivate(void)
{
	RefreshValues();

	return S_OK;
}

HRESULT FrameRatePage::OnApplyChanges(void)
{
	if (this->m_bDirty && this->m_frameRateParams != NULL)
	{
		this->m_frameRateParams->SetFrameRate(GetValue(IDC_EDIT_RATE));

		RefreshValues();
	}

	return S_OK;
}

INT_PTR FrameRatePage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		}
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

void FrameRatePage::SetDirty(void)
{
	this->m_bDirty = TRUE;
	if (this->m_pPageSite)
	{
		this->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}

int FrameRatePage::GetValue(int dlgItem)
{
	WCHAR text[10];
	if (GetDlgItemText(this->m_Dlg, dlgItem, text, 10))
	{
		return _wtoi(text);
	}

	return 0;
}

void FrameRatePage::RefreshValues(void)
{
	if (this->m_frameRateParams != NULL)
	{
		this->m_frameRateParams->GetFrameRate(&this->m_oldFps);
	}

	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETRANGE, 0, MAKELPARAM(60, 1));
	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETPOS, 0, this->m_oldFps);
}