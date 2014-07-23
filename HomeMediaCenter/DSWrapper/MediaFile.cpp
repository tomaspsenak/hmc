#include "StdAfx.h"
#include "MediaFile.h"
#include "DSWrapper.h"
#include "DSException.h"

namespace DSWrapper 
{
	MediaFile::MediaFile(void)
	{
	}

	HRESULT MediaFile::GetVideoInfo(IO::FileInfo ^ file, String ^ thumbnailPath, TimeSpan% duration, String ^% resolution, Int32% audioStreamsCount, System::Collections::Generic::List<String^> ^% subtitlesStreams)
	{
		DSEncoder ^ enc = nullptr;
		HRESULT hr = S_OK;
		Int64 encDuration;

		try
		{
			enc = gcnew DSEncoder();
			enc->SetInput(file->FullName, true);

			encDuration = enc->GetDuration();
			duration = TimeSpan::FromMilliseconds((Double)encDuration);
			audioStreamsCount = 0;
			subtitlesStreams = gcnew System::Collections::Generic::List<String^>();

			bool selectVideo = true;
			for each(PinInfoItem ^ item in enc->SourcePins)
			{
				if (item->MediaType == PinMediaType::Video)
				{
					if (selectVideo)
					{
						LONG width = ((PinVideoItem ^)item)->Width;
						LONG height = ((PinVideoItem ^)item)->Height;

						resolution = width + "x" + height;

						item->IsSelected = true;
						selectVideo = false;
					}
					else
					{
						item->IsSelected = false;
					}
				}
				else if (item->MediaType == PinMediaType::Audio)
				{
					audioStreamsCount++;

					item->IsSelected = false;
				}
				else if (item->MediaType == PinMediaType::Subtitle)
				{
					String ^ langName = ((PinSubtitleItem^)item)->LangName;
					if (langName == String::Empty || langName == nullptr)
						subtitlesStreams->Add((subtitlesStreams->Count + 1).ToString());
					else
						subtitlesStreams->Add(langName);

					item->IsSelected = false;
				}
			}

			if (thumbnailPath != nullptr)
			{
				//Generovanie thumbnail-u
				Int64 startTime = 60 * 1000; //Nastavi sa zaciatok na x sekund
				if (startTime > (encDuration - 1000)) //Rezerva je jedna sekunda - pri nastaveni na koniec sa neziska snimok
					startTime  = 0;

				enc->SetOutput(gcnew ContainerJPEG(280, 280, 1000 * 1000, thumbnailPath), startTime, 0);

				//Vytvorenie thumbnailu s obmedzenym casom na jeho ziskanie
				try { enc->StartEncode(3000); } catch (Exception ^) { }
			}

			return S_OK;
		}
		catch (DSWrapper::DSException ^ ex)
		{
			hr = ex->Result;
		}
		catch (Exception ^)
		{
			hr = E_FAIL;
		}
		finally
		{
			if (enc != nullptr) delete enc;
		}

		duration = TimeSpan::Zero;
		resolution = "0x0";
		audioStreamsCount = 0;
		subtitlesStreams = gcnew System::Collections::Generic::List<String^>();

		return hr;
	}

	HRESULT MediaFile::GetAudioDuration(IO::FileInfo ^ file, TimeSpan% duration, Int32% audioStreamsCount)
	{
		DSEncoder ^ enc = nullptr;
		HRESULT hr = S_OK;

		try
		{
			enc = gcnew DSEncoder();
			enc->SetInput(file->FullName, true);

			duration = TimeSpan::FromMilliseconds((Double)enc->GetDuration());
			audioStreamsCount = 0;

			for each(PinInfoItem ^ item in enc->SourcePins)
			{
				if (item->MediaType == PinMediaType::Audio)
				{
					audioStreamsCount++;
				}
			}

			return S_OK;
		}
		catch (DSWrapper::DSException ^ ex)
		{
			hr = ex->Result;
		}
		catch (Exception ^)
		{
			hr = E_FAIL;
		}
		finally
		{
			if (enc != nullptr) delete enc;
		}

		duration = TimeSpan::Zero;
		audioStreamsCount = 0;

		return hr;
	}

