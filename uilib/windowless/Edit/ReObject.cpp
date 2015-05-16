#include "stdafx.h"


#include "ReObject.h"
#include "EnumFormatEtcImpl.h"
#include "utils.h"
#include "common/MemoryLeakChecker.h"
LONG ReObject::s_x_pixel_per_inch = 0 ;
LONG ReObject::s_y_pixel_per_inch = 0 ;

ReObject::ReObject(void)
{
    m_ref_count = 0 ;

    m_width_in_pixel = 47 ;
    m_height_in_pixel = 47 ;

    m_client_site = NULL ;
    m_advise_sink = NULL ;

    m_host = NULL ;
    m_been_drawn_flag = false ;
    ::SetRect(&m_draw_bound, 0, 0, 0, 0) ;

    if (s_x_pixel_per_inch == 0 || s_y_pixel_per_inch == 0)
    {
        HDC hdc = ::GetDC(NULL) ;

        s_x_pixel_per_inch = ::GetDeviceCaps(hdc, LOGPIXELSX) ;
        s_y_pixel_per_inch = ::GetDeviceCaps(hdc, LOGPIXELSY) ;

        ::ReleaseDC(NULL, hdc) ;
    }

    m_width_in_himetric = DXtoHimetricX(m_width_in_pixel, s_x_pixel_per_inch) ;
    m_height_in_himetric = DYtoHimetricY(m_height_in_pixel, s_y_pixel_per_inch) ;
}

ReObject::~ReObject(void)
{
}

void ReObject::SetHost(HWND host) 
{
    m_host = host ;
}

void ReObject::ResetHide(RECT const &update_bound) 
{
    RECT temp ;
    if (m_been_drawn_flag && ::IntersectRect(&temp, &m_draw_bound, &update_bound))
    {
        m_been_drawn_flag = false ;
    }
}

void ReObject::CheckShowOrHide() 
{
    if (!m_been_drawn_flag)
    {
        MoveOutOfVisibleArea() ;
    }
}

// 对象移出了编辑框的可视区域 - virtual
void ReObject::MoveOutOfVisibleArea() 
{
    // DO NOTHING!!!
}

// 对象移入了编辑框的可视区域 - virtual
void ReObject::MoveIntoVisibleArea() 
{
    // DO NOTHING!!!
}

// 这个接口没有判断tomEdit的inset，所以目前有BUG
bool ReObject::IsShowInEditJustByBound() 
{
    IOleClientSite * client_site = NULL ;
    GetClientSite(&client_site) ;
    if (client_site == NULL)
        return false ;

    IOleInPlaceSite * inplace_site = NULL ;
    client_site->QueryInterface(IID_IOleInPlaceSite, (void**)&inplace_site) ;
    if (inplace_site == NULL)
        return false ;

    RECT bound, clip ;
    if (S_OK == inplace_site->GetWindowContext(NULL, NULL, &bound, &clip, NULL))
    {
        RECT intersect ;
        if (::IntersectRect(&intersect, &bound, &clip))
            return true ;
    }

    return false ;
}

//////////////////////////////////////////////////////////////////////////
//IUnknown methods

HRESULT ReObject::QueryInterface( const IID &riid, void **ppvObject )
{
	bool support = false;
	HRESULT hr = E_NOINTERFACE;

    if( riid == IID_IUnknown )
    {
        support = true;
        *ppvObject = this;
    }
    else if( riid == IID_IOleObject )
    {
        support = true;
        *ppvObject = static_cast< IOleObject * >(this);
    }
    else if( riid == IID_IDataObject )
    {
        support = true;
        *ppvObject = static_cast< IDataObject * >(this);
    }
    else if( riid == IID_IPersistStorage )
    {
        support = true;
        *ppvObject = static_cast< IPersistStorage * >(this);
    }
    else if( riid == IID_IViewObject )
    {
        support = true;
        *ppvObject = static_cast< IViewObject * >(this);
    }
    else if( riid == IID_IViewObject2 )
    {
        support = true;
        *ppvObject = static_cast< IViewObject2 * >(this);
    }
    else if( riid == IID_IViewObjectEx )
    {
        support = true;
        *ppvObject = static_cast< IViewObjectEx * >(this);
    }
    else if( riid == IID_IOleWindow )
    {
        support = true;
        *ppvObject = static_cast< IOleWindow * >(static_cast< IOleInPlaceActiveObject * >(this));
    }
	else if( riid == IID_IOleInPlaceActiveObject )
	{
		support = true;
		*ppvObject = static_cast< IOleInPlaceActiveObject * >(this);
    }
    else if( riid == IID_IOleInPlaceObject )
    {
        support = true;
        *ppvObject = static_cast< IOleInPlaceObject * >(this);
    }
	else if( riid == IID_IOleInPlaceObjectWindowless )
	{
		support = true;
		*ppvObject = static_cast< IOleInPlaceObjectWindowless * >(this);
	}
    else if( riid == CLSID_ReObject )
	{
		support = true;
		*ppvObject = this;
	}
    else if (riid == IID_IClipboardDataObject)
    {
        support = true ;
        *ppvObject = static_cast<IClipboardDataObject*>(this) ;
    }

	if( support )
	{
		AddRef();
		hr = S_OK;
	}
	return hr;
}

