#include "StdAfx.h"
#include "EncoderPage.h"
#include "resource.h"

EncoderPage::EncoderPage(LPUNKNOWN punk, HRESULT * phr) : 
	CBasePropertyPage(L"HMCEncoderPage", punk, IDD_ENCODER_PAGE, IDS_ENCODER_PAGE), m_encoderParams(NULL), 
	m_oldContainer(Container_MPEG2PS), m_oldStreamable(FALSE), m_oldInterlaced(FALSE), m_oldVideoBitrate(0), m_oldVideoBitrateMax(0),
	m_oldVideoBitrateMin(0), m_oldVideoQuality(0), m_oldVideoConstant(FALSE), m_oldAudioConstant(FALSE), m_oldAudioBitrate(0),
	m_oldAudioBitrateMax(0), m_oldAudioBitrateMin(0), m_oldAudioQuality(0), m_oldWidth(0), m_oldHeight(0)
{
}

EncoderPage::~EncoderPage(void)
{
	SAFE_RELEASE(this->m_encoderParams);
}

//************** EncoderPage ***************\\

CUnknown * WINAPI EncoderPage::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    EncoderPage * page = new EncoderPage(pUnk, phr);
    if (page == NULL && phr != NULL)
        *phr = E_OUTOFMEMORY;

    return page;
}

//*********** CBasePropertyPage ************\\

HRESULT EncoderPage::OnConnect(IUnknown * pUnknown)
{
	CheckPointer(pUnknown, E_POINTER);
	HRESULT hr = S_OK;

	if (this->m_encoderParams != NULL)
		return E_FAIL;

	CHECK_HR(hr = pUnknown->QueryInterface(IID_IHMCEncoder, (void **)&this->m_encoderParams));

done:
	return hr;
}

HRESULT EncoderPage::OnDisconnect(void)
{
	SAFE_RELEASE(this->m_encoderParams);

	return S_OK;
}

HRESULT EncoderPage::OnActivate(void)
{
	RefreshValues();

	return S_OK;
}

HRESULT EncoderPage::OnApplyChanges(void)
{
	if (this->m_bDirty && this->m_encoderParams != NULL)
	{
		this->m_encoderParams->SetContainer((Container)ComboBox_GetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER)));

		this->m_encoderParams->SetStreamable(Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_STREAMABLE)));

		this->m_encoderParams->SetVideoInterlace(Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_INTERLACED)));

		//Video bitrate
		if (Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_VCONST)))
		{
			this->m_encoderParams->SetVideoCBR(GetValue(IDC_EDIT_VBITRATE));
		}
		else
		{
			this->m_encoderParams->SetVideoVBR(GetValue(IDC_EDIT_VBITRATE), GetValue(IDC_EDIT_VMAXBITRATE),
				GetValue(IDC_EDIT_VMINBITRATE), GetValue(IDC_EDIT_VQUALITY));
		}

		//Audio bitrate
		if (Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_ACONST)))
		{
			this->m_encoderParams->SetAudioCBR(GetValue(IDC_EDIT_ABITRATE));
		}
		else
		{
			this->m_encoderParams->SetAudioVBR(GetValue(IDC_EDIT_ABITRATE), GetValue(IDC_EDIT_AMAXBITRATE),
				GetValue(IDC_EDIT_AMINBITRATE), GetValue(IDC_EDIT_AQUALITY));
		}

		this->m_encoderParams->SetWidth(GetValue(IDC_EDIT_WIDTH));
		this->m_encoderParams->SetHeight(GetValue(IDC_EDIT_HEIGHT));

		RefreshValues();
	}

	return S_OK;
}

INT_PTR EncoderPage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND)
	{
		int dlgItem = LOWORD(wParam);

		switch (dlgItem)
		{
			case IDC_COMBO_CONTAINER:
				if (this->m_oldContainer != (Container)ComboBox_GetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER)))
					SetDirty();
				break;

			case IDC_CHECK_STREAMABLE:
				if (this->m_oldStreamable != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_STREAMABLE)))
					SetDirty();
				break;

			case IDC_CHECK_INTERLACED:
				if (this->m_oldInterlaced != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_INTERLACED)))
					SetDirty();
				break;

			case IDC_CHECK_VCONST:
				if (this->m_oldVideoConstant != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_VCONST)))
					SetDirty();
				SetVBitrateStaticEnable(!Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_VCONST)));
				break;

			case IDC_EDIT_VBITRATE:
				if (this->m_oldVideoBitrate != GetValue(IDC_EDIT_VBITRATE))
					SetDirty();
				break;

			case IDC_EDIT_VMAXBITRATE:
				if (this->m_oldVideoBitrateMax != GetValue(IDC_EDIT_VMAXBITRATE))
					SetDirty();
				break;

			case IDC_EDIT_VMINBITRATE:
				if (this->m_oldVideoBitrateMin != GetValue(IDC_EDIT_VMINBITRATE))
					SetDirty();
				break;

			case IDC_EDIT_VQUALITY:
				if (this->m_oldVideoQuality != GetValue(IDC_EDIT_VQUALITY))
					SetDirty();
				break;

			case IDC_CHECK_ACONST:
				if (this->m_oldAudioConstant != Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_ACONST)))
					SetDirty();
				SetABitrateStaticEnable(!Button_GetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_ACONST)));
				break;

			case IDC_EDIT_ABITRATE:
				if (this->m_oldAudioBitrate != GetValue(IDC_EDIT_ABITRATE))
					SetDirty();
				break;

			case IDC_EDIT_AMAXBITRATE:
				if (this->m_oldAudioBitrateMax != GetValue(IDC_EDIT_AMAXBITRATE))
					SetDirty();
				break;

			case IDC_EDIT_AMINBITRATE:
				if (this->m_oldAudioBitrateMin != GetValue(IDC_EDIT_AMINBITRATE))
					SetDirty();
				break;

			case IDC_EDIT_AQUALITY:
				if (this->m_oldAudioQuality != GetValue(IDC_EDIT_AQUALITY))
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
		}
	}

	return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);   
}

