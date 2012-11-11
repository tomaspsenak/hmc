

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Jun 14 12:00:19 2012
 */
/* Compiler settings for ..\IDL\webmmux.idl:
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


#ifndef __webmmuxidl_h__
#define __webmmuxidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWebmMux_FWD_DEFINED__
#define __IWebmMux_FWD_DEFINED__
typedef interface IWebmMux IWebmMux;
#endif 	/* __IWebmMux_FWD_DEFINED__ */


#ifndef __WebmMux_FWD_DEFINED__
#define __WebmMux_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebmMux WebmMux;
#else
typedef struct WebmMux WebmMux;
#endif /* __cplusplus */

#endif 	/* __WebmMux_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __WebmMuxLib_LIBRARY_DEFINED__
#define __WebmMuxLib_LIBRARY_DEFINED__

/* library WebmMuxLib */
/* [version][helpstring][uuid] */ 


enum WebmMuxMode
    {	kWebmMuxModeDefault	= 0,
	kWebmMuxModeLive	= 1
    } ;

EXTERN_C const IID LIBID_WebmMuxLib;

#ifndef __IWebmMux_INTERFACE_DEFINED__
#define __IWebmMux_INTERFACE_DEFINED__

/* interface IWebmMux */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWebmMux;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED311106-5211-11DF-94AF-0026B977EEAA")
    IWebmMux : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetWritingApp( 
            /* [string][in] */ const wchar_t *__MIDL__IWebmMux0000) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWritingApp( 
            /* [string][out] */ wchar_t **__MIDL__IWebmMux0001) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMuxMode( 
            /* [in] */ enum WebmMuxMode __MIDL__IWebmMux0002) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMuxMode( 
            /* [out] */ enum WebmMuxMode *__MIDL__IWebmMux0003) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebmMuxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebmMux * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebmMux * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebmMux * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetWritingApp )( 
            IWebmMux * This,
            /* [string][in] */ const wchar_t *__MIDL__IWebmMux0000);
        
        HRESULT ( STDMETHODCALLTYPE *GetWritingApp )( 
            IWebmMux * This,
            /* [string][out] */ wchar_t **__MIDL__IWebmMux0001);
        
        HRESULT ( STDMETHODCALLTYPE *SetMuxMode )( 
            IWebmMux * This,
            /* [in] */ enum WebmMuxMode __MIDL__IWebmMux0002);
        
        HRESULT ( STDMETHODCALLTYPE *GetMuxMode )( 
            IWebmMux * This,
            /* [out] */ enum WebmMuxMode *__MIDL__IWebmMux0003);
        
        END_INTERFACE
    } IWebmMuxVtbl;

    interface IWebmMux
    {
        CONST_VTBL struct IWebmMuxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebmMux_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebmMux_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebmMux_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebmMux_SetWritingApp(This,__MIDL__IWebmMux0000)	\
    ( (This)->lpVtbl -> SetWritingApp(This,__MIDL__IWebmMux0000) ) 

#define IWebmMux_GetWritingApp(This,__MIDL__IWebmMux0001)	\
    ( (This)->lpVtbl -> GetWritingApp(This,__MIDL__IWebmMux0001) ) 

#define IWebmMux_SetMuxMode(This,__MIDL__IWebmMux0002)	\
    ( (This)->lpVtbl -> SetMuxMode(This,__MIDL__IWebmMux0002) ) 

#define IWebmMux_GetMuxMode(This,__MIDL__IWebmMux0003)	\
    ( (This)->lpVtbl -> GetMuxMode(This,__MIDL__IWebmMux0003) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebmMux_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WebmMux;

#ifdef __cplusplus

class DECLSPEC_UUID("ED3110F0-5211-11DF-94AF-0026B977EEAA")
WebmMux;
#endif
#endif /* __WebmMuxLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


