#include "StdAfx.h"
#include "MediaFile.h"
#include "DSWrapper.h"

namespace DSWrapper 
{
	MediaFile::MediaFile(void)
	{
	}

	Boolean MediaFile::GetVideoInfo(IO::FileInfo ^ file, TimeSpan% duration, String ^% resolution)
	{
		DSEncoder ^ enc = nullptr;
		try
		{
			enc = gcnew DSEncoder();
			enc->SetInput(file->FullName, true);

			duration = TimeSpan::FromMilliseconds((Double)enc->GetDuration());

			for each(PinInfoItem ^ item in enc->SourcePins)
			{
				if (item->MediaType == PinMediaType::Video)
				{
					LONG width = ((PinVideoItem ^)item)->Width;
					LONG height = ((PinVideoItem ^)item)->Height;

					resolution = width + "x" + height;
					break;
				}
			}

			return true;
		}
		catch (Exception ^)
		{
			duration = TimeSpan::Zero;
			resolution = "0x0";

			return false;
		}
		finally
		{
			if (enc != nullptr) delete enc;
		}
	}

	Boolean MediaFile::GetAudioDuration(IO::FileInfo ^ file, TimeSpan% duration)
	{
		if (Environment::OSVersion->Version->Major < 6)
			return TimeSpan::TryParse(GetValue(file, 21, String::Empty), duration);

		return TimeSpan::TryParse(GetValue(file, 27, String::Empty), duration);
	}

	String ^ MediaFile::GetImageResolution(IO::FileInfo ^ file)
	{
		String ^ width, ^ height;

		if (Environment::OSVersion->Version->Major < 6)
		{
			width = Regex::Match(GetValue(file, 27, "0"), "\\d+")->Value;
			height = Regex::Match(GetValue(file, 28, "0"), "\\d+")->Value;
		}
		else if (Environment::OSVersion->Version->Minor < 2)
		{
			width = Regex::Match(GetValue(file, 162, "0"), "\\d+")->Value;
			height = Regex::Match(GetValue(file, 164, "0"), "\\d+")->Value;
		}
		else
		{
			width = Regex::Match(GetValue(file, 167, "0"), "\\d+")->Value;
			height = Regex::Match(GetValue(file, 165, "0"), "\\d+")->Value;
		}

		return width + "x" + height;
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