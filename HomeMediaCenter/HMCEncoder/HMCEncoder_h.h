

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Nov 11 22:16:26 2013
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
	Container_ASF	= ( Container_FLV + 1 ) 
    } ;

enum BitrateMode
    {	BitrateMode_CBR	= 0,
	BitrateMode_VBR	= ( BitrateMode_CBR + 1 ) 
    } ;

EXTERN_C const IID LIBID_HMCEncoderLib;

#ifndef __IHMCEncoder_INTERFACE_DEFINED__
#define __IHMCEncoder_INTERFACE_DEFINED__

/* interface IHMCEncoder */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHMCEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2FDD6C62-CA3B-4d2a-A93C-107EA551F42D")
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
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoCBR( 
            /* [in] */ int videoBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoVBR( 
            /* [in] */ int videoBitrate,
            /* [in] */ int videoBitrateMax,
            /* [in] */ int videoBitrateMin,
            /* [in] */ int videoQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoInterlace( 
            /* [in] */ BOOL interlaced) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoInterlace( 
            /* [out] */ BOOL *interlaced) = 0;
        
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
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoCBR )( 
            IHMCEncoder * This,
            /* [in] */ int videoBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoVBR )( 
            IHMCEncoder * This,
            /* [in] */ int videoBitrate,
            /* [in] */ int videoBitrateMax,
            /* [in] */ int videoBitrateMin,
            /* [in] */ int videoQuality);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoInterlace )( 
            IHMCEncoder * This,
            /* [in] */ BOOL interlaced);
        
        HRESULT ( STDMETHODCALLTYPE *GetVideoInterlace )( 
            IHMCEncoder * This,
            /* [out] */ BOOL *interlaced);
        
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

#define IHMCEncoder_SetVideoCBR(This,videoBitrate)	\
    ( (This)->lpVtbl -> SetVideoCBR(This,videoBitrate) ) 

#define IHMCEncoder_SetVideoVBR(This,videoBitrate,videoBitrateMax,videoBitrateMin,videoQuality)	\
    ( (This)->lpVtbl -> SetVideoVBR(This,videoBitrate,videoBitrateMax,videoBitrateMin,videoQuality) ) 

#define IHMCEncoder_SetVideoInterlace(This,interlaced)	\
    ( (This)->lpVtbl -> SetVideoInterlace(This,interlaced) ) 

#define IHMCEncoder_GetVideoInterlace(This,interlaced)	\
    ( (This)->lpVtbl -> GetVideoInterlace(This,interlaced) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHMCEncoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HMCEncoder;

#ifdef __cplusplus

class DECLSPEC_UUID("3A45D055-A75B-4f46-8676-B8EB32883699")
HMCEncoder;
#endif
#endif /* __HMCEncoderLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