	Boolean MediaFile::GetImageResolution(IO::FileInfo ^ file, String ^% resolution)
	{
		String ^ width = nullptr, ^ height = nullptr;

		if (Environment::OSVersion->Version->Major < 6)
		{
			width = Regex::Match(GetValue(file, 27, String::Empty), "\\d+")->Value;
			height = Regex::Match(GetValue(file, 28, String::Empty), "\\d+")->Value;
		}
		else
		{
			array<String ^> ^ splitRes = GetValue(file, 31, String::Empty)->Split(gcnew array<Char> { 'x', 'X' }, StringSplitOptions::RemoveEmptyEntries);

			if (splitRes->Length == 2)
			{
				width = Regex::Match(splitRes[0], "\\d+")->Value;
				height = Regex::Match(splitRes[1], "\\d+")->Value;
			}
		}

		if (width == nullptr || width == String::Empty || height == nullptr || height == String::Empty)
		{
			resolution = "0x0";
			return false;
		}

		resolution = width + "x" + height;
		return true;
	}

	String ^ MediaFile::GetAudioArtist(IO::FileInfo ^ file)
	{
		if (Environment::OSVersion->Version->Major < 6)
			return GetValue(file, 16, String::Empty);

		return GetValue(file, 20, String::Empty);
	}

	String ^ MediaFile::GetAudioGenre(IO::FileInfo ^ file)
	{
		if (Environment::OSVersion->Version->Major < 6)
			return GetValue(file, 20, String::Empty);

		return GetValue(file, 16, String::Empty);
	}

	String ^ MediaFile::GetAudioAlbum(IO::FileInfo ^ file)
	{
		if (Environment::OSVersion->Version->Major < 6)
			return GetValue(file, 17, String::Empty);

		return GetValue(file, 14, String::Empty);
	}

	Boolean MediaFile::GetAudioThumbnail(IO::FileInfo ^ file, String ^ thumbnailPath)
	{
		HRESULT hr = S_OK;
		UCHAR bBuffer[4096];
		ULONG cbRead;
		IStream * stream = NULL;
		HANDLE hFile = NULL;
		VARIANT val;
		VariantInit(&val);

		//Hodnota v propkey.h
		CHECK_HR(hr = GetValue(file, L"{F29F85E0-4FF9-1068-AB91-08002B27B3D9} 27", &val));

		if (val.vt != VT_UNKNOWN)
		{
			hr = E_FAIL;
			goto done;
		}

		CHECK_HR(hr = val.punkVal->QueryInterface(IID_IStream, (void **)&stream));

		pin_ptr<const wchar_t> pPath = PtrToStringChars(thumbnailPath);
		hFile = CreateFile(pPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hr = E_FAIL;
			goto done;
		}

		while (SUCCEEDED(stream->Read(bBuffer, sizeof(bBuffer), &cbRead)) && cbRead> 0) 
		{ 
			WriteFile (hFile, bBuffer, cbRead, &cbRead, NULL); 
		}

	done:

		CloseHandle(hFile);
		SAFE_RELEASE(stream);
		VariantClear(&val);

		return hr == S_OK;
	}

	HRESULT MediaFile::GetValue(IO::FileInfo ^ file, OLECHAR * propName, VARIANT * vValue)
	{
		IntPtr fileNamePtr = IntPtr::Zero;
		IShellDispatch * shell = NULL;
		FolderItem2 * item2 = NULL;
		FolderItem * item = NULL;
		Folder * folder = NULL;
		BSTR bstrPropName = NULL;
		HRESULT hr = S_OK;
		
		VARIANT vDir;
		VariantInit(&vDir);

		CHECK_HR(hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&shell));

		pin_ptr<const wchar_t> dirName = PtrToStringChars(file->DirectoryName);
		vDir.vt = VT_BSTR;
		vDir.bstrVal = SysAllocStringLen(dirName, file->DirectoryName->Length);

		fileNamePtr = Runtime::InteropServices::Marshal::StringToBSTR(file->Name);
		BSTR fileNameStr = static_cast<BSTR>(fileNamePtr.ToPointer());
		//pin_ptr zabranuje presunu adresy GC
		pin_ptr<BSTR> b = &fileNameStr;

