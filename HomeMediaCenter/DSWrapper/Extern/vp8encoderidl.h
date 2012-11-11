

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Jun 14 11:59:12 2012
 */
/* Compiler settings for ..\IDL\vp8encoder.idl:
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


#ifndef __vp8encoderidl_h__
#define __vp8encoderidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVP8Encoder_FWD_DEFINED__
#define __IVP8Encoder_FWD_DEFINED__
typedef interface IVP8Encoder IVP8Encoder;
#endif 	/* __IVP8Encoder_FWD_DEFINED__ */


#ifndef __VP8Encoder_FWD_DEFINED__
#define __VP8Encoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class VP8Encoder VP8Encoder;
#else
typedef struct VP8Encoder VP8Encoder;
#endif /* __cplusplus */

#endif 	/* __VP8Encoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "strmif.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __VP8EncoderLib_LIBRARY_DEFINED__
#define __VP8EncoderLib_LIBRARY_DEFINED__

/* library VP8EncoderLib */
/* [version][helpstring][uuid] */ 


enum VP8Deadline
    {	kDeadlineBestQuality	= 0,
	kDeadlineRealtime	= 1,
	kDeadlineGoodQuality	= 1000000
    } ;

enum VP8KeyframeMode
    {	kKeyframeModeDefault	= -1,
	kKeyframeModeDisabled	= 0,
	kKeyframeModeAuto	= 1
    } ;

enum VP8EndUsage
    {	kEndUsageDefault	= -1,
	kEndUsageVBR	= 0,
	kEndUsageCBR	= 1
    } ;

enum VP8PassMode
    {	kPassModeOnePass	= 0,
	kPassModeFirstPass	= ( kPassModeOnePass + 1 ) ,
	kPassModeLastPass	= ( kPassModeFirstPass + 1 ) 
    } ;

EXTERN_C const IID LIBID_VP8EncoderLib;

#ifndef __IVP8Encoder_INTERFACE_DEFINED__
#define __IVP8Encoder_INTERFACE_DEFINED__