ULONG ReObject::AddRef()
{
    return InterlockedIncrement((long*)&m_ref_count);
}

ULONG ReObject::Release()
{
    long count = InterlockedDecrement((long*)&m_ref_count) ;
    if( 0 == count )
    {
//         m_img.Reset() ;
        delete this ;
    }
    return count ;
}

//////////////////////////////////////////////////////////////////////////
//IOleObject methods

HRESULT ReObject::SetClientSite( IOleClientSite *pClientSite )
{
    m_client_site = pClientSite ;
    return S_OK ;
}

HRESULT ReObject::GetClientSite( IOleClientSite **ppClientSite )
{
    if (ppClientSite != NULL)
        *ppClientSite = m_client_site ;
    return S_OK ;
}

HRESULT ReObject::SetHostNames( LPCOLESTR szContainerApp, LPCOLESTR szContainerObj )
{
    return S_OK ;
}

HRESULT ReObject::Close( DWORD dwSaveOption )
{
    return S_OK ;
}

HRESULT ReObject::SetMoniker( DWORD dwWhichMoniker, IMoniker *pmk )
{
    return E_NOTIMPL ;
}

HRESULT ReObject::GetMoniker( DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk )
{
    *ppmk = NULL ;
    return E_NOTIMPL ;
}

HRESULT ReObject::InitFromData( IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved )
{
    return E_NOTIMPL ;
}

HRESULT ReObject::GetClipboardData( DWORD dwReserved, IDataObject **ppDataObject )
{
    //QueryInterface(IID_IDataObject, (void**)ppDataObject) ;
    //return S_OK ;
    return E_NOTIMPL ;
}

HRESULT ReObject::DoVerb( LONG iVerb, 
                         LPMSG lpMsg, 
                         IOleClientSite *pActiveSite, 
                         LONG lIndex, 
                         HWND hWndParent, 
                         LPCRECT lprcPosRect )
{
    return S_OK ;
}

HRESULT ReObject::EnumVerbs( IEnumOLEVERB **ppEnumOleVerb )
{
    return S_OK ;
}

HRESULT ReObject::Update()
{
    return S_OK ;
}

HRESULT ReObject::IsUpToDate()
{
    return S_OK ;
}

HRESULT ReObject::GetUserClassID( CLSID *pClsid )
{
    *pClsid = CLSID_ReObject ;
    return S_OK ;
}

HRESULT ReObject::GetUserType( DWORD dwFormOfType, LPOLESTR *pszUserType )
{
//     *pszUserType = (LPOLESTR)CoTaskMemAlloc(4 * sizeof(TCHAR)) ;
//     wcscpy(*pszUserType, _T("abc")) ;
    return S_OK ;
}

HRESULT ReObject::SetExtent( DWORD dwDrawAspect, SIZEL *psizel )
{
//     m_width = psizel->cx ;
//     m_height = psizel->cy ;
//     return S_OK ;
    return E_NOTIMPL;
}

HRESULT ReObject::GetExtent( DWORD dwDrawAspect, SIZEL *psizel )
{
    psizel->cx = m_width_in_himetric ;
    psizel->cy = m_height_in_himetric ;

    return S_OK ;
}

HRESULT ReObject::Advise( IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
    return S_OK ;
}

HRESULT ReObject::Unadvise( DWORD dwConnection )
{
    return S_OK ;
}

HRESULT ReObject::EnumAdvise( IEnumSTATDATA **ppEnumAdvise )
{
    return S_OK ;
}

HRESULT ReObject::GetMiscStatus( DWORD dwAspect, DWORD *pdwStatus )
{
    return S_OK ;
}

HRESULT ReObject::SetColorScheme( LOGPALETTE *pLogpal )
{
    return E_NOTIMPL ;
}

//////////////////////////////////////////////////////////////////////////
//IDataObject methods

//similar to GetClipboardData
HRESULT ReObject::GetData( FORMATETC *pFormatetcIn, STGMEDIUM *pMedium )
{
    if (pFormatetcIn->cfFormat == CF_UNICODETEXT)
    {
        wchar_t data[] = L"我是一个对象" ;
        HGLOBAL mem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(data)) ;
        wchar_t * p = (wchar_t*)GlobalLock(mem) ;
        memcpy(p, data, sizeof(data)) ;
        GlobalUnlock(mem) ;

        pMedium->tymed = TYMED_HGLOBAL ;
        pMedium->pUnkForRelease = NULL ;
        pMedium->hGlobal = mem ;
    }

    return S_OK ;
}

