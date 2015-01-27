#include "StdAfx.h"
#include "StaticSourcePage.h"
#include "resource.h"

StaticSourcePage::StaticSourcePage(LPUNKNOWN punk, HRESULT * phr) :
	CBasePropertyPage(L"HMCStaticSourcePage", punk, IDD_STATICSOURCE_PAGE, IDS_STATICSOURCE_PAGE), m_staticSourceParams(NULL), m_oldFps(0)
{
}

StaticSourcePage::~StaticSourcePage(void)
{
	SAFE_RELEASE(this->m_staticSourceParams);
}

//*********** DesktopSourcePage ************\\

CUnknown * WINAPI StaticSourcePage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    StaticSourcePage * page = new StaticSourcePage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}

//*********** CBasePropertyPage ************\\

HRESULT StaticSourcePage::OnConnect(IUnknown * pUnknown)
{
	CheckPointer(pUnknown, E_POINTER);
	HRESULT hr = S_OK;

	if (this->m_staticSourceParams != NULL)
		return E_FAIL;

	CHECK_HR(hr = pUnknown->QueryInterface(IID_IHMCStaticSource, (void **)&this->m_staticSourceParams));

done:
	return hr;
}

HRESULT StaticSourcePage::OnDisconnect(void)
{
	SAFE_RELEASE(this->m_staticSourceParams);

	return S_OK;
}

HRESULT StaticSourcePage::OnActivate(void)
{
	RefreshValues();

	return S_OK;
}

HRESULT StaticSourcePage::OnApplyChanges(void)
{
	if (this->m_bDirty && this->m_staticSourceParams != NULL)
	{
		this->m_staticSourceParams->SetFrameRate(GetValue(IDC_EDIT_RATE));
		
		wchar_t path[MAX_PATH];
		Static_GetText(GetDlgItem(this->m_Dlg, IDC_EDIT_PATH), path, MAX_PATH);

		if (SetBitmapFile(path) != S_OK)
			Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_PATH), L"");

		RefreshValues();
	}

	return S_OK;
}

INT_PTR StaticSourcePage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

			case IDC_BUTTON_BROWSE:
			{	
				OPENFILENAME ofn;
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = this->m_Dlg;

				ofn.hInstance = NULL;
				ofn.lpstrFilter = NULL;
				ofn.lpstrCustomFilter = NULL;
				ofn.nMaxCustFilter = NULL;
				ofn.nFilterIndex = NULL;

				wchar_t ext[MAX_PATH];
				wcscpy_s(ext, L"*.bmp");
				ofn.lpstrFile = ext;

				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = NULL;
				ofn.lpstrInitialDir = NULL;
				ofn.lpstrTitle = L"Load Bitmap";

				ofn.Flags = NULL;
				ofn.nFileOffset = NULL;
				ofn.nFileExtension = NULL;
				ofn.lpstrDefExt = NULL;
				ofn.lCustData = NULL;
				ofn.lpfnHook = NULL;
				ofn.lpTemplateName = NULL;

				if (GetOpenFileName(&ofn))
				{
					Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_PATH), ofn.lpstrFile);
					SetDirty();
				}
			}
			break;
		}
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

void StaticSourcePage::SetDirty(void)
{
	this->m_bDirty = TRUE;
	if (this->m_pPageSite)
	{
		this->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}

int StaticSourcePage::GetValue(int dlgItem)
{
	WCHAR text[10];
	if (GetDlgItemText(this->m_Dlg, dlgItem, text, 10))
	{
		return _wtoi(text);
	}

	return 0;
}

void StaticSourcePage::RefreshValues(void)
{
	if (this->m_staticSourceParams != NULL)
	{
		this->m_staticSourceParams->GetFrameRate(&this->m_oldFps);
	}

	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETRANGE, 0, MAKELPARAM(60, 1));
	SendDlgItemMessage(this->m_Dlg, IDC_SPIN_RATE, UDM_SETPOS, 0, this->m_oldFps);
}

HRESULT StaticSourcePage::SetBitmapFile(wchar_t * path)
{
	HRESULT hr = E_FAIL;
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD read, readTotal = 0, size = GetFileSize(file, NULL);
		if (size > 0)
		{
			BYTE * buffer = (BYTE *)malloc(size);
			if (buffer != NULL)
			{
				while (ReadFile(file, buffer + readTotal, size - readTotal, &read, NULL) && read > 0)
				{
					readTotal += read;
				}

				if (this->m_staticSourceParams->SetBitmapData(buffer, size) == S_OK)
					hr = S_OK;

				free(buffer);
			}
		}
		CloseHandle(file);
	}

	return hr;
}