#pragma once

#ifndef __IVORBISENCODESETTINGS__
#define __IVORBISENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif

//#include "VorbisEncodeSettings.h"
class VorbisEncodeSettings;

DECLARE_INTERFACE_(IVorbisEncodeSettings, IUnknown) {

	//virtual STDMETHODIMP_(bool) setQuality(signed char inQuality) PURE;
	virtual STDMETHODIMP_(VorbisEncodeSettings) getEncoderSettings() PURE;
	

    virtual STDMETHODIMP_(bool) setQuality(int inQuality) PURE;
    virtual STDMETHODIMP_(bool) setBitrateQualityMode(int inBitrate) PURE;
    virtual STDMETHODIMP_(bool) setManaged(int inBitrate, int inMinBitrate, int inMaxBitrate) PURE;
};



#ifdef __cplusplus
}
#endif

#endif