HRESULT ReObject::GetDataHere( FORMATETC *pFormatetc, STGMEDIUM *pMedium )
{
    return DV_E_FORMATETC ;
}

//similar to IsClipboardFormatAvailable
HRESULT ReObject::QueryGetData( FORMATETC *pFormatetc )
{
	return DV_E_FORMATETC ;
}

//The word "canonical" means "the simplest form of something," 
//In general, the canonical FORMATETC should contain the most general information possible for a specific rendering.
//The simplest implementation of GetCanonicalFormatEtc copies the input structure to the output structure, 
//sets the output ptd field to NULL, and returns DATA_S_SAMEFORMATETC. 
//This says that the data object doesn't care about target devices for the specific FORMATETC. 
//If, on the other hand, the source does care, it returns NOERROR, filling the output FORMATETC as generally as it can.
HRESULT ReObject::GetCanonicalFormatEtc( FORMATETC *pFormatetcIn, FORMATETC *pFormatetcOut )
{
    return S_OK ;
}

//similar to SetClipboardData
HRESULT ReObject::SetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium, BOOL fRelease )
{
    return DATA_E_FORMATETC ;
}

//similar to EnumClipboardFormats
HRESULT ReObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc )
{
    *ppEnumFormatEtc = NULL;
    if ( dwDirection == DATADIR_GET )
    {
        FORMATETC fc;
        //fc.cfFormat = CF_BITMAP;
        fc.cfFormat = CF_UNICODETEXT ;
        fc.dwAspect = DVASPECT_CONTENT;
        fc.ptd = NULL;
        fc.lindex = -1;
        //fc.tymed = TYMED_GDI;
        fc.tymed = TYMED_HGLOBAL ;

        *ppEnumFormatEtc = new EnumFormatEtcImpl(1, &fc) ;
        //		*ppEnumFormatEtc = new xeditor::CEnumFormatEtc( 1,&fc );

        if ( *ppEnumFormatEtc )
        {
            (*ppEnumFormatEtc)->AddRef();
        }
    }
    return S_OK;
}

HRESULT ReObject::DAdvise( FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
    return S_OK ;
}

HRESULT ReObject::DUnadvise( DWORD dwConnection )
{
    return S_OK ;
}

HRESULT ReObject::EnumDAdvise( IEnumSTATDATA **ppEnumAdvise )
{
    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
//IPersistStorage methods;

HRESULT ReObject::GetClassID( CLSID *pClassID )
{
    *pClassID = CLSID_ReObject ;
    return S_OK ;
}

HRESULT ReObject::IsDirty()
{
    return S_OK ;
}

HRESULT ReObject::InitNew( IStorage *pStg )
{
    return S_OK ;
}

HRESULT ReObject::Load( IStorage *pStg )
{
    return S_OK ;
}

HRESULT ReObject::Save( IStorage *pStg, BOOL fSameAsLoad )
{
    return S_OK ;
}

HRESULT ReObject::SaveCompleted( IStorage *pStgNew )
{
    return S_OK ;
}

HRESULT ReObject::HandsOffStorage()
{
    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
//IViewObjectEx methods
//1.IViewObject methods
HRESULT ReObject::Draw( DWORD dwDrawAspect, 
                       LONG lIndex, 
                       void *pvAspect, 
                       DVTARGETDEVICE *ptd, 
                       HDC hdcTargetDev,
                       HDC hdcDraw, 
                       LPCRECTL lprcBounds, 
                       LPCRECTL lprcWBounds, 
                       BOOL (__stdcall *pfnContinue)( ULONG_PTR ), 
                       ULONG_PTR dwCotinue )
{
    ::SetRect(&m_draw_bound, lprcBounds->left, lprcBounds->top, lprcBounds->right, lprcBounds->bottom) ;

    if (!IsShowInEditJustByBound())
    {
        //m_image->Stop() ;
        MoveOutOfVisibleArea() ;
        return S_OK ;
    }
    else
    {
        // 标记为显示
        m_been_drawn_flag = true ;

        //m_image->Play() ;
        MoveIntoVisibleArea() ;
    }

    Draw(hdcDraw, *(RECT*)(lprcBounds)) ;

    return S_OK ;
}

// 绘制 - virtual
void ReObject::Draw(HDC hdcDraw, RECT const &draw_bound) 
{
    // 仅绘制边框和交叉线
    HPEN pen = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 255)) ;
    HGDIOBJ old_pen = ::SelectObject(hdcDraw, pen) ;

    ::MoveToEx(hdcDraw, draw_bound.left, draw_bound.top, NULL) ;
    ::LineTo(hdcDraw, draw_bound.right - 1, draw_bound.top) ;
    ::LineTo(hdcDraw, draw_bound.right - 1, draw_bound.bottom - 1) ;
    ::LineTo(hdcDraw, draw_bound.left, draw_bound.bottom - 1) ;
    ::LineTo(hdcDraw, draw_bound.left, draw_bound.top) ;
    ::LineTo(hdcDraw, draw_bound.right - 1, draw_bound.bottom - 1) ;

    ::MoveToEx(hdcDraw, draw_bound.right - 1, draw_bound.top, NULL) ;
    ::LineTo(hdcDraw, draw_bound.left, draw_bound.bottom - 1) ;

    ::SelectObject(hdcDraw, old_pen) ;
    ::DeleteObject(pen) ;
}

