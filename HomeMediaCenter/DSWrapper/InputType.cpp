#include "StdAfx.h"
#include "DSWrapper.h"
#include "InputType.h"
#include "DSException.h"
#include "DesktopSourceFilter.h"

namespace DSWrapper 
{
	InputDictionaryHelper inputDictionaryHelper;

	InputType ^ InputType::Desktop(UINT32 fps)
	{
		if (fps == 0)
			throw gcnew DSException(L"Desktop - frame rate must be specified", 0);
		return gcnew DesktopInput(fps);
	}

	InputType ^ InputType::Webcam(System::String ^ videoName, System::String ^ audioName)
	{
		return gcnew WebcamInput(videoName, audioName);
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


	//********************************************
	//*************** WebcamInput ****************
	//********************************************


	HRESULT WebcamInput::GetInputFilter(IBaseFilter ** inputFilter)
	{
		HRESULT hr = S_OK;
		SinkFilterGraph * videoFilter = NULL;
		SinkFilterGraph * audioFilter = NULL;
		InputDictionary ^ dict = inputDictionaryHelper.GetDictionary();
		
		System::Threading::Monitor::Enter(dict);
		try
		{
			if (this->m_videoName != nullptr)
			{
				if (dict->ContainsKey(this->m_videoName))
				{
					videoFilter = (SinkFilterGraph *)((void *)dict[this->m_videoName]);
				}
				else
				{
					IBaseFilter * filter = GetInputDevice(this->m_videoName, CLSID_VideoInputDeviceCategory);
					if (filter != NULL)
					{
						IPin * pin = DSEncoder::GetFirstPin(filter, PINDIR_OUTPUT);
						if (pin != NULL)
						{
							videoFilter = SinkFilterGraph::Create(pin);
							if (videoFilter != NULL)
								dict->Add(this->m_videoName, System::IntPtr(videoFilter));

							SAFE_RELEASE(pin);
						}
						SAFE_RELEASE(filter);
					}
				}
			}

			if (this->m_audioName != nullptr)
			{
				if (dict->ContainsKey(this->m_audioName))
				{
					audioFilter = (SinkFilterGraph *)((void *)dict[this->m_audioName]);
				}
				else
				{
					IBaseFilter * filter = GetInputDevice(this->m_audioName, CLSID_AudioInputDeviceCategory);
					if (filter != NULL)
					{
						IPin * pin = DSEncoder::GetFirstPin(filter, PINDIR_OUTPUT);
						if (pin != NULL)
						{
							audioFilter = SinkFilterGraph::Create(pin);
							if (audioFilter != NULL)
								dict->Add(this->m_audioName, System::IntPtr(audioFilter));

							SAFE_RELEASE(pin);
						}
						SAFE_RELEASE(filter);
					}
				}
			}

		}
		finally
		{
			System::Threading::Monitor::Exit(dict);
		}

		*inputFilter = SinkSourceFilter::Create();
		if (*inputFilter == NULL)
			return E_FAIL;

		if (videoFilter != NULL)
			videoFilter->InsertSourcePin((SinkSourceFilter*)*inputFilter);

		if (audioFilter != NULL)
			audioFilter->InsertSourcePin((SinkSourceFilter*)*inputFilter);

		return S_OK;
	}

	System::Collections::Generic::List<System::String^> ^ WebcamInput::GetInputDeviceNames(REFCLSID category)
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
		CHECK_HR(hr = deviceEnum->CreateClassEnumerator(category, &enumMoniker, 0));

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

	IBaseFilter * WebcamInput::GetInputDevice(System::String ^ name, REFCLSID category)
	{
		HRESULT hr = S_OK;

		IMoniker * moniker = NULL;
		ICreateDevEnum * deviceEnum = NULL;
		IEnumMoniker * enumMoniker = NULL;
		IPropertyBag * propBag = NULL;
		IBaseFilter * filter = NULL;

		VARIANT var;
		VariantInit(&var);

		pin_ptr<const wchar_t> pName = PtrToStringChars(name);

		CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&deviceEnum));
		CHECK_HR(hr = deviceEnum->CreateClassEnumerator(category, &enumMoniker, 0));

		enumMoniker->Reset();
		while(enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			if (moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propBag) >= 0)
			{
				if (propBag->Read(L"FriendlyName", &var, NULL) == S_OK)
				{
					if (var.vt == VT_BSTR && _wcsicmp(pName, var.bstrVal) == 0)
					{
						CHECK_HR(hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter));
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

		return filter;
	}


	//********************************************
	//********** InputDictionaryHelper ***********
	//********************************************

	InputDictionaryHelper::InputDictionaryHelper(void) : m_dictionary(gcnew InputDictionary())
	{
	}

	InputDictionaryHelper::~InputDictionaryHelper(void)
	{
		System::Threading::Monitor::Enter(this->m_dictionary);
		try
		{
			for each (System::IntPtr pointer in this->m_dictionary->Values)
			{
				SinkFilterGraph * filter = (SinkFilterGraph *)((void *)pointer);
				filter->ReleaseAll();
			}
		}
		finally
		{
			System::Threading::Monitor::Exit(this->m_dictionary);
		}
	}
}