void EncoderPage::SetDirty(void)
{
	this->m_bDirty = TRUE;
	if (this->m_pPageSite)
	{
		this->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}

int EncoderPage::GetValue(int dlgItem)
{
	WCHAR text[10];
	if (GetDlgItemText(this->m_Dlg, dlgItem, text, 10))
	{
		return _wtoi(text);
	}

	return 0;
}

void EncoderPage::RefreshValues(void)
{
	CString str;

	if (this->m_encoderParams != NULL)
	{
		this->m_encoderParams->GetContainer(&this->m_oldContainer);
		this->m_encoderParams->GetStreamable(&this->m_oldStreamable);
		this->m_encoderParams->GetVideoInterlace(&this->m_oldInterlaced);
		this->m_encoderParams->GetVideoBitrate(&this->m_oldVideoConstant, &this->m_oldVideoBitrate, &this->m_oldVideoBitrateMax,
			&this->m_oldVideoBitrateMin, &this->m_oldVideoQuality);
		this->m_encoderParams->GetAudioBitrate(&this->m_oldAudioConstant, &this->m_oldAudioBitrate, &this->m_oldAudioBitrateMax,
			&this->m_oldAudioBitrateMin, &this->m_oldAudioQuality);
		this->m_encoderParams->GetWidth(&this->m_oldWidth);
		this->m_encoderParams->GetHeight(&this->m_oldHeight);
	}

	ComboBox_ResetContent(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER));
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"MPEG2 PS");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"MPEG2 TS");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"MPEG2 TS H264");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"MP3");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"MP4");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"AVI");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"FLV");
	ComboBox_AddString(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), L"ASF");
	ComboBox_SetCurSel(GetDlgItem(this->m_Dlg, IDC_COMBO_CONTAINER), this->m_oldContainer);

	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_STREAMABLE), this->m_oldStreamable);

	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_INTERLACED), this->m_oldInterlaced);

	//Video bitrate
	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_VCONST), this->m_oldVideoConstant);
	SetVBitrateStaticEnable(!this->m_oldVideoConstant);

	str.Format(L"%d", this->m_oldVideoBitrate);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_VBITRATE), str);

	str.Format(L"%d", this->m_oldVideoBitrateMax);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_VMAXBITRATE), str);

	str.Format(L"%d", this->m_oldVideoBitrateMin);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_VMINBITRATE), str);

	str.Format(L"%d", this->m_oldVideoQuality);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_VQUALITY), str);

	//Audio bitrate
	Button_SetCheck(GetDlgItem(this->m_Dlg, IDC_CHECK_ACONST), this->m_oldAudioConstant);
	SetABitrateStaticEnable(!this->m_oldAudioConstant);

	str.Format(L"%d", this->m_oldAudioBitrate);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_ABITRATE), str);

	str.Format(L"%d", this->m_oldAudioBitrateMax);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_AMAXBITRATE), str);

	str.Format(L"%d", this->m_oldAudioBitrateMin);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_AMINBITRATE), str);

	str.Format(L"%d", this->m_oldAudioQuality);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_AQUALITY), str);

	//Video resolution
	str.Format(L"%d", this->m_oldWidth);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_WIDTH), str);

	str.Format(L"%d", this->m_oldHeight);
	Static_SetText(GetDlgItem(this->m_Dlg, IDC_EDIT_HEIGHT), str);
}

void EncoderPage::SetVBitrateStaticEnable(BOOL enable)
{
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_VMINBITRATE), enable);
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_VMAXBITRATE), enable);
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_VQUALITY), enable);
}

void EncoderPage::SetABitrateStaticEnable(BOOL enable)
{
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_AMINBITRATE), enable);
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_AMAXBITRATE), enable);
	Static_Enable(GetDlgItem(this->m_Dlg, IDC_EDIT_AQUALITY), enable);
}