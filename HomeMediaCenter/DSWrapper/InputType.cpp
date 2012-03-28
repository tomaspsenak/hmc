#include "StdAfx.h"
#include "InputType.h"
#include "DSException.h"
#include "DesktopSourceFilter.h"

namespace DSWrapper 
{
	InputType ^ InputType::Desktop(UINT32 fps)
	{
		if (fps == 0)
			throw gcnew DSException(L"Desktop - frame rate must be specified", 0);
		return gcnew DesktopInput(fps);
	}

	InputType ^ InputType::Webcam(System::String ^ camName)
	{
		return gcnew WebcamInput(camName);
	}

	HRESULT DesktopInput::GetInputFilter(IBaseFilter ** inputFilter)
	{
		HRESULT hr = S_OK;
		IBaseFilter * filter = NULL;

		filter = new DesktopSourceFilter(NULL, &hr, this->m_fps);
		if (filter == NULL)
		{
			CHECK_HR(hr = E_OUTOFMEMORY);
		}
		else
		{
			filter->AddRef();
		}
		CHECK_HR(hr);

		*inputFilter = filter;
		filter = NULL;

	done:

		SAFE_RELEASE(filter);

		return hr;
	}

	HRESULT WebcamInput::GetInputFilter(IBaseFilter ** inputFilter)
	{
		HRESULT hr = S_OK;

		IMoniker * moniker = NULL;
		ICreateDevEnum * deviceEnum = NULL;
		IEnumMoniker * enumMoniker = NULL;
		IPropertyBag * propBag = NULL;

		VARIANT var;
		VariantInit(&var);

		pin_ptr<const wchar_t> pCamName = PtrToStringChars(this->m_camName);

		CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&deviceEnum));
		CHECK_HR(hr = deviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,	&enumMoniker, 0));

		enumMoniker->Reset();
		while(enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			if (moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propBag) >= 0)
			{
				if (propBag->Read(L"FriendlyName", &var, NULL) == S_OK)
				{
					if (var.vt == VT_BSTR && _wcsicmp(pCamName, var.bstrVal) == 0)
					{
						CHECK_HR(hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)inputFilter));
						break;
					}
					VariantClear(&var);
				}
				SAFE_RELEASE(propBag);
			}
			SAFE_RELEASE(moniker);
		}

	done:

		SAFE_RELEASE(moniker);
		SAFE_RELEASE(deviceEnum);
		SAFE_RELEASE(enumMoniker);
		SAFE_RELEASE(propBag);

		VariantClear(&var);

		return hr;
	}

	System::Collections::Generic::List<System::String^> ^ WebcamInput::GetWebcamNames(void)
	{
		HRESULT hr = S_OK;

		IMoniker * moniker = NULL;
		ICreateDevEnum * deviceEnum = NULL;
		IEnumMoniker * enumMoniker = NULL;
		IPropertyBag * propBag = NULL;

		VARIANT var;
		VariantInit(&var);

		System::Collections::Generic::List<System::String^> ^ list = gcnew System::Collections::Generic::List<System::String^>();

		CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&deviceEnum));
		CHECK_HR(hr = deviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,	&enumMoniker, 0));

		enumMoniker->Reset();
		while(enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			if (moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propBag) >= 0)
			{
				if (propBag->Read(L"FriendlyName", &var, NULL) == S_OK)
				{
					if (var.vt == VT_BSTR)
					{
						System::String ^ name = System::Runtime::InteropServices::Marshal::PtrToStringBSTR(
							static_cast<System::IntPtr>(var.bstrVal));
						list->Add(name);
					}
					VariantClear(&var);
				}
				SAFE_RELEASE(propBag);
			}
			SAFE_RELEASE(moniker);
		}

	done:

		SAFE_RELEASE(moniker);
		SAFE_RELEASE(deviceEnum);
		SAFE_RELEASE(enumMoniker);
		SAFE_RELEASE(propBag);

		VariantClear(&var);

		return list;
	}
}
