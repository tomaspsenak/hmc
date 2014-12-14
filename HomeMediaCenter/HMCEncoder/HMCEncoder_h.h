

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Nov 14 21:05:09 2014
 */
/* Compiler settings for HMCEncoder.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __HMCEncoder_h_h__
#define __HMCEncoder_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IHMCEncoder_FWD_DEFINED__
#define __IHMCEncoder_FWD_DEFINED__
typedef interface IHMCEncoder IHMCEncoder;
#endif 	/* __IHMCEncoder_FWD_DEFINED__ */


#ifndef __HMCEncoder_FWD_DEFINED__
#define __HMCEncoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCEncoder HMCEncoder;
#else
typedef struct HMCEncoder HMCEncoder;
#endif /* __cplusplus */

#endif 	/* __HMCEncoder_FWD_DEFINED__ */


#ifndef __HMCEncoderPage_FWD_DEFINED__
#define __HMCEncoderPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCEncoderPage HMCEncoderPage;
#else
typedef struct HMCEncoderPage HMCEncoderPage;
#endif /* __cplusplus */

#endif 	/* __HMCEncoderPage_FWD_DEFINED__ */


#ifndef __HMCEncoderLogPage_FWD_DEFINED__
#define __HMCEncoderLogPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCEncoderLogPage HMCEncoderLogPage;
#else
typedef struct HMCEncoderLogPage HMCEncoderLogPage;
#endif /* __cplusplus */

#endif 	/* __HMCEncoderLogPage_FWD_DEFINED__ */


#ifndef __IHMCFrameRate_FWD_DEFINED__
#define __IHMCFrameRate_FWD_DEFINED__
typedef interface IHMCFrameRate IHMCFrameRate;
#endif 	/* __IHMCFrameRate_FWD_DEFINED__ */


#ifndef __HMCFrameRate_FWD_DEFINED__
#define __HMCFrameRate_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCFrameRate HMCFrameRate;
#else
typedef struct HMCFrameRate HMCFrameRate;
#endif /* __cplusplus */

#endif 	/* __HMCFrameRate_FWD_DEFINED__ */


#ifndef __HMCFrameRatePage_FWD_DEFINED__
#define __HMCFrameRatePage_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCFrameRatePage HMCFrameRatePage;
#else
typedef struct HMCFrameRatePage HMCFrameRatePage;
#endif /* __cplusplus */

#endif 	/* __HMCFrameRatePage_FWD_DEFINED__ */


#ifndef __IHMCDesktopSource_FWD_DEFINED__
#define __IHMCDesktopSource_FWD_DEFINED__
typedef interface IHMCDesktopSource IHMCDesktopSource;
#endif 	/* __IHMCDesktopSource_FWD_DEFINED__ */


#ifndef __HMCDesktopSource_FWD_DEFINED__
#define __HMCDesktopSource_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCDesktopSource HMCDesktopSource;
#else
typedef struct HMCDesktopSource HMCDesktopSource;
#endif /* __cplusplus */

#endif 	/* __HMCDesktopSource_FWD_DEFINED__ */


#ifndef __HMCDesktopSourcePage_FWD_DEFINED__
#define __HMCDesktopSourcePage_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCDesktopSourcePage HMCDesktopSourcePage;
#else
typedef struct HMCDesktopSourcePage HMCDesktopSourcePage;
#endif /* __cplusplus */

#endif 	/* __HMCDesktopSourcePage_FWD_DEFINED__ */


#ifndef __IHMCFrameWriter_FWD_DEFINED__
#define __IHMCFrameWriter_FWD_DEFINED__
typedef interface IHMCFrameWriter IHMCFrameWriter;
#endif 	/* __IHMCFrameWriter_FWD_DEFINED__ */


#ifndef __HMCFrameWriter_FWD_DEFINED__
#define __HMCFrameWriter_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCFrameWriter HMCFrameWriter;
#else
typedef struct HMCFrameWriter HMCFrameWriter;
#endif /* __cplusplus */

#endif 	/* __HMCFrameWriter_FWD_DEFINED__ */


#ifndef __HMCFrameWriterPage_FWD_DEFINED__
#define __HMCFrameWriterPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class HMCFrameWriterPage HMCFrameWriterPage;
#else
typedef struct HMCFrameWriterPage HMCFrameWriterPage;
#endif /* __cplusplus */

