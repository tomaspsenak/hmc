#include "StdAfx.h"
#include "DesktopSourceSilenceGenerator.h"
#include <Audioclient.h>
#include <avrt.h>

DesktopSourceSilenceGenerator * DesktopSourceSilenceGenerator::g_singleton = NULL;

DesktopSourceSilenceGenerator::DesktopSourceSilenceGenerator(void) : m_startedEvent(NULL), m_stopEvent(NULL), m_thread(NULL), m_hr(E_FAIL), m_count(0)
{
}

DesktopSourceSilenceGenerator::~DesktopSourceSilenceGenerator(void)
{
}

DesktopSourceSilenceGenerator::DesktopSourceSilenceGenerator(const DesktopSourceSilenceGenerator & obj)
{
	g_singleton = obj.g_singleton;
}

DesktopSourceSilenceGenerator & DesktopSourceSilenceGenerator::operator = (const DesktopSourceSilenceGenerator & obj)
{
	if (this != &obj)
		g_singleton = obj.g_singleton;
	
	return *this;
}

HRESULT DesktopSourceSilenceGenerator::Start(IMMDevice * device)
{
	DWORD waitResult;
	HANDLE waitArray[2];

	CAutoLock cAutoLock(&this->m_mainLock);

	if (this->m_count == 0)
	{
		//Nastavenie argumentov a spustenie vlakna generujuceho ticho - koli kontinualnemu zaznamu cez WASAPI
		this->m_hr = S_OK;
		this->m_device = device;

		if ((this->m_startedEvent = waitArray[0] = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		{
			return E_FAIL;
		}

		if ((this->m_stopEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		{
			CloseHandle(this->m_startedEvent);
			this->m_startedEvent = NULL;
			return E_FAIL;
		}

		if ((this->m_thread = waitArray[1] = CreateThread(NULL, 0, PlaySilenceFunction, this, 0, NULL)) == NULL)
		{
			CloseHandle(this->m_startedEvent);
			CloseHandle(this->m_stopEvent);
			this->m_startedEvent = NULL;
			this->m_stopEvent = NULL;
			return E_FAIL;
		}

		//Pocka sa, kym sa spusti alebo ukonci vlakno generujuce ticho
		waitResult = WaitForMultipleObjects(ARRAYSIZE(waitArray), waitArray, FALSE, INFINITE);
		if (waitResult != WAIT_OBJECT_0)
		{
			CloseHandle(this->m_startedEvent);
			CloseHandle(this->m_stopEvent);
			this->m_startedEvent = NULL;
			this->m_stopEvent = NULL;
			return E_FAIL;
		}
	}

	this->m_count++;

	return S_OK;
}

void DesktopSourceSilenceGenerator::Stop(void)
{
	CAutoLock cAutoLock(&this->m_mainLock);

	this->m_count--;
	if (this->m_count == 0)
	{
		//Pockanie na ukoncenie vlakna generujuceho ticho
		if (this->m_stopEvent != NULL)
			SetEvent(this->m_stopEvent);
		if (this->m_thread != NULL)
		{
			WaitForSingleObject(this->m_thread, INFINITE);
			CloseHandle(this->m_thread);
		}

		//Uzavrenie premennych typu HANDLE a nastavenie na NULL
		if (this->m_startedEvent != NULL)
			CloseHandle(this->m_startedEvent);

		if (this->m_stopEvent != NULL)
			CloseHandle(this->m_stopEvent);

		this->m_thread = NULL;
		this->m_stopEvent = NULL;
		this->m_startedEvent = NULL;
		this->m_hr = E_FAIL;
	}
}

void DesktopSourceSilenceGenerator::Init(void)
{
	if (!g_singleton)
		g_singleton = new DesktopSourceSilenceGenerator();
}

//SOURCE: http://blogs.msdn.com/b/matthew_van_eerde/archive/2008/12/10/sample-playing-silence-via-wasapi-event-driven-pull-mode.aspx

DWORD WINAPI DesktopSourceSilenceGenerator::PlaySilenceFunction(LPVOID pContext)
{
    DesktopSourceSilenceGenerator * pArgs = (DesktopSourceSilenceGenerator *)pContext;

	HRESULT hr = S_OK;
	DWORD dwWaitResult, nTaskIndex = 0;
	UINT32 nFramesInBuffer, nFramesOfPadding;
	BYTE * pData;

	WAVEFORMATEX * pwfx = NULL;
	IAudioClient * pAudioClient = NULL;
	IAudioRenderClient * pAudioRenderClient = NULL;
	HANDLE hTask, waitArray[2] = { pArgs->m_stopEvent, NULL };
	waitArray[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (waitArray[1] == NULL)
        CHECK_HR(hr = E_FAIL);

    CHECK_HR(hr = CoInitialize(NULL));

    CHECK_HR(hr = pArgs->m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient));

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

    SetEvent(pArgs->m_startedEvent);

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

	pArgs->m_hr = hr;
    return 0;
}
