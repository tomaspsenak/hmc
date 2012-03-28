#ifndef _IFFDECODER_H_
#define _IFFDECODER_H_

#ifdef __cplusplus
extern "C" {
#endif

// {00F99063-70D5-4bcc-9D88-3801F3E3881B}
DEFINE_GUID(IID_IffDecoder, 0x00f99063, 0x70d5, 0x4bcc, 0x9d, 0x88, 0x38, 0x01, 0xf3, 0xe3, 0x88, 0x1b);
// {04FE9017-F873-410e-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW , 0x04fe9017, 0xf873, 0x410e, 0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7);

#define FFDSHOW_NAME_L  L"ffdshow MPEG-4 Video Decoder"

#define IDFF_autoPreset            1  //automatic preset loading enabled
#define IDFF_lastPage              2  //last active page in configuration dialog
#define IDFF_dlgRestorePos         9
#define IDFF_dlgPosX              10
#define IDFF_dlgPosY              11
#define IDFF_lvWidth0             12
#define IDFF_showMV                8
#define IDFF_trayIcon              3  //is tray icon visible
#define IDFF_cfgDlgHnwd            4  //handle to configuration dialog
#define IDFF_autoPresetFileFirst   5  //try to load preset from file 
#define IDFF_autoLoadedFromFile    6
#define IDFF_inPlayer              7

#define IDFF_isPostproc          106  
#define IDFF_orderPostproc       109
#define IDFF_ppqual              101  //postprocessing quality set by user (active when not autoq)
#define IDFF_autoq               102  //is automatic postprocessing control enabled?
#define IDFF_ppIsCustom          103  //custom postprocessing settings are enabled
#define IDFF_ppcustom            104  //custom postprocessing settings
#define IDFF_currentq            105
#define IDFF_deblockStrength     110
#define IDFF_levelFixLum         107
#define IDFF_levelFixChrom       108

#define IDFF_isPictProp          205 
#define IDFF_orderPictProp       207
#define IDFF_lumGain             201  //luminance gain
#define IDFF_lumOffset           202  //luminance offset
#define IDFF_gammaCorrection     206  //gamma correction (*100)
#define IDFF_hue                 203  //hue 
#define IDFF_saturation          204  //saturation
#define IDFF_lumGainDef          208
#define IDFF_lumOffsetDef        209
#define IDFF_gammaCorrectionDef  210
#define IDFF_hueDef              211
#define IDFF_saturationDef       212

#define IDFF_flip                301 //should output video be flipped?

#define IDFF_isSharpen           401 //is xshapen filter active?
#define IDFF_orderSharpen        407
#define IDFF_sharpenMethod       406 //0 - xsharpen, 1 - unsharp
#define IDFF_xsharp_strength     402 //xsharpen filter strength
#define IDFF_xsharp_threshold    403 //xsharpen filter threshold
#define IDFF_unsharp_strength    404 //xsharpen filter strength
#define IDFF_unsharp_threshold   405 //xsharpen filter threshold
 
#define IDFF_isNoise             501 //is noising filter active?
#define IDFF_orderNoise          506
#define IDFF_noiseMethod         505 //0 - my noise, 1 - avih noise
#define IDFF_uniformNoise        502 //is uniform noise active (applies only to luma noise now)?
#define IDFF_noiseStrength       503 //luma noise strength
#define IDFF_noiseStrengthChroma 504 //chroma noise strength
 
#define IDFF_idct                601 //IDCT function user (0 = libavcodec simple 16383, 1 = libavcodec normal, 2 = reference, 3 = libavcodec simple 16384)

#define IDFF_isResize            701 //is resizing active (or will be resizing active)
#define IDFF_resizeDx            702 //new width
#define IDFF_resizeDy            705 //new height, 5?
#define IDFF_resizeAspect        704 //0 - no aspect ratio correctio, 1 - keep original aspect, 2 - aspect ration is set in IDFF_aspectRatio
#define IDFF_resizeIsDy0         703 //0 - horizontal and vertical, 1 - horizontal, 3?
#define IDFF_resizeMethod        706
#define IDFF_aspectRatio         707 //aspect ratio (<<16)
#define IDFF_resizeGblurLum      708 // *100
#define IDFF_resizeGblurChrom    709 // *100
#define IDFF_resizeSharpenLum    710 // *100
#define IDFF_resizeSharpenChrom  711 // *100
#define IDFF_isCropNzoom         712
#define IDFF_isZoom              713
#define IDFF_magnificationX      714 
#define IDFF_cropLeft            715
#define IDFF_cropRight           716
#define IDFF_cropTop             717
#define IDFF_cropBottom          718
#define IDFF_autocrop            719
#define IDFF_magnificationY      720
#define IDFF_magnificationLocked 721

#define IDFF_isSubtitles         801
#define IDFF_orderSubtitles      815
#define IDFF_fontCharset         802
#define IDFF_fontSize            803
#define IDFF_fontWeight          804
#define IDFF_fontShadowStrength  805  //shadow strength (0..100) 100 - subtitles aren't transparent
#define IDFF_fontShadowRadius    806  //shadow radius  
#define IDFF_fontSpacing         808
#define IDFF_fontColor           809
#define IDFF_subPosX             810
#define IDFF_subPosY             811
#define IDFF_subDelay            812
#define IDFF_subSpeed            813
#define IDFF_subAutoFlnm         814

#define IDFF_isBlur              901
#define IDFF_orderBlur           903
#define IDFF_blurStrength        902
#define IDFF_tempSmooth          904

#define IDFF_isOffset           1101
#define IDFF_orderOffset        1102
#define IDFF_offsetY_X          1103
#define IDFF_offsetY_Y          1104
#define IDFF_offsetU_X          1105
#define IDFF_offsetU_Y          1106
#define IDFF_offsetV_X          1107
#define IDFF_offsetV_Y          1108
              
#define IDFF_xvid               1001 //are AVIs with this FOURCC played by ffdshow?
#define IDFF_div3               1002
#define IDFF_divx               1003
#define IDFF_dx50               1004
#define IDFF_mp43               1005
#define IDFF_mp42               1006
#define IDFF_mp41               1007
#define IDFF_h263               1008
#define IDFF_rawv               1009 

struct TpresetSettings;
class Tpresets;
DECLARE_INTERFACE_(IffDecoder, IUnknown)
{
 STDMETHOD (getParam)(unsigned int paramID, int* value) PURE;
 STDMETHOD (getParam2)(unsigned int paramID) PURE;
 STDMETHOD (putParam)(unsigned int paramID, int  value) PURE;
 STDMETHOD (getNumPresets)(unsigned int *value) PURE;
 STDMETHOD (getPresetName)(unsigned int i,char *buf,unsigned int len) PURE;
 STDMETHOD (getActivePresetName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setActivePreset)(const char *name) PURE;
 STDMETHOD (getAVIname)(char *buf,unsigned int len) PURE;
 STDMETHOD (getAVIfourcc)(char *buf,unsigned int len) PURE;
 STDMETHOD (getAVIdimensions)(unsigned int *x,unsigned int *y) PURE;
 STDMETHOD (getAVIfps)(unsigned int *fps) PURE;
 STDMETHOD (saveActivePreset)(const char *name) PURE;
 STDMETHOD (saveActivePresetToFile)(const char *flnm) PURE;
 STDMETHOD (loadActivePresetFromFile)(const char *flnm) PURE;
 STDMETHOD (removePreset)(const char *name) PURE;
 STDMETHOD (notifyParamsChanged)(void) PURE;
 STDMETHOD (getAVcodecVersion)(char *buf,unsigned int len) PURE;
 STDMETHOD (getPPmode)(unsigned int *ppmode) PURE;
 STDMETHOD (getFontName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setFontName)(const char *name) PURE;
 STDMETHOD (getSubFlnm)(char *buf,unsigned int len) PURE;
 STDMETHOD (loadSubtitles)(const char *flnm) PURE;
 STDMETHOD (getRealCrop)(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom) PURE;
 STDMETHOD (getMinOrder2)(void) PURE;
 STDMETHOD (getMaxOrder2)(void) PURE;
 STDMETHOD (saveGlobalSettings)(void) PURE;
 STDMETHOD (loadGlobalSettings)(void) PURE;
 STDMETHOD (saveDialogSettings)(void) PURE;
 STDMETHOD (loadDialogSettings)(void) PURE;
 STDMETHOD (getPresets)(Tpresets *presets2) PURE;
 STDMETHOD (setPresets)(const Tpresets *presets2) PURE;
 STDMETHOD (savePresets)(void) PURE;
 STDMETHOD (setPresetPtr)(TpresetSettings *preset) PURE;
 STDMETHOD (renameActivePreset)(const char *newName) PURE;
 STDMETHOD (setOnChangeMsg)(HWND wnd,unsigned int msg) PURE;
 STDMETHOD (setOnInfoMsg)(HWND wnd,unsigned int msg) PURE;
 STDMETHOD (getDefaultPresetName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setDefaultPresetName)(const char *presetName) PURE;
 STDMETHOD (isDefaultPreset)(const char *presetName) PURE;
 STDMETHOD (showCfgDlg)(HWND owner) PURE;
 STDMETHOD (getXvidVersion)(char *buf,unsigned int len) PURE;
};

#ifdef __cplusplus
}
#endif

#endif 
