#pragma once

#if !defined(CONTAINERTYPE_DSWRAPPER_INCLUDED)
#define CONTAINERTYPE_DSWRAPPER_INCLUDED

#include "Enums.h"

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

					static System::Boolean IsMPEG2Installed(void);
					static System::Boolean IsWEBMInstalled(void);
					static System::Boolean IsWMVInstalled(void);
					static System::Boolean IsFFDSHOWInstalled(void);

		internal:	virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * writerPin) = 0;

					virtual HRESULT GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter);

					virtual GUID GetSubtype() = 0;

					ContainerType(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps, bool intSubtitles, 
						System::String ^ intSubtitlesPath, bool keepAspectRatio) : m_width(width), m_height(height), m_bitrateMode(bitrateMode), 
						m_vidBitrate(vidBitrate), m_percentQuality(percentQuality), m_fps(fps), m_intSubtitles(intSubtitles), m_intSubtitlesPath(intSubtitlesPath),
						m_keepAspectRatio(keepAspectRatio) { }

		protected:	HRESULT ConfigureDecoder(IGraphBuilder * graphBuilder, IPin ** pVideoPin, IPin ** pAudioPin);
			
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

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * writerPin) override;

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
						System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate) : ContainerType(width, height, bitrateMode, vidBitrate, percentQuality, 
						fps, intSubtitles, intSubtitlesPath, keepAspectRatio), m_audBitrate(audBitrate) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * writerPin) override;

					virtual GUID GetSubtype() override { return CLSID_WebmOut; }

		protected:	UINT32 m_audBitrate;			
	};

	private ref class ContainerWEBM_TS : ContainerWEBM
	{
		internal:	ContainerWEBM_TS(UINT32 width, UINT32 height, BitrateMode bitrateMode, UINT32 vidBitrate, UINT32 percentQuality, UINT32 fps, bool intSubtitles, 
						System::String ^ intSubtitlesPath, bool keepAspectRatio, UINT32 audBitrate) : ContainerWEBM(width, height, bitrateMode, vidBitrate, percentQuality, 
						fps, intSubtitles, intSubtitlesPath, keepAspectRatio, audBitrate) { }

					virtual HRESULT GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter) override;			
	};

	private ref class ContainerWMV : ContainerType
	{
		internal:	ContainerWMV(UINT32 width, UINT32 height, WMVideoSubtype videoSubtype, UINT32 vidBitrate, UINT32 percentQuality, 
						UINT32 fps, bool intSubtitles, System::String ^ intSubtitlesPath, UINT32 audBitrate) : ContainerType(width, height, BitrateMode::CBR, vidBitrate, 
						percentQuality, fps, intSubtitles, intSubtitlesPath, FALSE), m_audBitrate(audBitrate), m_videoSubtype(videoSubtype) { }

					virtual HRESULT ConfigureContainer(IGraphBuilder * graphBuilder, IPin * videoPin, IPin * audioPin, IPin * writerPin) override;

					virtual HRESULT GetWriter(System::IO::Stream ^ outputStream, IGraphBuilder * graphBuilder, IBaseFilter ** writerFilter) override;

					virtual GUID GetSubtype() override { return GUID_NULL; }

		protected:	UINT32 m_audBitrate;
					WMVideoSubtype m_videoSubtype;
	};
}

#endif //CONTAINERTYPE_DSWRAPPER_INCLUDED
