#include "stdafx.h"
#include "DataObjectImpl.h"

#include "EnumFormatEtcImpl.h"
#include "common/MemoryLeakChecker.h"
DataObjectImpl::DataObjectImpl()
{
    m_ref_count = 0 ;
}

//////////////////////////////////////////////////////////////////////////
// IUnknown Interface

// virtual 
HRESULT DataObjectImpl::QueryInterface( 
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */ 
    __RPC__deref_out  void **ppvObject) 
{
    *ppvObject = NULL ;
    HRESULT hr = E_NOINTERFACE ;

    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObject = this ;
    }
    else if (IsEqualIID(riid, IID_IDataObject))
    {
        *ppvObject = (IDataObject*)this ;
    }

    if (*ppvObject != NULL)
    {
        AddRef() ;
        hr = S_OK ;
    }

    return hr ;
}

// virtual 
ULONG DataObjectImpl::AddRef( void) 
{
    ++ m_ref_count ;
    return m_ref_count ;
}

// virtual 
ULONG DataObjectImpl::Release( void) 
{
    -- m_ref_count ;
    if (m_ref_count == 0)
    {
        delete this ;
    }

    return m_ref_count ;
}

//////////////////////////////////////////////////////////////////////////
// IDataObject Interface
// virtual  
HRESULT DataObjectImpl::GetData( 
                                FORMATETC *pformatetcIn,
                                STGMEDIUM *pmedium) 
{
    if (pformatetcIn->cfFormat == CF_TEXT)
    {
        char t[] = "ÎÒÊÇË­" ;
        HGLOBAL mem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(t)) ;
        char * p = (char*)GlobalLock(mem) ;
        memcpy(p, t, sizeof(t)) ;
        GlobalUnlock(mem) ;

        pmedium->tymed = TYMED_HGLOBAL ;
        pmedium->hGlobal = mem ;
        pmedium->pUnkForRelease = NULL ;
    }
    return S_OK ;
}

// virtual
HRESULT DataObjectImpl::GetDataHere( 
    FORMATETC *pformatetc,
    STGMEDIUM *pmedium) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::QueryGetData( 
    __RPC__in_opt FORMATETC *pformatetc) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::GetCanonicalFormatEtc( 
    __RPC__in_opt FORMATETC *pformatectIn,
    __RPC__out FORMATETC *pformatetcOut) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::SetData( 
    FORMATETC *pformatetc,
    STGMEDIUM *pmedium,
    BOOL fRelease) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::EnumFormatEtc( 
    DWORD dwDirection,
    __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc) 
{
    FORMATETC fetc[3] = 
    {
        {CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}, 
        {CF_METAFILEPICT, NULL, DVASPECT_CONTENT, -1, TYMED_MFPICT}, 
        {CF_BITMAP, NULL, DVASPECT_CONTENT, -1, TYMED_GDI}, 
    } ;
    *ppenumFormatEtc = new EnumFormatEtcImpl(1, fetc) ;
    (*ppenumFormatEtc)->AddRef() ;
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::DAdvise( 
    __RPC__in FORMATETC *pformatetc,
    DWORD advf,
    __RPC__in_opt IAdviseSink *pAdvSink,
    __RPC__out DWORD *pdwConnection) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::DUnadvise( 
    DWORD dwConnection) 
{
    return S_OK ;
}

// virtual 
HRESULT DataObjectImpl::EnumDAdvise( 
    __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise) 
{
    return S_OK ;
}
