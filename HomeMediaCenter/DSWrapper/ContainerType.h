#pragma once

#if !defined(CONTAINERTYPE_DSWRAPPER_INCLUDED)
#define CONTAINERTYPE_DSWRAPPER_INCLUDED

#include "Enums.h"
#include "Extern/webmmuxidl.h"
#include "HMCEncoder_h.h"

namespace DSWrapper 
{
	public ref class ContainerType abstract
	{
		public:		static ContainerType ^ MPEG2_PS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
						UINT32 percentQuality, UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, 
						bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate);

					static ContainerType ^ MPEG2_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, 
						UINT32 percentQuality, UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath,
						bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate);

					static ContainerType ^ WEBM(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static ContainerType ^ WEBM_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static ContainerType ^ WMV(UINT32 width, UINT32 height, WMVideoSubtype videoSubtype, UINT32 vidBitrate, UINT32 percentQuality,
						UINT32 fps,	bool intSubtitles, System::String ^ intSubtitlesPath, UINT32 audBitrate);

					static ContainerType ^ AVI(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static ContainerType ^ MP4(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static ContainerType ^ MP3(BitrateMode bitrateMode, UINT32 audBitrate, UINT32 percentQuality);

					static ContainerType ^ MP3_TS(BitrateMode bitrateMode, UINT32 audBitrate, UINT32 percentQuality);

					static ContainerType ^ FLV(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static ContainerType ^ FLV_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate);

					static System::Boolean IsMPEG2Installed(void);
					static System::Boolean IsWEBMInstalled(void);
					static System::Boolean IsWMVInstalled(void);
					static System::Boolean IsFFDSHOWInstalled(void);

		internal:	virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin) = 0;

					virtual HRESULT GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter);

					virtual GUID GetSubtype() = 0;

					ContainerType(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps, bool intSubtitles, 
						System::String ^ intSubtitlesPath, bool keepAspectRatio) : m_width(width), m_height(height), m_bitrateMode(bitrateMode), 
						m_vidBitrate(vidBitrate), m_percentQuality(percentQuality), m_fps(fps), m_intSubtitles(intSubtitles), m_intSubtitlesPath(intSubtitlesPath),
						m_keepAspectRatio(keepAspectRatio) { }

		protected:	HRESULT ConfigureDecoder(IGraphBuilder * graphBuilder, IPin ** pVideoPin, IPin ** pAudioPin, IPin ** pSubtitlePin);
			
					UINT32 m_fps;
					UINT32 m_width;
					UINT32 m_height;
					UINT32 m_vidBitrate;
					UINT32 m_percentQuality;
					BitrateMode m_bitrateMode;
					bool m_intSubtitles;
					System::String ^ m_intSubtitlesPath;
					bool m_keepAspectRatio;
	};

	private ref class ContainerMPEG2_PS : ContainerType
	{
		internal:	ContainerMPEG2_PS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps,
						ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate) : 
						ContainerType(width, height, bitrateMode, vidBitrate, percentQuality, fps, intSubtitles, intSubtitlesPath, keepAspectRatio), 
						m_scanType(scanType), m_mpaLayer(mpaLayer), m_audBitrate(audBitrate) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin) override;

					virtual GUID GetSubtype() override { return MEDIASUBTYPE_MPEG2_PROGRAM; }

		protected:	ScanType m_scanType;
					MpaLayer m_mpaLayer;
					UINT32 m_audBitrate;
	};

	private ref class ContainerMPEG2_TS : ContainerMPEG2_PS
	{
		internal:	ContainerMPEG2_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps,
						ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, MpaLayer mpaLayer, UINT32 audBitrate) : 
						ContainerMPEG2_PS(width, height, bitrateMode, vidBitrate, percentQuality, fps, scanType, intSubtitles, intSubtitlesPath, keepAspectRatio, mpaLayer, audBitrate) { }

					virtual GUID GetSubtype() override { return MEDIASUBTYPE_MPEG2_TRANSPORT; }
	};

	private ref class ContainerWEBM : ContainerType
	{
		internal:	ContainerWEBM(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps, bool intSubtitles, 
						System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate, WebmMuxMode muxMode) : ContainerType(width, height, bitrateMode, vidBitrate, 
						percentQuality, fps, intSubtitles, intSubtitlesPath, keepAspectRatio), m_audBitrate(audBitrate), m_muxMode(muxMode) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin) override;

					virtual GUID GetSubtype() override;

		protected:	UINT32 m_audBitrate;
					WebmMuxMode m_muxMode;
	};

	private ref class ContainerWMV : ContainerType
	{
		internal:	ContainerWMV(UINT32 width, UINT32 height, WMVideoSubtype videoSubtype, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, UINT32 audBitrate) : ContainerType(width, height, BitrateMode::CBR, vidBitrate, 
						percentQuality, fps, intSubtitles, intSubtitlesPath, FALSE), m_audBitrate(audBitrate), m_videoSubtype(videoSubtype) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin) override;

					virtual HRESULT GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter) override;

					virtual GUID GetSubtype() override { return GUID_NULL; }

		protected:	UINT32 m_audBitrate;
					WMVideoSubtype m_videoSubtype;
	};

	private ref class ContainerHMC : ContainerType
	{
		internal:	ContainerHMC(enum Container container, bool streamable, UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, ScanType scanType, bool intSubtitles, System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate) : ContainerType(width, 
						height, bitrateMode, vidBitrate, percentQuality, fps, intSubtitles, intSubtitlesPath, keepAspectRatio), m_audBitrate(audBitrate), m_container(container), 
						m_streamable(streamable), m_scanType(scanType) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * subtitlePin, IPin * writerPin) override;

					virtual GUID GetSubtype() override { return GUID_NULL; }

		protected:	bool m_streamable;
					UINT32 m_audBitrate;
					ScanType m_scanType;
					enum Container m_container;
	};
}

#endif //CONTAINERTYPE_DSWRAPPER_INCLUDED