/* interface IVP8Encoder */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVP8Encoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED311151-5211-11DF-94AF-0026B977EEAA")
    IVP8Encoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ApplySettings( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetSettings( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDeadline( 
            /* [in] */ int Deadline) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeadline( 
            /* [out] */ int *pDeadline) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThreadCount( 
            /* [in] */ int Threads) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThreadCount( 
            /* [out] */ int *pThreads) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetErrorResilient( 
            /* [in] */ int ErrorResilient) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrorResilient( 
            /* [out] */ int *pErrorResilient) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDropframeThreshold( 
            /* [in] */ int DropframeThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDropframeThreshold( 
            /* [out] */ int *pDropframeThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResizeAllowed( 
            /* [in] */ int ResizeAllowed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResizeAllowed( 
            /* [out] */ int *pResizeAllowed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResizeUpThreshold( 
            /* [in] */ int ResizeUpThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResizeUpThreshold( 
            /* [out] */ int *pResizeUpThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResizeDownThreshold( 
            /* [in] */ int ResizeDownThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResizeDownThreshold( 
            /* [out] */ int *pResizeDownThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEndUsage( 
            /* [in] */ enum VP8EndUsage EndUsage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEndUsage( 
            /* [out] */ enum VP8EndUsage *pEndUsage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLagInFrames( 
            /* [in] */ int LagInFrames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLagInFrames( 
            /* [out] */ int *pLagInFrames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTokenPartitions( 
            /* [in] */ int TokenPartition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenPartitions( 
            /* [out] */ int *pTokenPartition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTargetBitrate( 
            /* [in] */ int Bitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTargetBitrate( 
            /* [out] */ int *pBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMinQuantizer( 
            /* [in] */ int MinQuantizer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMinQuantizer( 
            /* [out] */ int *pMinQuantizer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMaxQuantizer( 
            /* [in] */ int MaxQuantizer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxQuantizer( 
            /* [out] */ int *pMaxQuantizer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetUndershootPct( 
            /* [in] */ int UndershootPct) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUndershootPct( 
            /* [out] */ int *pUndershootPct) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOvershootPct( 
            /* [in] */ int OvershootPct) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOvershootPct( 
            /* [out] */ int *pOvershootPct) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDecoderBufferSize( 
            /* [in] */ int TimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDecoderBufferSize( 
            /* [out] */ int *pTimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDecoderBufferInitialSize( 
            /* [in] */ int TimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDecoderBufferInitialSize( 
            /* [out] */ int *pTimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDecoderBufferOptimalSize( 
            /* [in] */ int TimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDecoderBufferOptimalSize( 
            /* [out] */ int *pTimeInMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKeyframeMode( 
            /* [in] */ enum VP8KeyframeMode Mode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyframeMode( 
            /* [out] */ enum VP8KeyframeMode *pMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKeyframeMinInterval( 
            /* [in] */ int MinInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyframeMinInterval( 
            /* [out] */ int *pMinInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKeyframeMaxInterval( 
            /* [in] */ int MaxInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyframeMaxInterval( 
            /* [out] */ int *pMaxInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPassMode( 
            /* [in] */ enum VP8PassMode PassMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPassMode( 
            /* [out] */ enum VP8PassMode *pPassMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTwoPassStatsBuf( 
            const BYTE *Buffer,
            LONGLONG Length) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTwoPassStatsBuf( 
            const BYTE **pBuffer,
            LONGLONG *pLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTwoPassVbrBiasPct( 
            /* [in] */ int Bias) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTwoPassVbrBiasPct( 
            /* [out] */ int *pBias) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTwoPassVbrMinsectionPct( 
            /* [in] */ int Bitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTwoPassVbrMinsectionPct( 
            /* [out] */ int *pBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTwoPassVbrMaxsectionPct( 
            /* [in] */ int Bitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTwoPassVbrMaxsectionPct( 
            /* [out] */ int *pBitrate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetForceKeyframe( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearForceKeyframe( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAutoAltRef( 
            /* [in] */ int AutoAltRef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAutoAltRef( 
            /* [out] */ int *pAutoAltRef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetARNRMaxFrames( 
            /* [in] */ int Frames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetARNRMaxFrames( 
            /* [out] */ int *pFrames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetARNRStrength( 
            /* [in] */ int Strength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetARNRStrength( 
            /* [out] */ int *pStrength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetARNRType( 
            /* [in] */ int Type) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetARNRType( 
            /* [out] */ int *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFixedKeyframeInterval( 
            /* [in] */ REFERENCE_TIME Interval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFixedKeyframeInterval( 
            /* [out] */ REFERENCE_TIME *pInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCPUUsed( 
            /* [in] */ int CPUUsed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCPUUsed( 
            /* [out] */ int *pCPUUsed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStaticThreshold( 
            /* [in] */ int StaticThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStaticThreshold( 
            /* [out] */ int *pStaticThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDecimate( 
            /* [in] */ int Decimate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDecimate( 
            /* [out] */ int *pDecimate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVP8EncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVP8Encoder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVP8Encoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVP8Encoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ApplySettings )( 
            IVP8Encoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ResetSettings )( 
            IVP8Encoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetDeadline )( 
            IVP8Encoder * This,
            /* [in] */ int Deadline);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeadline )( 
            IVP8Encoder * This,
            /* [out] */ int *pDeadline);
        
        HRESULT ( STDMETHODCALLTYPE *SetThreadCount )( 
            IVP8Encoder * This,
            /* [in] */ int Threads);
        
        HRESULT ( STDMETHODCALLTYPE *GetThreadCount )( 
            IVP8Encoder * This,
            /* [out] */ int *pThreads);
        
        HRESULT ( STDMETHODCALLTYPE *SetErrorResilient )( 
            IVP8Encoder * This,
            /* [in] */ int ErrorResilient);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorResilient )( 
            IVP8Encoder * This,
            /* [out] */ int *pErrorResilient);
        
        HRESULT ( STDMETHODCALLTYPE *SetDropframeThreshold )( 
            IVP8Encoder * This,
            /* [in] */ int DropframeThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *GetDropframeThreshold )( 
            IVP8Encoder * This,
            /* [out] */ int *pDropframeThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *SetResizeAllowed )( 
            IVP8Encoder * This,
            /* [in] */ int ResizeAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *GetResizeAllowed )( 
            IVP8Encoder * This,
            /* [out] */ int *pResizeAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *SetResizeUpThreshold )( 
            IVP8Encoder * This,
            /* [in] */ int ResizeUpThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *GetResizeUpThreshold )( 
            IVP8Encoder * This,
            /* [out] */ int *pResizeUpThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *SetResizeDownThreshold )( 
            IVP8Encoder * This,
            /* [in] */ int ResizeDownThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *GetResizeDownThreshold )( 
            IVP8Encoder * This,
            /* [out] */ int *pResizeDownThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *SetEndUsage )( 
            IVP8Encoder * This,
            /* [in] */ enum VP8EndUsage EndUsage);
        
        HRESULT ( STDMETHODCALLTYPE *GetEndUsage )( 
            IVP8Encoder * This,
            /* [out] */ enum VP8EndUsage *pEndUsage);
        
        HRESULT ( STDMETHODCALLTYPE *SetLagInFrames )( 
            IVP8Encoder * This,
            /* [in] */ int LagInFrames);
        
        HRESULT ( STDMETHODCALLTYPE *GetLagInFrames )( 
            IVP8Encoder * This,
            /* [out] */ int *pLagInFrames);
        
        HRESULT ( STDMETHODCALLTYPE *SetTokenPartitions )( 
            IVP8Encoder * This,
            /* [in] */ int TokenPartition);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenPartitions )( 
            IVP8Encoder * This,
            /* [out] */ int *pTokenPartition);
        
        HRESULT ( STDMETHODCALLTYPE *SetTargetBitrate )( 
            IVP8Encoder * This,
            /* [in] */ int Bitrate);
        
        HRESULT ( STDMETHODCALLTYPE *GetTargetBitrate )( 
            IVP8Encoder * This,
            /* [out] */ int *pBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetMinQuantizer )( 
            IVP8Encoder * This,
            /* [in] */ int MinQuantizer);
        
        HRESULT ( STDMETHODCALLTYPE *GetMinQuantizer )( 
            IVP8Encoder * This,
            /* [out] */ int *pMinQuantizer);
        
        HRESULT ( STDMETHODCALLTYPE *SetMaxQuantizer )( 
            IVP8Encoder * This,
            /* [in] */ int MaxQuantizer);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxQuantizer )( 
            IVP8Encoder * This,
            /* [out] */ int *pMaxQuantizer);
        
        HRESULT ( STDMETHODCALLTYPE *SetUndershootPct )( 
            IVP8Encoder * This,
            /* [in] */ int UndershootPct);
        
        HRESULT ( STDMETHODCALLTYPE *GetUndershootPct )( 
            IVP8Encoder * This,
            /* [out] */ int *pUndershootPct);
        
        HRESULT ( STDMETHODCALLTYPE *SetOvershootPct )( 
            IVP8Encoder * This,
            /* [in] */ int OvershootPct);
        
        HRESULT ( STDMETHODCALLTYPE *GetOvershootPct )( 
            IVP8Encoder * This,
            /* [out] */ int *pOvershootPct);
        
        HRESULT ( STDMETHODCALLTYPE *SetDecoderBufferSize )( 
            IVP8Encoder * This,
            /* [in] */ int TimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *GetDecoderBufferSize )( 
            IVP8Encoder * This,
            /* [out] */ int *pTimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *SetDecoderBufferInitialSize )( 
            IVP8Encoder * This,
            /* [in] */ int TimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *GetDecoderBufferInitialSize )( 
            IVP8Encoder * This,
            /* [out] */ int *pTimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *SetDecoderBufferOptimalSize )( 
            IVP8Encoder * This,
            /* [in] */ int TimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *GetDecoderBufferOptimalSize )( 
            IVP8Encoder * This,
            /* [out] */ int *pTimeInMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *SetKeyframeMode )( 
            IVP8Encoder * This,
            /* [in] */ enum VP8KeyframeMode Mode);
        
        HRESULT ( STDMETHODCALLTYPE *GetKeyframeMode )( 
            IVP8Encoder * This,
            /* [out] */ enum VP8KeyframeMode *pMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetKeyframeMinInterval )( 
            IVP8Encoder * This,
            /* [in] */ int MinInterval);
        
        HRESULT ( STDMETHODCALLTYPE *GetKeyframeMinInterval )( 
            IVP8Encoder * This,
            /* [out] */ int *pMinInterval);
        
        HRESULT ( STDMETHODCALLTYPE *SetKeyframeMaxInterval )( 
            IVP8Encoder * This,
            /* [in] */ int MaxInterval);
        
        HRESULT ( STDMETHODCALLTYPE *GetKeyframeMaxInterval )( 
            IVP8Encoder * This,
            /* [out] */ int *pMaxInterval);
        
        HRESULT ( STDMETHODCALLTYPE *SetPassMode )( 
            IVP8Encoder * This,
            /* [in] */ enum VP8PassMode PassMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetPassMode )( 
            IVP8Encoder * This,
            /* [out] */ enum VP8PassMode *pPassMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetTwoPassStatsBuf )( 
            IVP8Encoder * This,
            const BYTE *Buffer,
            LONGLONG Length);
        
        HRESULT ( STDMETHODCALLTYPE *GetTwoPassStatsBuf )( 
            IVP8Encoder * This,
            const BYTE **pBuffer,
            LONGLONG *pLength);
        
        HRESULT ( STDMETHODCALLTYPE *SetTwoPassVbrBiasPct )( 
            IVP8Encoder * This,
            /* [in] */ int Bias);
        
        HRESULT ( STDMETHODCALLTYPE *GetTwoPassVbrBiasPct )( 
            IVP8Encoder * This,
            /* [out] */ int *pBias);
        
        HRESULT ( STDMETHODCALLTYPE *SetTwoPassVbrMinsectionPct )( 
            IVP8Encoder * This,
            /* [in] */ int Bitrate);
        
        HRESULT ( STDMETHODCALLTYPE *GetTwoPassVbrMinsectionPct )( 
            IVP8Encoder * This,
            /* [out] */ int *pBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetTwoPassVbrMaxsectionPct )( 
            IVP8Encoder * This,
            /* [in] */ int Bitrate);
        
        HRESULT ( STDMETHODCALLTYPE *GetTwoPassVbrMaxsectionPct )( 
            IVP8Encoder * This,
            /* [out] */ int *pBitrate);
        
        HRESULT ( STDMETHODCALLTYPE *SetForceKeyframe )( 
            IVP8Encoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ClearForceKeyframe )( 
            IVP8Encoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetAutoAltRef )( 
            IVP8Encoder * This,
            /* [in] */ int AutoAltRef);
        
        HRESULT ( STDMETHODCALLTYPE *GetAutoAltRef )( 
            IVP8Encoder * This,
            /* [out] */ int *pAutoAltRef);
        
        HRESULT ( STDMETHODCALLTYPE *SetARNRMaxFrames )( 
            IVP8Encoder * This,
            /* [in] */ int Frames);
        
        HRESULT ( STDMETHODCALLTYPE *GetARNRMaxFrames )( 
            IVP8Encoder * This,
            /* [out] */ int *pFrames);
        
        HRESULT ( STDMETHODCALLTYPE *SetARNRStrength )( 
            IVP8Encoder * This,
            /* [in] */ int Strength);
        
        HRESULT ( STDMETHODCALLTYPE *GetARNRStrength )( 
            IVP8Encoder * This,
            /* [out] */ int *pStrength);
        
        HRESULT ( STDMETHODCALLTYPE *SetARNRType )( 
            IVP8Encoder * This,
            /* [in] */ int Type);
        
        HRESULT ( STDMETHODCALLTYPE *GetARNRType )( 
            IVP8Encoder * This,
            /* [out] */ int *pType);
        
        HRESULT ( STDMETHODCALLTYPE *SetFixedKeyframeInterval )( 
            IVP8Encoder * This,
            /* [in] */ REFERENCE_TIME Interval);
        
        HRESULT ( STDMETHODCALLTYPE *GetFixedKeyframeInterval )( 
            IVP8Encoder * This,
            /* [out] */ REFERENCE_TIME *pInterval);
        
        HRESULT ( STDMETHODCALLTYPE *SetCPUUsed )( 
            IVP8Encoder * This,
            /* [in] */ int CPUUsed);
        
        HRESULT ( STDMETHODCALLTYPE *GetCPUUsed )( 
            IVP8Encoder * This,
            /* [out] */ int *pCPUUsed);
        
        HRESULT ( STDMETHODCALLTYPE *SetStaticThreshold )( 
            IVP8Encoder * This,
            /* [in] */ int StaticThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *GetStaticThreshold )( 
            IVP8Encoder * This,
            /* [out] */ int *pStaticThreshold);
        
        HRESULT ( STDMETHODCALLTYPE *SetDecimate )( 
            IVP8Encoder * This,
            /* [in] */ int Decimate);
        
        HRESULT ( STDMETHODCALLTYPE *GetDecimate )( 
            IVP8Encoder * This,
            /* [out] */ int *pDecimate);
        
        END_INTERFACE
    } IVP8EncoderVtbl;

    interface IVP8Encoder
    {
        CONST_VTBL struct IVP8EncoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVP8Encoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVP8Encoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVP8Encoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVP8Encoder_ApplySettings(This)	\
    ( (This)->lpVtbl -> ApplySettings(This) ) 

#define IVP8Encoder_ResetSettings(This)	\
    ( (This)->lpVtbl -> ResetSettings(This) ) 

#define IVP8Encoder_SetDeadline(This,Deadline)	\
    ( (This)->lpVtbl -> SetDeadline(This,Deadline) ) 

#define IVP8Encoder_GetDeadline(This,pDeadline)	\
    ( (This)->lpVtbl -> GetDeadline(This,pDeadline) ) 

#define IVP8Encoder_SetThreadCount(This,Threads)	\
    ( (This)->lpVtbl -> SetThreadCount(This,Threads) ) 

#define IVP8Encoder_GetThreadCount(This,pThreads)	\
    ( (This)->lpVtbl -> GetThreadCount(This,pThreads) ) 

#define IVP8Encoder_SetErrorResilient(This,ErrorResilient)	\
    ( (This)->lpVtbl -> SetErrorResilient(This,ErrorResilient) ) 

#define IVP8Encoder_GetErrorResilient(This,pErrorResilient)	\
    ( (This)->lpVtbl -> GetErrorResilient(This,pErrorResilient) ) 

#define IVP8Encoder_SetDropframeThreshold(This,DropframeThreshold)	\
    ( (This)->lpVtbl -> SetDropframeThreshold(This,DropframeThreshold) ) 

#define IVP8Encoder_GetDropframeThreshold(This,pDropframeThreshold)	\
    ( (This)->lpVtbl -> GetDropframeThreshold(This,pDropframeThreshold) ) 

#define IVP8Encoder_SetResizeAllowed(This,ResizeAllowed)	\
    ( (This)->lpVtbl -> SetResizeAllowed(This,ResizeAllowed) ) 

#define IVP8Encoder_GetResizeAllowed(This,pResizeAllowed)	\
    ( (This)->lpVtbl -> GetResizeAllowed(This,pResizeAllowed) ) 

#define IVP8Encoder_SetResizeUpThreshold(This,ResizeUpThreshold)	\
    ( (This)->lpVtbl -> SetResizeUpThreshold(This,ResizeUpThreshold) ) 

#define IVP8Encoder_GetResizeUpThreshold(This,pResizeUpThreshold)	\
    ( (This)->lpVtbl -> GetResizeUpThreshold(This,pResizeUpThreshold) ) 

#define IVP8Encoder_SetResizeDownThreshold(This,ResizeDownThreshold)	\
    ( (This)->lpVtbl -> SetResizeDownThreshold(This,ResizeDownThreshold) ) 

#define IVP8Encoder_GetResizeDownThreshold(This,pResizeDownThreshold)	\
    ( (This)->lpVtbl -> GetResizeDownThreshold(This,pResizeDownThreshold) ) 

#define IVP8Encoder_SetEndUsage(This,EndUsage)	\
    ( (This)->lpVtbl -> SetEndUsage(This,EndUsage) ) 

#define IVP8Encoder_GetEndUsage(This,pEndUsage)	\
    ( (This)->lpVtbl -> GetEndUsage(This,pEndUsage) ) 

#define IVP8Encoder_SetLagInFrames(This,LagInFrames)	\
    ( (This)->lpVtbl -> SetLagInFrames(This,LagInFrames) ) 

#define IVP8Encoder_GetLagInFrames(This,pLagInFrames)	\
    ( (This)->lpVtbl -> GetLagInFrames(This,pLagInFrames) ) 

#define IVP8Encoder_SetTokenPartitions(This,TokenPartition)	\
    ( (This)->lpVtbl -> SetTokenPartitions(This,TokenPartition) ) 

#define IVP8Encoder_GetTokenPartitions(This,pTokenPartition)	\
    ( (This)->lpVtbl -> GetTokenPartitions(This,pTokenPartition) ) 

#define IVP8Encoder_SetTargetBitrate(This,Bitrate)	\
    ( (This)->lpVtbl -> SetTargetBitrate(This,Bitrate) ) 

#define IVP8Encoder_GetTargetBitrate(This,pBitrate)	\
    ( (This)->lpVtbl -> GetTargetBitrate(This,pBitrate) ) 

#define IVP8Encoder_SetMinQuantizer(This,MinQuantizer)	\
    ( (This)->lpVtbl -> SetMinQuantizer(This,MinQuantizer) ) 

#define IVP8Encoder_GetMinQuantizer(This,pMinQuantizer)	\
    ( (This)->lpVtbl -> GetMinQuantizer(This,pMinQuantizer) ) 

#define IVP8Encoder_SetMaxQuantizer(This,MaxQuantizer)	\
    ( (This)->lpVtbl -> SetMaxQuantizer(This,MaxQuantizer) ) 

#define IVP8Encoder_GetMaxQuantizer(This,pMaxQuantizer)	\
    ( (This)->lpVtbl -> GetMaxQuantizer(This,pMaxQuantizer) ) 

#define IVP8Encoder_SetUndershootPct(This,UndershootPct)	\
    ( (This)->lpVtbl -> SetUndershootPct(This,UndershootPct) ) 

#define IVP8Encoder_GetUndershootPct(This,pUndershootPct)	\
    ( (This)->lpVtbl -> GetUndershootPct(This,pUndershootPct) ) 

#define IVP8Encoder_SetOvershootPct(This,OvershootPct)	\
    ( (This)->lpVtbl -> SetOvershootPct(This,OvershootPct) ) 

#define IVP8Encoder_GetOvershootPct(This,pOvershootPct)	\
    ( (This)->lpVtbl -> GetOvershootPct(This,pOvershootPct) ) 

#define IVP8Encoder_SetDecoderBufferSize(This,TimeInMilliseconds)	\
    ( (This)->lpVtbl -> SetDecoderBufferSize(This,TimeInMilliseconds) ) 

#define IVP8Encoder_GetDecoderBufferSize(This,pTimeInMilliseconds)	\
    ( (This)->lpVtbl -> GetDecoderBufferSize(This,pTimeInMilliseconds) ) 

#define IVP8Encoder_SetDecoderBufferInitialSize(This,TimeInMilliseconds)	\
    ( (This)->lpVtbl -> SetDecoderBufferInitialSize(This,TimeInMilliseconds) ) 

#define IVP8Encoder_GetDecoderBufferInitialSize(This,pTimeInMilliseconds)	\
    ( (This)->lpVtbl -> GetDecoderBufferInitialSize(This,pTimeInMilliseconds) ) 

#define IVP8Encoder_SetDecoderBufferOptimalSize(This,TimeInMilliseconds)	\
    ( (This)->lpVtbl -> SetDecoderBufferOptimalSize(This,TimeInMilliseconds) ) 

#define IVP8Encoder_GetDecoderBufferOptimalSize(This,pTimeInMilliseconds)	\
    ( (This)->lpVtbl -> GetDecoderBufferOptimalSize(This,pTimeInMilliseconds) ) 

#define IVP8Encoder_SetKeyframeMode(This,Mode)	\
    ( (This)->lpVtbl -> SetKeyframeMode(This,Mode) ) 

#define IVP8Encoder_GetKeyframeMode(This,pMode)	\
    ( (This)->lpVtbl -> GetKeyframeMode(This,pMode) ) 

#define IVP8Encoder_SetKeyframeMinInterval(This,MinInterval)	\
    ( (This)->lpVtbl -> SetKeyframeMinInterval(This,MinInterval) ) 

#define IVP8Encoder_GetKeyframeMinInterval(This,pMinInterval)	\
    ( (This)->lpVtbl -> GetKeyframeMinInterval(This,pMinInterval) ) 

#define IVP8Encoder_SetKeyframeMaxInterval(This,MaxInterval)	\
    ( (This)->lpVtbl -> SetKeyframeMaxInterval(This,MaxInterval) ) 

#define IVP8Encoder_GetKeyframeMaxInterval(This,pMaxInterval)	\
    ( (This)->lpVtbl -> GetKeyframeMaxInterval(This,pMaxInterval) ) 

#define IVP8Encoder_SetPassMode(This,PassMode)	\
    ( (This)->lpVtbl -> SetPassMode(This,PassMode) ) 

#define IVP8Encoder_GetPassMode(This,pPassMode)	\
    ( (This)->lpVtbl -> GetPassMode(This,pPassMode) ) 

#define IVP8Encoder_SetTwoPassStatsBuf(This,Buffer,Length)	\
    ( (This)->lpVtbl -> SetTwoPassStatsBuf(This,Buffer,Length) ) 

#define IVP8Encoder_GetTwoPassStatsBuf(This,pBuffer,pLength)	\
    ( (This)->lpVtbl -> GetTwoPassStatsBuf(This,pBuffer,pLength) ) 

#define IVP8Encoder_SetTwoPassVbrBiasPct(This,Bias)	\
    ( (This)->lpVtbl -> SetTwoPassVbrBiasPct(This,Bias) ) 

#define IVP8Encoder_GetTwoPassVbrBiasPct(This,pBias)	\
    ( (This)->lpVtbl -> GetTwoPassVbrBiasPct(This,pBias) ) 

#define IVP8Encoder_SetTwoPassVbrMinsectionPct(This,Bitrate)	\
    ( (This)->lpVtbl -> SetTwoPassVbrMinsectionPct(This,Bitrate) ) 

#define IVP8Encoder_GetTwoPassVbrMinsectionPct(This,pBitrate)	\
    ( (This)->lpVtbl -> GetTwoPassVbrMinsectionPct(This,pBitrate) ) 

#define IVP8Encoder_SetTwoPassVbrMaxsectionPct(This,Bitrate)	\
    ( (This)->lpVtbl -> SetTwoPassVbrMaxsectionPct(This,Bitrate) ) 

#define IVP8Encoder_GetTwoPassVbrMaxsectionPct(This,pBitrate)	\
    ( (This)->lpVtbl -> GetTwoPassVbrMaxsectionPct(This,pBitrate) ) 

#define IVP8Encoder_SetForceKeyframe(This)	\
    ( (This)->lpVtbl -> SetForceKeyframe(This) ) 

#define IVP8Encoder_ClearForceKeyframe(This)	\
    ( (This)->lpVtbl -> ClearForceKeyframe(This) ) 

#define IVP8Encoder_SetAutoAltRef(This,AutoAltRef)	\
    ( (This)->lpVtbl -> SetAutoAltRef(This,AutoAltRef) ) 

#define IVP8Encoder_GetAutoAltRef(This,pAutoAltRef)	\
    ( (This)->lpVtbl -> GetAutoAltRef(This,pAutoAltRef) ) 

#define IVP8Encoder_SetARNRMaxFrames(This,Frames)	\
    ( (This)->lpVtbl -> SetARNRMaxFrames(This,Frames) ) 

#define IVP8Encoder_GetARNRMaxFrames(This,pFrames)	\
    ( (This)->lpVtbl -> GetARNRMaxFrames(This,pFrames) ) 

#define IVP8Encoder_SetARNRStrength(This,Strength)	\
    ( (This)->lpVtbl -> SetARNRStrength(This,Strength) ) 

#define IVP8Encoder_GetARNRStrength(This,pStrength)	\
    ( (This)->lpVtbl -> GetARNRStrength(This,pStrength) ) 

#define IVP8Encoder_SetARNRType(This,Type)	\
    ( (This)->lpVtbl -> SetARNRType(This,Type) ) 

#define IVP8Encoder_GetARNRType(This,pType)	\
    ( (This)->lpVtbl -> GetARNRType(This,pType) ) 

#define IVP8Encoder_SetFixedKeyframeInterval(This,Interval)	\
    ( (This)->lpVtbl -> SetFixedKeyframeInterval(This,Interval) ) 

#define IVP8Encoder_GetFixedKeyframeInterval(This,pInterval)	\
    ( (This)->lpVtbl -> GetFixedKeyframeInterval(This,pInterval) ) 

#define IVP8Encoder_SetCPUUsed(This,CPUUsed)	\
    ( (This)->lpVtbl -> SetCPUUsed(This,CPUUsed) ) 

#define IVP8Encoder_GetCPUUsed(This,pCPUUsed)	\
    ( (This)->lpVtbl -> GetCPUUsed(This,pCPUUsed) ) 

#define IVP8Encoder_SetStaticThreshold(This,StaticThreshold)	\
    ( (This)->lpVtbl -> SetStaticThreshold(This,StaticThreshold) ) 

#define IVP8Encoder_GetStaticThreshold(This,pStaticThreshold)	\
    ( (This)->lpVtbl -> GetStaticThreshold(This,pStaticThreshold) ) 

#define IVP8Encoder_SetDecimate(This,Decimate)	\
    ( (This)->lpVtbl -> SetDecimate(This,Decimate) ) 

#define IVP8Encoder_GetDecimate(This,pDecimate)	\
    ( (This)->lpVtbl -> GetDecimate(This,pDecimate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVP8Encoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VP8Encoder;

#ifdef __cplusplus

class DECLSPEC_UUID("ED3110F5-5211-11DF-94AF-0026B977EEAA")
VP8Encoder;
#endif
#endif /* __VP8EncoderLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