HRESULT ReObject::GetColorSet( DWORD dwDrawAspect,
                                      LONG lindex,
                                      void *pvAspect,
                                      DVTARGETDEVICE *ptd,
                                      HDC hicTargetDev,
                                      LOGPALETTE **ppColorSet )
{
    return S_OK ;
}

HRESULT ReObject::Freeze( DWORD dwDrawAspect, LONG lIndex, void *pvAspect, DWORD *pdwFreeze )
{
    return S_OK ;
}

HRESULT ReObject::Unfreeze( DWORD dwFreeze )
{
    return S_OK ;
}

HRESULT ReObject::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink )
{
    m_advise_sink = pAdvSink ;
    return S_OK ;
}

HRESULT ReObject::GetAdvise( DWORD *pAspects, DWORD *pAdvf, IAdviseSink **pAdvSink )
{
    *pAdvSink = m_advise_sink ;
    return S_OK ;
}

//2.IViewObject2 methods
HRESULT ReObject::GetExtent(  DWORD dwAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel )
{
    lpsizel->cx = HimetricXtoDX(m_width_in_himetric, s_x_pixel_per_inch) ;
    lpsizel->cy = HimetricXtoDX(m_height_in_himetric, s_y_pixel_per_inch) ;

    return S_OK ;
}

//3.IViewObjectEx methods
HRESULT ReObject::GetRect( DWORD dwAspect, LPRECTL pRect )
{
    return S_OK ;
}

HRESULT ReObject::GetViewStatus( DWORD *pdwStatus )
{
    return S_OK ;
}

HRESULT ReObject::QueryHitPoint( DWORD dwAspect, 
                                LPCRECT pRectBounds, 
                                POINT ptlLoc, 
                                LONG lCloseHint, 
                                DWORD *pHitResult )
{
    return S_OK ;
}

HRESULT ReObject::QueryHitRect( DWORD dwAspect,
                               LPCRECT pRectBound,
                               LPCRECT pRectLoc,
                               LONG lCloseHint,
                               DWORD *pHitResult )
{
    return S_OK ;
}

HRESULT ReObject::GetNaturalExtent( DWORD dwAspect,
                                   LONG lIndex,
                                   DVTARGETDEVICE *ptd,
                                   HDC hicTargetDev,
                                   DVEXTENTINFO *pExtentInfo,
                                   LPSIZEL pSizel )
{
    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
//IOleInPlaceActiveObject methods

//1.IOleWindow methods

HRESULT ReObject::GetWindow( HWND *phWnd )
{
    return S_OK ;
}

HRESULT ReObject::ContextSensitiveHelp( BOOL fEnterMode )
{
    return S_OK ;
}

//2.IOleInPlaceActiveObject methods

HRESULT ReObject::TranslateAccelerator( LPMSG lpMsg )
{
    return S_OK ;
}

HRESULT ReObject::OnFrameWindowActivate( BOOL fActive )
{
    return S_OK ;
}

HRESULT ReObject::OnDocWindowActivate( BOOL fActive )
{
    return S_OK ;
}

HRESULT ReObject::ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow )
{
    return S_OK ;
}

HRESULT ReObject::EnableModeless( BOOL fEnable )
{
    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
//IOleInPlaceObjectWindowless methods

//1.IOleInPlaceObject methods

HRESULT ReObject::InPlaceDeactivate()
{
    return S_OK ;
}

HRESULT ReObject::UIDeactivate()
{
    return S_OK ;
}

HRESULT ReObject::SetObjectRects( LPCRECT lprcPosRect, LPCRECT lprcClipRect )
{
    return S_OK ;
}

HRESULT ReObject::ReactivateAndUndo()
{
    return S_OK ;
}

//2.IOleInPlaceObjectWindowless methods

HRESULT ReObject::OnWindowMessage( UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult )
{
    return S_OK ;
}

HRESULT ReObject::GetDropTarget( IDropTarget **ppDropTarget )
{
    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
// IClipboardDataObject methods
// 转换成剪切板数据 - virtual

bool ReObject::QueryTextStreamClipboardData(
    Clipboard::Format format,
    std::wstring &data
    ) 
{
    return false ;
}
