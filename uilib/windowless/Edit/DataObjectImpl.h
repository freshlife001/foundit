//////////////////////////////////////////////////////////////////////////
//
//	描述:   IDataObject的实现，用以保存数据到剪切板中
//
//	
//	日期:   2011/11/24
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <Ole2.h>

class DataObjectImpl : public IDataObject
{
public:
    DataObjectImpl() ;

    //////////////////////////////////////////////////////////////////////////
    // IUnknown Interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [annotation][iid_is][out] */ 
        __RPC__deref_out  void **ppvObject) ;

    virtual ULONG STDMETHODCALLTYPE AddRef( void) ;

    virtual ULONG STDMETHODCALLTYPE Release( void) ;

    //////////////////////////////////////////////////////////////////////////
    // IDataObject Interface
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData( 
        /* [unique][in] */ FORMATETC *pformatetcIn,
        /* [out] */ STGMEDIUM *pmedium) ;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere( 
        /* [unique][in] */ FORMATETC *pformatetc,
        /* [out][in] */ STGMEDIUM *pmedium) ;

    virtual HRESULT STDMETHODCALLTYPE QueryGetData( 
        /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc) ;

    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc( 
        /* [unique][in] */ __RPC__in_opt FORMATETC *pformatectIn,
        /* [out] */ __RPC__out FORMATETC *pformatetcOut) ;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData( 
        /* [unique][in] */ FORMATETC *pformatetc,
        /* [unique][in] */ STGMEDIUM *pmedium,
        /* [in] */ BOOL fRelease) ;

    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc( 
        /* [in] */ DWORD dwDirection,
        /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc) ;

    virtual HRESULT STDMETHODCALLTYPE DAdvise( 
        /* [in] */ __RPC__in FORMATETC *pformatetc,
        /* [in] */ DWORD advf,
        /* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
        /* [out] */ __RPC__out DWORD *pdwConnection) ;

    virtual HRESULT STDMETHODCALLTYPE DUnadvise( 
        /* [in] */ DWORD dwConnection) ;

    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise( 
        /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise) ;

private:
    ULONG m_ref_count ;

};