#endif 	/* __HMCFrameWriterPage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __HMCEncoderLib_LIBRARY_DEFINED__
#define __HMCEncoderLib_LIBRARY_DEFINED__

/* library HMCEncoderLib */
/* [version][helpstring][uuid] */ 


enum Container
    {	Container_MPEG2PS	= 0,
	Container_MPEG2TS	= ( Container_MPEG2PS + 1 ) ,
	Container_MPEG2TSH264	= ( Container_MPEG2TS + 1 ) ,
	Container_MP3	= ( Container_MPEG2TSH264 + 1 ) ,
	Container_MP4	= ( Container_MP3 + 1 ) ,
	Container_AVI	= ( Container_MP4 + 1 ) ,
	Container_FLV	= ( Container_AVI + 1 ) ,
	Container_FLVH264	= ( Container_FLV + 1 ) ,
	Container_ASF	= ( Container_FLVH264 + 1 ) ,
	Container_WEBM	= ( Container_ASF + 1 ) 
    } ;

enum BitrateMode
    {	BitrateMode_CBR	= 0,
	BitrateMode_VBR	= ( BitrateMode_CBR + 1 ) 
    } ;

enum ImageFormat
    {	ImageFormat_BMP	= 0,
	ImageFormat_JPEG	= ( ImageFormat_BMP + 1 ) ,
	ImageFormat_PNG	= ( ImageFormat_JPEG + 1 ) 
    } ;

EXTERN_C const IID LIBID_HMCEncoderLib;

#ifndef __IHMCEncoder_INTERFACE_DEFINED__
#define __IHMCEncoder_INTERFACE_DEFINED__

