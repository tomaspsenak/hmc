#pragma once

#if !defined(AUDIOSTREAM_MFWRAPPER_INCLUDED)
#define AUDIOSTREAM_MFWRAPPER_INCLUDED

namespace MFWrapper 
{
	public ref class AudioStream
	{
		public:		AudioStream(AudioFormat subtype, UINT32 numChannels, UINT32 sampleRate, UINT32 bitrate) : m_bitrate(bitrate), 
						m_numChannels(numChannels), m_sampleRate(sampleRate)
					{ 
						switch (subtype)
						{
							case AudioFormat::AAC: 		m_subtype = FromGUID(MFAudioFormat_AAC); break;
							case AudioFormat::WMAudioV8:m_subtype = FromGUID(MFAudioFormat_WMAudioV8); break;
							case AudioFormat::WMAudioV9:m_subtype = FromGUID(MFAudioFormat_WMAudioV9); break;
						}
					}

					property UINT32 Bitrate
					{
						UINT32 get() { return m_bitrate; }
					}

					property UINT32 SampleRate
					{
						UINT32 get() { return m_sampleRate; }
					}

					property UINT32 NumChannels
					{
						UINT32 get() { return m_numChannels; }
					}

					virtual System::String^ ToString(void) override
					{
						return System::String::Format(L"{0}CH - {1} Hz - {2} bps", m_numChannels, m_sampleRate, m_bitrate);
					}

		internal:	AudioStream(GUID subtype, UINT32 numChannels, UINT32 sampleRate, UINT32 bitrate) : 
						m_numChannels(numChannels), m_sampleRate(sampleRate), m_bitrate(bitrate)
					{
						m_subtype = FromGUID(subtype);
					}

					property GUID SubtypeInternal
					{
						GUID get() { return ToGUID(m_subtype); }
					}

					IMFMediaType * CreateMediaType(void)
					{
						HRESULT hr = S_OK;
						DWORD bestElement = 0;
						DWORD elementsCount = 0;
						UINT32 bestRatio = System::UInt32::MaxValue;

						IUnknown * unkAudioType = NULL;
						IMFMediaType * mediaType = NULL;
						IMFCollection * availableTypes = NULL;

						CHECK_HR(hr = MFTranscodeGetAudioOutputAvailableTypes(SubtypeInternal, MFT_ENUM_FLAG_ALL, NULL, &availableTypes));
						CHECK_HR(hr = availableTypes->GetElementCount(&elementsCount));

						for (DWORD i = 0; i < elementsCount; i++)
						{
							UINT32 actBitrate = 0;
							UINT32 actNumChannels = 0;
							UINT32 actSampleRate = 0;

							CHECK_HR(hr = availableTypes->GetElement(i, &unkAudioType));
							CHECK_HR(hr = unkAudioType->QueryInterface(IID_PPV_ARGS(&mediaType)));

							CHECK_HR(hr = mediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &actBitrate));
							actBitrate = abs((INT32)actBitrate - (INT32)this->m_bitrate);

							CHECK_HR(hr = mediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &actNumChannels));
							actNumChannels = abs((INT32)actNumChannels - (INT32)this->m_numChannels) * 10000;

							CHECK_HR(hr = mediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &actSampleRate));
							actSampleRate = abs((INT32)actSampleRate - (INT32)this->m_sampleRate);

							if (actBitrate + actNumChannels + actSampleRate < bestRatio)
							{
								bestRatio = actBitrate + actNumChannels + actSampleRate;
								bestElement = i;
							}

							SAFE_RELEASE(unkAudioType);
							SAFE_RELEASE(mediaType);
						}

						CHECK_HR(hr = availableTypes->GetElement(bestElement, &unkAudioType));
						CHECK_HR(hr = unkAudioType->QueryInterface(IID_PPV_ARGS(&mediaType)));

					done:

						SAFE_RELEASE(availableTypes);
						SAFE_RELEASE(unkAudioType);

						if (hr != S_OK)
							SAFE_RELEASE(mediaType);
						return mediaType;
					}

		private:	System::Guid m_subtype;
					UINT32 m_numChannels;
					UINT32 m_sampleRate;
					UINT32 m_bitrate;
	};
}

#endif //AUDIOSTREAM_MFWRAPPER_INCLUDED

