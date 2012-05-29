#include "StdAfx.h"
#include "DesktopSilenceGenerator.h"
#include <Audioclient.h>
#include <avrt.h>

//SOURCE: http://blogs.msdn.com/b/matthew_van_eerde/archive/2008/12/10/sample-playing-silence-via-wasapi-event-driven-pull-mode.aspx

DWORD WINAPI PlaySilenceFunction(LPVOID pContext)
{
    PlaySilenceArgs * pArgs = (PlaySilenceArgs*)pContext;

	HRESULT hr = S_OK;
	DWORD dwWaitResult, nTaskIndex = 0;
	UINT32 nFramesInBuffer, nFramesOfPadding;
	BYTE * pData;

	WAVEFORMATEX * pwfx = NULL;
	IAudioClient * pAudioClient = NULL;
	IAudioRenderClient * pAudioRenderClient = NULL;
	HANDLE hTask, waitArray[2] = { pArgs->stopEvent, NULL };
	waitArray[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (waitArray[1] == NULL)
        CHECK_HR(hr = E_FAIL);

    CHECK_HR(hr = CoInitialize(NULL));

    CHECK_HR(hr = pArgs->device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient));

    CHECK_HR(hr = pAudioClient->GetMixFormat(&pwfx));

    CHECK_HR(hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, pwfx, NULL));
    
    CHECK_HR(hr = pAudioClient->GetBufferSize(&nFramesInBuffer));

    CHECK_HR(hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pAudioRenderClient));

    CHECK_HR(hr = pAudioClient->SetEventHandle(waitArray[1]));
    
    CHECK_HR(hr = pAudioRenderClient->GetBuffer(nFramesInBuffer, &pData));

    CHECK_HR(hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer, AUDCLNT_BUFFERFLAGS_SILENT));

    hTask = AvSetMmThreadCharacteristics(L"Playback", &nTaskIndex);
    if (NULL == hTask)
		CHECK_HR(hr = E_FAIL);

    CHECK_HR(hr = pAudioClient->Start());

    SetEvent(pArgs->startedEvent);

    while (true)
	{
        dwWaitResult = WaitForMultipleObjects(ARRAYSIZE(waitArray), waitArray, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == dwWaitResult)
		{
            break;
        }

        if (WAIT_OBJECT_0 + 1 != dwWaitResult)
		{
            CHECK_HR(hr = E_UNEXPECTED);
        }

        // got "feed me" event - see how much padding there is
        //
        // padding is how much of the buffer is currently in use
        //
        // note in particular that event-driven (pull-mode) render should not
        // call GetCurrentPadding multiple times
        // in a single processing pass
        // this is in stark contrast to timer-driven (push-mode) render
        
        CHECK_HR(hr = pAudioClient->GetCurrentPadding(&nFramesOfPadding));

        if (nFramesOfPadding == nFramesInBuffer)
		{
			continue;
        }
    
        CHECK_HR(hr = pAudioRenderClient->GetBuffer(nFramesInBuffer - nFramesOfPadding, &pData));

        // *** AT THIS POINT ***
        // If you wanted to render something besides silence,
        // you would fill the buffer pData
        // with (nFramesInBuffer - nFramesOfPadding) worth of audio data
        // this should be in the same wave format
        // that the stream was initialized with
        //
        // In particular, if you didn't want to use the mix format,
        // you would need to either ask for a different format in IAudioClient::Initialize
        // or do a format conversion
        //
        // If you do, then change the AUDCLNT_BUFFERFLAGS_SILENT flags value below to 0

        CHECK_HR(hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer - nFramesOfPadding, AUDCLNT_BUFFERFLAGS_SILENT));     
    }

done:

	if (pAudioClient != NULL)
	{
		pAudioClient->Stop();
		SAFE_RELEASE(pAudioClient);
	}
	SAFE_RELEASE(pAudioRenderClient);
    AvRevertMmThreadCharacteristics(hTask);
    CloseHandle(waitArray[1]);
	if (pwfx != NULL)
		CoTaskMemFree(pwfx);

	CoUninitialize();

	pArgs->hr = hr;
    return 0;
}