		CHECK_HR(hr = shell->NameSpace(vDir, &folder));
		CHECK_HR(hr = folder->ParseName(fileNameStr, &item));
		CHECK_HR(hr = item->QueryInterface(IID_FolderItem2, (void **)&item2));

		bstrPropName = SysAllocString(propName);
		CHECK_HR(hr = item2->ExtendedProperty(bstrPropName, vValue));
		
	done:

		Runtime::InteropServices::Marshal::FreeBSTR(fileNamePtr);

		SAFE_RELEASE(shell);
		SAFE_RELEASE(item2);
		SAFE_RELEASE(item);
		SAFE_RELEASE(folder);

		VariantClear(&vDir);

		SysFreeString(bstrPropName);

		return hr;
	}

	String ^ MediaFile::GetValue(IO::FileInfo ^ file, Int32 iValue, String ^ defaultValue)
	{
		IShellDispatch * shell = NULL;
		FolderItem * item = NULL;
		Folder * folder = NULL;
		VARIANT vDir, vFile;
		BSTR strVal = NULL, fileNameStr = NULL;
		HRESULT hr = S_OK;

		IntPtr fileNamePtr = IntPtr::Zero;
		String ^ retString = String::Empty;

		VariantInit(&vDir);
		VariantInit(&vFile);

		CHECK_HR(hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&shell));

		pin_ptr<const wchar_t> dirName = PtrToStringChars(file->DirectoryName);
		vDir.vt = VT_BSTR;
		vDir.bstrVal = SysAllocStringLen(dirName, file->DirectoryName->Length);

		fileNamePtr = Runtime::InteropServices::Marshal::StringToBSTR(file->Name);
		fileNameStr = static_cast<BSTR>(fileNamePtr.ToPointer());
		//pin_ptr zabranuje presunu adresy GC
		pin_ptr<BSTR> b = &fileNameStr;

		CHECK_HR(hr = shell->NameSpace(vDir, &folder));
		CHECK_HR(hr = folder->ParseName(fileNameStr, &item));

		vFile.vt = VT_DISPATCH;
		vFile.pdispVal = item;

		CHECK_HR(hr = folder->GetDetailsOf(vFile, iValue, &strVal));

		retString = Runtime::InteropServices::Marshal::PtrToStringBSTR((IntPtr)strVal);

	done:

		Runtime::InteropServices::Marshal::FreeBSTR(fileNamePtr);

		SysFreeString(strVal);

		SAFE_RELEASE(shell);
		SAFE_RELEASE(item);
		SAFE_RELEASE(folder);

		VariantClear(&vDir);

		return retString == String::Empty ? defaultValue : retString;
	}

	void MediaFile::PrintKeys(IO::FileInfo ^ file, Int32 maxKeys)
	{
		IShellDispatch * shell = NULL;
		Folder * folder = NULL;
		VARIANT vDir, vFile;
		BSTR strVal = NULL;
		HRESULT hr = S_OK;

		VariantInit(&vDir);
		VariantInit(&vFile);

		CHECK_HR(hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&shell));

		pin_ptr<const wchar_t> dirName = PtrToStringChars(file->DirectoryName);
		vDir.vt = VT_BSTR;
		vDir.bstrVal = SysAllocStringLen(dirName, file->DirectoryName->Length);

		CHECK_HR(hr = shell->NameSpace(vDir, &folder));
		
		for (int i = 0; i < maxKeys; i++)
		{
			CHECK_HR(hr = folder->GetDetailsOf(vFile, i, &strVal));

			String ^ str = Runtime::InteropServices::Marshal::PtrToStringBSTR((IntPtr)strVal);
			Console::WriteLine("{0} - {1}", i, str);

			SysFreeString(strVal);
			strVal = NULL;
		}

	done:

		SysFreeString(strVal);

		SAFE_RELEASE(shell);
		SAFE_RELEASE(folder);

		VariantClear(&vDir);
		VariantClear(&vFile);
	}
}