/* interface IHMCEncoder */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHMCEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2FDD6C62-CA3B-4D2A-A92C-107EA551F42D")
    IHMCEncoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetContainer( 
            /* [in] */ enum Container container) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainer( 
            /* [out] */ enum Container *pContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStreamable( 
            /* [in] */ BOOL streamable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStreamable( 
            /* [out] */ BOOL *pStreamable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAudioCBR( 
            /* [in] */ int audioBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAudioVBR( 
            /* [in] */ int audioBitrate,
            /* [in] */ int audioBitrateMax,
            /* [in] */ int audioBitrateMin,
            /* [in] */ int audioQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioBitrate( 
            /* [out] */ BOOL *pIsConstant,
            /* [out] */ int *pAudioBitrate,
            /* [out] */ int *pAudioBitrateMax,
            /* [out] */ int *pAudioBitrateMin,
            /* [out] */ int *pAudioQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoCBR( 
            /* [in] */ int videoBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoVBR( 
            /* [in] */ int videoBitrate,
            /* [in] */ int videoBitrateMax,
            /* [in] */ int videoBitrateMin,
            /* [in] */ int videoQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoBitrate( 
            /* [out] */ BOOL *pIsConstant,
            /* [out] */ int *pVideoBitrate,
            /* [out] */ int *pVideoBitrateMax,
            /* [out] */ int *pVideoBitrateMin,
            /* [out] */ int *pVideoQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoInterlace( 
            /* [in] */ BOOL interlaced) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoInterlace( 
            /* [out] */ BOOL *interlaced) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ UINT32 width) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ UINT32 *pWidth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeight( 
            /* [in] */ UINT32 height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [out] */ UINT32 *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoQuality( 
            /* [in] */ int videoQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoQuality( 
            /* [out] */ int *pVideoQuality) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHMCEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHMCEncoder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHMCEncoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHMCEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetContainer )( 
            IHMCEncoder * This,
            /* [in] */ enum Container container);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IHMCEncoder * This,
            /* [out] */ enum Container *pContainer);
        
        HRESULT ( STDMETHODCALLTYPE *SetStreamable )( 
            IHMCEncoder * This,
            /* [in] */ BOOL streamable);
        
        HRESULT ( STDMETHODCALLTYPE *GetStreamable )( 
            IHMCEncoder * This,
            /* [out] */ BOOL *pStreamable);
        
        HRESULT ( STDMETHODCALLTYPE *SetAudioCBR )( 
            IHMCEncoder * This,
            /* [in] */ int audioBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetAudioVBR )( 
            IHMCEncoder * This,
            /* [in] */ int audioBitrate,
            /* [in] */ int audioBitrateMax,
            /* [in] */ int audioBitrateMin,
            /* [in] */ int audioQuality);
        
        HRESULT ( STDMETHODCALLTYPE *GetAudioBitrate )( 
            IHMCEncoder * This,
            /* [out] */ BOOL *pIsConstant,
            /* [out] */ int *pAudioBitrate,
            /* [out] */ int *pAudioBitrateMax,
            /* [out] */ int *pAudioBitrateMin,
            /* [out] */ int *pAudioQuality);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoCBR )( 
            IHMCEncoder * This,
            /* [in] */ int videoBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoVBR )( 
            IHMCEncoder * This,
            /* [in] */ int videoBitrate,
            /* [in] */ int videoBitrateMax,
            /* [in] */ int videoBitrateMin,
            /* [in] */ int videoQuality);
        
        HRESULT ( STDMETHODCALLTYPE *GetVideoBitrate )( 
            IHMCEncoder * This,
            /* [out] */ BOOL *pIsConstant,
            /* [out] */ int *pVideoBitrate,
            /* [out] */ int *pVideoBitrateMax,
            /* [out] */ int *pVideoBitrateMin,
            /* [out] */ int *pVideoQuality);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoInterlace )( 
            IHMCEncoder * This,
            /* [in] */ BOOL interlaced);
        
        HRESULT ( STDMETHODCALLTYPE *GetVideoInterlace )( 
            IHMCEncoder * This,
            /* [out] */ BOOL *interlaced);
        
        HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IHMCEncoder * This,
            /* [in] */ UINT32 width);
        
        HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IHMCEncoder * This,
            /* [out] */ UINT32 *pWidth);
        
        HRESULT ( STDMETHODCALLTYPE *SetHeight )( 
            IHMCEncoder * This,
            /* [in] */ UINT32 height);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeight )( 
            IHMCEncoder * This,
            /* [out] */ UINT32 *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoQuality )( 
            IHMCEncoder * This,
            /* [in] */ int videoQuality);
        
        HRESULT ( STDMETHODCALLTYPE *GetVideoQuality )( 
            IHMCEncoder * This,
            /* [out] */ int *pVideoQuality);
        
        END_INTERFACE
    } IHMCEncoderVtbl;

    interface IHMCEncoder
    {
        CONST_VTBL struct IHMCEncoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHMCEncoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHMCEncoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHMCEncoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHMCEncoder_SetContainer(This,container)	\
    ( (This)->lpVtbl -> SetContainer(This,container) ) 

#define IHMCEncoder_GetContainer(This,pContainer)	\
    ( (This)->lpVtbl -> GetContainer(This,pContainer) ) 

#define IHMCEncoder_SetStreamable(This,streamable)	\
    ( (This)->lpVtbl -> SetStreamable(This,streamable) ) 

#define IHMCEncoder_GetStreamable(This,pStreamable)	\
    ( (This)->lpVtbl -> GetStreamable(This,pStreamable) ) 

#define IHMCEncoder_SetAudioCBR(This,audioBitrate)	\
    ( (This)->lpVtbl -> SetAudioCBR(This,audioBitrate) ) 

#define IHMCEncoder_SetAudioVBR(This,audioBitrate,audioBitrateMax,audioBitrateMin,audioQuality)	\
    ( (This)->lpVtbl -> SetAudioVBR(This,audioBitrate,audioBitrateMax,audioBitrateMin,audioQuality) ) 

#define IHMCEncoder_GetAudioBitrate(This,pIsConstant,pAudioBitrate,pAudioBitrateMax,pAudioBitrateMin,pAudioQuality)	\
    ( (This)->lpVtbl -> GetAudioBitrate(This,pIsConstant,pAudioBitrate,pAudioBitrateMax,pAudioBitrateMin,pAudioQuality) ) 

#define IHMCEncoder_SetVideoCBR(This,videoBitrate)	\
    ( (This)->lpVtbl -> SetVideoCBR(This,videoBitrate) ) 

#define IHMCEncoder_SetVideoVBR(This,videoBitrate,videoBitrateMax,videoBitrateMin,videoQuality)	\
    ( (This)->lpVtbl -> SetVideoVBR(This,videoBitrate,videoBitrateMax,videoBitrateMin,videoQuality) ) 

#define IHMCEncoder_GetVideoBitrate(This,pIsConstant,pVideoBitrate,pVideoBitrateMax,pVideoBitrateMin,pVideoQuality)	\
    ( (This)->lpVtbl -> GetVideoBitrate(This,pIsConstant,pVideoBitrate,pVideoBitrateMax,pVideoBitrateMin,pVideoQuality) ) 

#define IHMCEncoder_SetVideoInterlace(This,interlaced)	\
    ( (This)->lpVtbl -> SetVideoInterlace(This,interlaced) ) 

#define IHMCEncoder_GetVideoInterlace(This,interlaced)	\
    ( (This)->lpVtbl -> GetVideoInterlace(This,interlaced) ) 

#define IHMCEncoder_SetWidth(This,width)	\
    ( (This)->lpVtbl -> SetWidth(This,width) ) 

#define IHMCEncoder_GetWidth(This,pWidth)	\
    ( (This)->lpVtbl -> GetWidth(This,pWidth) ) 

#define IHMCEncoder_SetHeight(This,height)	\
    ( (This)->lpVtbl -> SetHeight(This,height) ) 

#define IHMCEncoder_GetHeight(This,pHeight)	\
    ( (This)->lpVtbl -> GetHeight(This,pHeight) ) 

#define IHMCEncoder_SetVideoQuality(This,videoQuality)	\
    ( (This)->lpVtbl -> SetVideoQuality(This,videoQuality) ) 

#define IHMCEncoder_GetVideoQuality(This,pVideoQuality)	\
    ( (This)->lpVtbl -> GetVideoQuality(This,pVideoQuality) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHMCEncoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HMCEncoder;

#ifdef __cplusplus

class DECLSPEC_UUID("2FDD6C62-CA3B-4D2A-A91C-107EA551F42D")
HMCEncoder;
#endif

EXTERN_C const CLSID CLSID_HMCEncoderPage;

#ifdef __cplusplus

class DECLSPEC_UUID("2FDD6C62-CA3B-4D2A-A93C-107EA551F42D")
HMCEncoderPage;
#endif

EXTERN_C const CLSID CLSID_HMCEncoderLogPage;

#ifdef __cplusplus

class DECLSPEC_UUID("2FDD6C62-CA3B-4D2A-A94C-107EA551F42D")
HMCEncoderLogPage;
#endif

#ifndef __IHMCFrameRate_INTERFACE_DEFINED__
#define __IHMCFrameRate_INTERFACE_DEFINED__

/* interface IHMCFrameRate */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHMCFrameRate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("89F5B7A4-F4DE-48A6-BB2C-D8AD772D177B")
    IHMCFrameRate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFrameRate( 
            /* [in] */ UINT32 fps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameRate( 
            /* [out] */ UINT32 *pFps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHMCFrameRateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHMCFrameRate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHMCFrameRate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHMCFrameRate * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFrameRate )( 
            IHMCFrameRate * This,
            /* [in] */ UINT32 fps);
        
        HRESULT ( STDMETHODCALLTYPE *GetFrameRate )( 
            IHMCFrameRate * This,
            /* [out] */ UINT32 *pFps);
        
        END_INTERFACE
    } IHMCFrameRateVtbl;

    interface IHMCFrameRate
    {
        CONST_VTBL struct IHMCFrameRateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHMCFrameRate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHMCFrameRate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHMCFrameRate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHMCFrameRate_SetFrameRate(This,fps)	\
    ( (This)->lpVtbl -> SetFrameRate(This,fps) ) 

#define IHMCFrameRate_GetFrameRate(This,pFps)	\
    ( (This)->lpVtbl -> GetFrameRate(This,pFps) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHMCFrameRate_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HMCFrameRate;

#ifdef __cplusplus

class DECLSPEC_UUID("89F5B7A4-F4DE-48A6-BB1C-D8AD772D177B")
HMCFrameRate;
#endif

EXTERN_C const CLSID CLSID_HMCFrameRatePage;

#ifdef __cplusplus

class DECLSPEC_UUID("89F5B7A4-F4DE-48A6-BB3C-D8AD772D177B")
HMCFrameRatePage;
#endif

#ifndef __IHMCDesktopSource_INTERFACE_DEFINED__
#define __IHMCDesktopSource_INTERFACE_DEFINED__

/* interface IHMCDesktopSource */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHMCDesktopSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3D7090C2-B04C-42F2-9324-3E7EE2BEEFE7")
    IHMCDesktopSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFrameRate( 
            /* [in] */ UINT32 fps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameRate( 
            /* [out] */ UINT32 *pFps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ UINT32 width) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ UINT32 *pWidth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeight( 
            /* [in] */ UINT32 height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [out] */ UINT32 *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoQuality( 
            /* [in] */ BYTE quality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoQuality( 
            /* [out] */ BYTE *pQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCaptureCursor( 
            /* [in] */ BOOL captureCursor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCaptureCursor( 
            /* [out] */ BOOL *pCaptureCursor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAspectRatio( 
            /* [in] */ BOOL keepAspectRatio) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAspectRatio( 
            /* [out] */ BOOL *pKeepAspectRatio) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHMCDesktopSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHMCDesktopSource * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHMCDesktopSource * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHMCDesktopSource * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFrameRate )( 
            IHMCDesktopSource * This,
            /* [in] */ UINT32 fps);
        
        HRESULT ( STDMETHODCALLTYPE *GetFrameRate )( 
            IHMCDesktopSource * This,
            /* [out] */ UINT32 *pFps);
        
        HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IHMCDesktopSource * This,
            /* [in] */ UINT32 width);
        
        HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IHMCDesktopSource * This,
            /* [out] */ UINT32 *pWidth);
        
        HRESULT ( STDMETHODCALLTYPE *SetHeight )( 
            IHMCDesktopSource * This,
            /* [in] */ UINT32 height);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeight )( 
            IHMCDesktopSource * This,
            /* [out] */ UINT32 *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoQuality )( 
            IHMCDesktopSource * This,
            /* [in] */ BYTE quality);
        
        HRESULT ( STDMETHODCALLTYPE *GetVideoQuality )( 
            IHMCDesktopSource * This,
            /* [out] */ BYTE *pQuality);
        
        HRESULT ( STDMETHODCALLTYPE *SetCaptureCursor )( 
            IHMCDesktopSource * This,
            /* [in] */ BOOL captureCursor);
        
        HRESULT ( STDMETHODCALLTYPE *GetCaptureCursor )( 
            IHMCDesktopSource * This,
            /* [out] */ BOOL *pCaptureCursor);
        
        HRESULT ( STDMETHODCALLTYPE *SetAspectRatio )( 
            IHMCDesktopSource * This,
            /* [in] */ BOOL keepAspectRatio);
        
        HRESULT ( STDMETHODCALLTYPE *GetAspectRatio )( 
            IHMCDesktopSource * This,
            /* [out] */ BOOL *pKeepAspectRatio);
        
        END_INTERFACE
    } IHMCDesktopSourceVtbl;

    interface IHMCDesktopSource
    {
        CONST_VTBL struct IHMCDesktopSourceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHMCDesktopSource_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHMCDesktopSource_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHMCDesktopSource_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHMCDesktopSource_SetFrameRate(This,fps)	\
    ( (This)->lpVtbl -> SetFrameRate(This,fps) ) 

#define IHMCDesktopSource_GetFrameRate(This,pFps)	\
    ( (This)->lpVtbl -> GetFrameRate(This,pFps) ) 

#define IHMCDesktopSource_SetWidth(This,width)	\
    ( (This)->lpVtbl -> SetWidth(This,width) ) 

#define IHMCDesktopSource_GetWidth(This,pWidth)	\
    ( (This)->lpVtbl -> GetWidth(This,pWidth) ) 

#define IHMCDesktopSource_SetHeight(This,height)	\
    ( (This)->lpVtbl -> SetHeight(This,height) ) 

#define IHMCDesktopSource_GetHeight(This,pHeight)	\
    ( (This)->lpVtbl -> GetHeight(This,pHeight) ) 

#define IHMCDesktopSource_SetVideoQuality(This,quality)	\
    ( (This)->lpVtbl -> SetVideoQuality(This,quality) ) 

#define IHMCDesktopSource_GetVideoQuality(This,pQuality)	\
    ( (This)->lpVtbl -> GetVideoQuality(This,pQuality) ) 

#define IHMCDesktopSource_SetCaptureCursor(This,captureCursor)	\
    ( (This)->lpVtbl -> SetCaptureCursor(This,captureCursor) ) 

#define IHMCDesktopSource_GetCaptureCursor(This,pCaptureCursor)	\
    ( (This)->lpVtbl -> GetCaptureCursor(This,pCaptureCursor) ) 

#define IHMCDesktopSource_SetAspectRatio(This,keepAspectRatio)	\
    ( (This)->lpVtbl -> SetAspectRatio(This,keepAspectRatio) ) 

#define IHMCDesktopSource_GetAspectRatio(This,pKeepAspectRatio)	\
    ( (This)->lpVtbl -> GetAspectRatio(This,pKeepAspectRatio) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHMCDesktopSource_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HMCDesktopSource;

#ifdef __cplusplus

class DECLSPEC_UUID("3D7090C2-B04C-42F2-9314-3E7EE2BEEFE7")
HMCDesktopSource;
#endif

EXTERN_C const CLSID CLSID_HMCDesktopSourcePage;

#ifdef __cplusplus

class DECLSPEC_UUID("3D7090C2-B04C-42F2-9334-3E7EE2BEEFE7")
HMCDesktopSourcePage;
#endif

#ifndef __IHMCFrameWriter_INTERFACE_DEFINED__
#define __IHMCFrameWriter_INTERFACE_DEFINED__

/* interface IHMCFrameWriter */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHMCFrameWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E6E91083-47A0-41CE-A721-611A62111F6A")
    IHMCFrameWriter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFormat( 
            /* [in] */ enum ImageFormat format) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFormat( 
            /* [out] */ enum ImageFormat *pFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ UINT32 width) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ UINT32 *pWidth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeight( 
            /* [in] */ UINT32 height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [out] */ UINT32 *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBitrate( 
            /* [in] */ int imageBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBitrate( 
            /* [out] */ int *pImageBitrate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHMCFrameWriterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHMCFrameWriter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHMCFrameWriter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHMCFrameWriter * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFormat )( 
            IHMCFrameWriter * This,
            /* [in] */ enum ImageFormat format);
        
        HRESULT ( STDMETHODCALLTYPE *GetFormat )( 
            IHMCFrameWriter * This,
            /* [out] */ enum ImageFormat *pFormat);
        
        HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IHMCFrameWriter * This,
            /* [in] */ UINT32 width);
        
        HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IHMCFrameWriter * This,
            /* [out] */ UINT32 *pWidth);
        
        HRESULT ( STDMETHODCALLTYPE *SetHeight )( 
            IHMCFrameWriter * This,
            /* [in] */ UINT32 height);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeight )( 
            IHMCFrameWriter * This,
            /* [out] */ UINT32 *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE *SetBitrate )( 
            IHMCFrameWriter * This,
            /* [in] */ int imageBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *GetBitrate )( 
            IHMCFrameWriter * This,
            /* [out] */ int *pImageBitrate);
        
        END_INTERFACE
    } IHMCFrameWriterVtbl;

    interface IHMCFrameWriter
    {
        CONST_VTBL struct IHMCFrameWriterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHMCFrameWriter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHMCFrameWriter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHMCFrameWriter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHMCFrameWriter_SetFormat(This,format)	\
    ( (This)->lpVtbl -> SetFormat(This,format) ) 

#define IHMCFrameWriter_GetFormat(This,pFormat)	\
    ( (This)->lpVtbl -> GetFormat(This,pFormat) ) 

#define IHMCFrameWriter_SetWidth(This,width)	\
    ( (This)->lpVtbl -> SetWidth(This,width) ) 

#define IHMCFrameWriter_GetWidth(This,pWidth)	\
    ( (This)->lpVtbl -> GetWidth(This,pWidth) ) 

#define IHMCFrameWriter_SetHeight(This,height)	\
    ( (This)->lpVtbl -> SetHeight(This,height) ) 

#define IHMCFrameWriter_GetHeight(This,pHeight)	\
    ( (This)->lpVtbl -> GetHeight(This,pHeight) ) 

#define IHMCFrameWriter_SetBitrate(This,imageBitrate)	\
    ( (This)->lpVtbl -> SetBitrate(This,imageBitrate) ) 

#define IHMCFrameWriter_GetBitrate(This,pImageBitrate)	\
    ( (This)->lpVtbl -> GetBitrate(This,pImageBitrate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHMCFrameWriter_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HMCFrameWriter;

#ifdef __cplusplus

class DECLSPEC_UUID("E6E91083-47A0-41CE-A711-611A62111F6A")
HMCFrameWriter;
#endif

EXTERN_C const CLSID CLSID_HMCFrameWriterPage;

#ifdef __cplusplus

class DECLSPEC_UUID("E6E91083-47A0-41CE-A731-611A62111F6A")
HMCFrameWriterPage;
#endif
#endif /* __HMCEncoderLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


