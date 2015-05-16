#include "stdafx.h"

#include "common/utils/ImageEx/ImageResource.h"

#include "tomEdit.h"
#include "tomEditCallback.h"
#include "utils.h"
#include "ReObject.h"
#include "ReImage.h"
#include "common/MemoryLeakChecker.h"
EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

// {13E670F5-1A5A-11cf-ABEB-00AA00B65EA1}
EXTERN_C const GUID IID_ITextHost = 
{ 0x13e670f4, 0x1a5a, 0x11cf, { 0xab, 0xeb, 0x0, 0xaa, 0x0, 0xb6, 0x5e, 0xa1 } };

// {13E670F5-1A5A-11cf-ABEB-00AA00B65EA1}
EXTERN_C const GUID IID_ITextHost2 = 
{ 0x13e670f5, 0x1a5a, 0x11cf, { 0xab, 0xeb, 0x0, 0xaa, 0x0, 0xb6, 0x5e, 0xa1 } };

LONG tomEdit::s_XPixelPerInch = 0 ;
LONG tomEdit::s_YPixelPerInch = 0 ;

tomEdit::tomEdit() 
: 
m_ref_count(0)
, m_services(NULL)
, m_border(NULL)
, m_scroll_width(0)
, m_rich_edit_ole(NULL)
, m_call_back(NULL)
{
    ::SetRect(&m_border_inset, 1, 1, 1, 1) ;
    ::SetRect(&m_view_inset, 0, 0, 0, 0) ;

    m_test_object = NULL ;
}

tomEdit::~tomEdit() 
{
}

bool tomEdit::SetBackgroundTransparent(bool transparent) 
{
    bool old = m_background_transparent ;
    if (m_background_transparent ^ transparent)
    {
        m_background_transparent = transparent ;

        if (m_services != NULL)
        {
            m_services->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, TXTBIT_BACKSTYLECHANGE) ;
        }
    }

    return old ;
}

COLORREF tomEdit::SetBackgroundColor(bool use_system_color, COLORREF color) 
{
    COLORREF old_color ;
    if (m_use_system_background_color)
    {
        old_color = GetSysColor(COLOR_WINDOW) ;

        if (!use_system_color)
        {
            m_use_system_background_color = false ;
            m_background_color = color ;
        }
    }
    else
    {
        old_color = m_background_color ;

        if (use_system_color)
        {
            m_use_system_background_color = true ;
        }
        else
        {
            m_background_color = color ;
        }
    }

    return old_color ;
}

COLORREF tomEdit::GetBackgroundColor() const
{
    if (m_use_system_background_color)
        return GetSysColor(COLOR_WINDOW) ;
    else
        return m_background_color ;
}

COLORREF tomEdit::SetHighLightTextColor(bool use_system_color, COLORREF color) 
{
    COLORREF old_color ;
    if (m_use_system_highlight_text_color)
    {
        old_color = GetSysColor(COLOR_HIGHLIGHTTEXT) ;

        if (!use_system_color)
        {
            m_use_system_highlight_text_color = false ;
            m_highlight_text_color = color ;
        }
    }
    else
    {
        old_color = m_highlight_text_color ;

        if (use_system_color)
        {
            m_use_system_highlight_text_color = true ;
        }
        else
        {
            m_highlight_text_color = color ;
        }
    }

    return old_color ;
}

COLORREF tomEdit::GetHighLightTextColor() const
{
    if (m_use_system_highlight_text_color)
        return GetSysColor(COLOR_HIGHLIGHTTEXT) ;
    else
        return m_highlight_text_color ;
}

COLORREF tomEdit::SetHighLightTextBackgroundColor(bool use_system_color, COLORREF color) 
{
    COLORREF old_color ;
    if (m_use_system_highlight_text_background_color)
    {
        old_color = GetSysColor(COLOR_HIGHLIGHT) ;

        if (!use_system_color)
        {
            m_use_system_highlight_text_background_color = false ;
            m_highlight_text_background_color = color ;
        }
    }
    else
    {
        old_color = m_highlight_text_background_color ;

        if (use_system_color)
        {
            m_use_system_highlight_text_background_color = true ;
        }
        else
        {
            m_highlight_text_background_color = color ;
        }
    }

    return old_color ;
}

COLORREF tomEdit::GetHighLightTextBackgroundColor() const
{
    if (m_use_system_highlight_text_background_color)
        return GetSysColor(COLOR_HIGHLIGHT) ;
    else
        return m_highlight_text_background_color ;
}

IRichEditOle * tomEdit::GetIRichEditOle() 
{
    if (m_services == NULL)
        return NULL ;

    if (m_rich_edit_ole != NULL)
        return m_rich_edit_ole ;

    LRESULT lresult = 0 ;
    if (S_OK != m_services->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&m_rich_edit_ole, &lresult))
        return NULL ;

    return m_rich_edit_ole ;
}

bool tomEdit::SetOLECallback(IRichEditOleCallback* pCallback) 
{
    if (m_services == NULL)
        return false ;

    LRESULT lresult = 0 ;
    if (S_OK != m_services->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback, &lresult))
        return false ;

    return true ;
}

void tomEdit::EmptyUndoBuffer() 
{
    if (m_services != NULL)
    {
        m_services->TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, NULL) ;
    }
}

WORD tomEdit::GetSelectionType() const
{
    LRESULT result = SEL_EMPTY ;
    if (m_services != NULL)
    {
        m_services->TxSendMessage(EM_SELECTIONTYPE, 0, 0, &result) ;
    }

    return (WORD)result ;
}

HRESULT tomEdit::GetObject(LONG iob, REOBJECT * reobject, DWORD flags) 
{
    HRESULT hr = E_FAIL ;
    if (m_rich_edit_ole != NULL)
    {
        hr = m_rich_edit_ole->GetObject(iob, reobject, flags) ;
    }

    return hr ;
}

// char_position值为REO_CP_SELECTION时表示选择文本被替换为图像对象
// 这里没检测imge_file_path是否有效图片路径
BOOL tomEdit::InsertImageObject(LONG char_position, std::wstring const image_file_path) 
{
    IRichEditOle * rich_edit_ole = GetIRichEditOle() ;
    if (rich_edit_ole == NULL)
        return FALSE ;

    IOleClientSite * obj_client_site = NULL ;
    rich_edit_ole->GetClientSite(&obj_client_site) ;
    if (obj_client_site == NULL)
        return FALSE ;

    ReImage * image = new ReImage ;
    image->SetHost(GetCenter()->GetHost()) ;
    m_object_group.insert(image) ;

    image->InitFromImageFile(image_file_path) ;

    REOBJECT reo;
    memset( &reo, 0, sizeof( reo ) );
    reo.cbStruct = sizeof( reo );

    CLSID classID;
    if ( image->GetUserClassID( &classID ) != S_OK)
        classID = CLSID_NULL;
    reo.clsid = classID;
    reo.cp = char_position ;
    reo.poleobj = image;
    reo.pstg = NULL;
    reo.polesite = obj_client_site ;

    reo.sizel.cx = reo.sizel.cy = 0 ;
    reo.dvaspect = DVASPECT_CONTENT;
    reo.dwFlags = REO_BELOWBASELINE /*| REO_RESIZABLE*/ ;
    reo.dwUser = 0 ;
    image->SetClientSite( obj_client_site );

    HRESULT hr = rich_edit_ole->InsertObject( &reo );

    obj_client_site->Release() ;

    return SUCCEEDED(hr) ;
}

void tomEdit::ReplaceSel(wchar_t const * new_text, BOOL can_undo /*= FALSE*/) 
{
    if (m_services != NULL)
    {
        m_services->TxSendMessage(EM_REPLACESEL, (WPARAM)can_undo, (LPARAM)new_text, NULL) ;
    }
}

void tomEdit::RemoveObject(IOleObject * object) 
{
    m_object_group.erase(object) ;
}

// void tomEdit::RevokeDragDrop() 
// {
//     ::RevokeDragDrop(GetCenter()->GetHost()) ;
// }

// void tomEdit::RegisterDragDrop() 
// {
//     IDropTarget * dt = NULL ;
//     if (m_services != NULL && S_OK == m_services->TxGetDropTarget(&dt))
//     {
//         HWND host = GetCenter()->GetHost() ;
//         ::RegisterDragDrop(host, dt) ;
//         dt->Release() ;
//     }
// }

// 测试接口
BOOL tomEdit::TestInsertObject() 
{
    IRichEditOle * rich_edit_ole = GetIRichEditOle() ;
    if (rich_edit_ole == NULL)
        return FALSE ;

    IOleClientSite * obj_client_site = NULL ;
    rich_edit_ole->GetClientSite(&obj_client_site) ;
    if (obj_client_site == NULL)
        return FALSE ;

    m_test_object = new ReImage ;
    m_test_object->SetHost(GetCenter()->GetHost()) ;

    m_object_group.insert(m_test_object) ;

    //m_test_object->InitFromImageFile(_T("C:\\2.gif")) ;
    m_test_object->InitFromImageFile(_T("C:\\x.jpg")) ;
    //m_test_object->SetImagePath(_T("F:\\Program\\Draw\\GetDIBitsUsage\\res\\bitmap1.bmp")) ;
    //m_test_object->SetImagePath(_T("F:\\图片\\风景\\退去喧嚣，我们在路上 - 西藏\\1315471834717.jpg")) ;

    REOBJECT reo;
    memset( &reo, 0, sizeof( reo ) );
    reo.cbStruct = sizeof( reo );

    CLSID classID;
    if ( m_test_object->GetUserClassID( &classID ) != S_OK)
        classID = CLSID_NULL;
    reo.clsid = classID;
    reo.cp = 0;
    reo.poleobj = m_test_object;
    reo.pstg = NULL;
    reo.polesite = obj_client_site ;

//     SIZEL sizel;
//     sizel.cx = DXtoHimetricX(100, s_XPixelPerInch) ;
//     sizel.cy = DYtoHimetricY(80, s_YPixelPerInch) ; // let rich edit determine initial size
//     reo.sizel = sizel;

    reo.sizel.cx = reo.sizel.cy = 0 ;
    reo.dvaspect = DVASPECT_CONTENT;
    reo.dwFlags = REO_BELOWBASELINE /*| REO_RESIZABLE*/ ;
    reo.dwUser = 0 ;
    m_test_object->SetClientSite( obj_client_site );

    HRESULT hr = rich_edit_ole->InsertObject( &reo );

    obj_client_site->Release() ;

    return SUCCEEDED(hr) ;
}

BOOL tomEdit::TestChangeObjectSize(int cx, int cy) 
{
    IRichEditOle * rich_edit_ole = GetIRichEditOle() ;
    if (rich_edit_ole == NULL)
        return FALSE ;

    m_test_object->AutoAdjustSizeBySpaceWidth(cx) ;
    HRESULT hr = rich_edit_ole->SetDvaspect(0, DVASPECT_CONTENT) ;

//     IOleClientSite * client_site = NULL ;
//     m_test_object->GetClientSite(&client_site) ;

    return SUCCEEDED(hr) ;
}

BOOL tomEdit::TestChangeObjectSizeSelf(int cx, int cy) 
{
//     IRichEditOle * rich_edit_ole = GetIRichEditOle() ;
//     if (rich_edit_ole == NULL)
//         return FALSE ;

    //m_test_object->SetSize(cx, cy) ;
    m_test_object->AutoAdjustSizeBySpaceWidth(cx) ;

    /*
    IOleClientSite * client_site = NULL ;
    m_test_object->GetClientSite(&client_site) ;

    if (client_site != NULL)
    {
        IAdviseSink * advise_sink = NULL ;
        client_site->QueryInterface(IID_IAdviseSink, (void**)&advise_sink) ;

        if (advise_sink != NULL)
        {
            advise_sink->OnViewChange(DVASPECT_CONTENT, -1) ;
        }
    }
    */

    IAdviseSink * advise_sink = NULL ;
    HRESULT hr = m_test_object->GetAdvise(NULL, NULL, &advise_sink) ;
    if (SUCCEEDED(hr) && advise_sink != NULL)
    {
        advise_sink->OnViewChange(DVASPECT_CONTENT, -1) ;
    }

/*    IOleClientSite * client_site = NULL ;
    m_test_object->GetClientSite(&client_site) ;

    IOleInPlaceSite * inplace_site = NULL ;
    client_site->QueryInterface(IID_IOleInPlaceSite, (void**)&inplace_site) ;

    inplace_site->OnUIActivate() ;*/

    return TRUE ;
}

void tomEdit::TestGetObjectBound() 
{
    IOleClientSite * client_site = NULL ;
    m_test_object->GetClientSite(&client_site) ;

    IOleInPlaceSite * inplace_site = NULL ;
    client_site->QueryInterface(IID_IOleInPlaceSite, (void**)&inplace_site) ;

    RECT bound, clip ;
    HRESULT hr = inplace_site->GetWindowContext(NULL, NULL, &bound, &clip, NULL) ;

}

void tomEdit::TestHideObject() 
{
    IOleClientSite * client_site = NULL ;
    m_test_object->GetClientSite(&client_site) ;

    client_site->OnShowWindow(FALSE) ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Interface

BOOL tomEdit::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
    if (!Init())
        return FALSE ;

    m_vert_scroll_bar.SetImage(IDI_SCROLLBAR_PNG) ;
    if (!m_vert_scroll_bar.Create(
        WS_VISIBLE | WLScrollBar::STYLE_RIGHTALIGN | WLScrollBar::STYLE_VERT,
        0, 0, cx, cy,
        this, (UINT)-1))
    {
        return FALSE ;
    }
    m_scroll_width = m_vert_scroll_bar.GetScrollWidth() ;

    m_horz_scroll_bar.SetImage(IDI_SCROLLBAR_PNG) ;
    if (!m_horz_scroll_bar.Create(
        WS_VISIBLE | WLScrollBar::STYLE_BOTTOMALIGN | WLScrollBar::STYLE_HORZ,
        0, 0, cx, cy,
        this, (UINT)-1))
    {
        return FALSE ;
    }

    SCROLLINFO si ;
    si.fMask = SIF_ALL ;
    si.nMin = 0 ;
    si.nMax = 0 ;
    si.nPos = 0 ;
    si.nPage = cy ;
    m_vert_scroll_bar.SetScrollInfo(&si) ;
    si.nPage = cx ;
    m_horz_scroll_bar.SetScrollInfo(&si) ;

    return TRUE ;
}

void tomEdit::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
    m_border->SetSize(szNew.cx, szNew.cy) ;

    m_vert_scroll_bar.SetWndLessPos(
        szNew.cx - m_border_inset.right - m_scroll_width,
        m_border_inset.top,
        0,
        szNew.cy - m_border_inset.top - m_border_inset.bottom - (m_show_horz_scroll_bar ? m_scroll_width : 0),
        SWLP_X | SWLP_Y | SWLP_CY
        ) ;
    m_horz_scroll_bar.SetWndLessPos(
        m_border_inset.left,
        szNew.cy - m_border_inset.bottom - m_scroll_width,
        szNew.cx - m_border_inset.left - m_border_inset.right - (m_show_vert_scroll_bar ? m_scroll_width : 0),
        0,
        SWLP_X | SWLP_Y | SWLP_CX
        ) ;

    RECT text_bound ;
    GetTextBound(text_bound) ;
    SCROLLINFO si ;
    si.fMask = SIF_PAGE ;
    si.nPage = text_bound.bottom - text_bound.top ;
    m_vert_scroll_bar.SetScrollInfo(&si) ;
    si.nPage = text_bound.right - text_bound.left ;
    m_horz_scroll_bar.SetScrollInfo(&si) ;

    WPARAM wParam = SIZE_RESTORED ;
    LPARAM lParam = MAKELPARAM(szNew.cx, szNew.cy) ;
    m_services->TxSendMessage(WM_SIZE, wParam, lParam, 0) ;

    //m_services->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE) ;
    //m_services->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE) ;
    //m_services->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE) ;

    m_services->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE) ;
}

void tomEdit::OnDestroy() 
{
    RevokeDragDrop() ;

    if (m_rich_edit_ole != NULL)
    {
        m_rich_edit_ole->Release() ;
        m_rich_edit_ole = NULL ;
    }

    if (m_call_back != NULL)
    {
        m_call_back->Release() ;
        m_call_back = NULL ;
    }

    if (m_services != NULL)
    {
        m_services->Release() ;
        m_services = NULL ;
    }

    if (m_border != NULL)
    {
        m_border->Release() ;
        m_border = NULL ;
    }
}

BOOL tomEdit::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
    BOOL consumed = FALSE ;

    switch (message)
    {
    case WM_SETCURSOR :
        {
            POINT pt ;
            ::GetCursorPos(&pt) ;
            ::ScreenToClient(GetCenter()->GetHost(), &pt) ;
            //RECT bound_in_host ;
            //GetRectInHost(&bound_in_host) ;
            RECT client_bound ;
            GetClientBound(client_bound) ;

            if (::PtInRect(&client_bound, pt))
            {
                HDC hdc = ::GetDC(GetCenter()->GetHost()) ;

                m_services->OnTxSetCursor(
                    DVASPECT_CONTENT,	
                    -1,
                    NULL,
                    NULL,
                    hdc,
                    NULL,
                    //&bound_in_host,
                    NULL,
                    pt.x, 
                    pt.y
                    );

                ::ReleaseDC(GetCenter()->GetHost(), hdc) ;
                consumed = TRUE ;
            }
        }
        break ;

    case WM_MOUSEWHEEL :
        {
            UINT flags = LOWORD(wParam) ;
            short zDelta = HIWORD(wParam) ;
            if (flags == 0)
            {
                // 1. 缺省的滚轮操作
                /*if (zDelta > 0)
                {
                    m_services->TxSendMessage(WM_VSCROLL, SB_LINEUP, 0, pResult) ;
                }
                else
                {
                    m_services->TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0, pResult) ;
                }*/

                // 2. 平滑滚动，即每次滚动固定像素，而不是滚动一整行
                // 获得当前位置
                LONG min, max, pos, page ;
                BOOL enable = FALSE ;
                m_services->TxGetVScroll(&min, &max, &pos, &page, &enable) ;
                LONG line_height = page / 4 ;
                if (enable)
                {
                    if (zDelta > 0)
                    {
                        if (pos <= line_height)
                            pos = 0 ;
                        else
                            pos -= line_height ;
                    }
                    else
                    {
                        pos += line_height ;
                        if (pos >= max)
                            pos = max - 1 ;
                    }
                    WPARAM cmb_wParam = MAKEWPARAM(SB_THUMBPOSITION, pos) ;
                    m_services->TxSendMessage(WM_VSCROLL, cmb_wParam, 0, NULL) ;
                }
            }
            else
            {
                /*hr = */m_services->TxSendMessage(WM_MOUSEWHEEL, wParam, lParam, pResult) ;
            }
            consumed = TRUE ;
        }
        break ;

    case WM_VSCROLL :
        {
            OnVScroll((LPSCROLLMSGINFO)wParam, (WLScrollBar*)lParam) ;
            consumed = TRUE ;
        }
        break ;

    case WM_HSCROLL :
        {
            OnHScroll((LPSCROLLMSGINFO)wParam, (WLScrollBar*)lParam) ;
            consumed = TRUE ;
        }
        break ;

        // 允许输入字符，才能允许启用输入法
    case WM_GETDLGCODE :
        {
            *pResult |= (DLGC_WANTCHARS | DLGC_HASSETSEL) ;
            consumed = TRUE ;
        }
        break ;

    case WM_TIMER :
        {
            if (!m_caret.OnTimer(wParam))
                m_services->TxSendMessage(message, wParam, lParam, pResult) ;
            consumed = TRUE ;
        }
        break ;

    case WM_KILLFOCUS :
        {
            m_caret.DestroyCaret() ;
            m_services->TxSendMessage(message, wParam, lParam, pResult) ;
            consumed = TRUE ;
        }
        break ;

    default :
        if (S_FALSE != (m_services->TxSendMessage(message, wParam, lParam, pResult)))
        {
            consumed = TRUE ;
        }
    }

    return consumed ;
}

void tomEdit::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
    // 保存绘制之前的DC的clip区域
    int save_mark = ::SaveDC(hDC) ;

    RECT bound ;
    GetRectInHost(&bound) ;
    HRGN hClipRgn = ::CreateRectRgnIndirect(&bound) ;
    ::ExtSelectClipRgn(hDC, hClipRgn, RGN_AND) ;
    ::DeleteObject(hClipRgn) ;

    std::set<IOleObject*>::iterator iter ;
    for (iter = m_object_group.begin() ; iter != m_object_group.end() ; ++ iter)
    {
        ((ReObject*)(*iter))->ResetHide(rcUpdate) ;
    }

    //RECT bound_in_host ;
    //GetRectInHost(&bound_in_host) ;
    //TRACE(_T("tomEdit::OnDraw: %d\n"), GetTickCount()) ;
    // 这里也可以不设置客户区区域，系统会自动调用TxGetClientRect获得该范围
    m_services->TxDraw(
        DVASPECT_CONTENT,  		// Draw Aspect
        //-1
        0,						// Lindex
        NULL,					// Info for drawing optimazation
        NULL,					// target device information
        (HDC) hDC,			// Draw device HDC
        NULL, 				   	// Target device HDC
        //(RECTL *) &bound_in_host,			// Bounding client rectangle
        NULL,
        NULL, 					// Clipping rectangle for metafiles
        (RECT *) &rcUpdate,		// Update rectangle
        NULL, 	   				// Call back function
        NULL,					// Call back parameter
        TXTVIEW_ACTIVE
        ) ;

    for (iter = m_object_group.begin() ; iter != m_object_group.end() ; ++ iter)
    {
        ((ReObject*)(*iter))->CheckShowOrHide() ;
    }

    m_caret.Draw(hDC, rcUpdate) ;

    m_border->Draw(hDC, rcUpdate) ;

    // 恢复绘制之前的DC的clip区域
    ::RestoreDC(hDC, save_mark) ;
}

bool tomEdit::Init() 
{
    WLDC dc(this) ;
    s_XPixelPerInch = dc.GetDeviceCaps(LOGPIXELSX) ;
    s_YPixelPerInch = dc.GetDeviceCaps(LOGPIXELSY) ;

    m_ref_count = 1 ;

    m_show_vert_scroll_bar = false ;
    m_show_horz_scroll_bar = false ;
    m_background_transparent = true ;
    m_use_system_background_color = true ;
    m_use_system_highlight_text_color = true ;
    m_use_system_highlight_text_background_color = true ;

    m_word_wrap = !(GetStyle() & (WS_HSCROLL | ES_AUTOHSCROLL)) ;

    HRESULT hr ;
    IUnknown * pUnknown ;

    // 由于CreateTextServices会触发TxGetCharFormat和TxGetParaFormat，
    // 所以要在此之前初始化CharFormat和ParaFormat

    // 初始化CharFormat
    InitDefaultCharFormat() ;

    // 初始化ParaFormat
    InitDefaultParaFormat() ;

    RECT bound_in_host ;
    GetRectInHost(&bound_in_host) ;

    if (FAILED(CreateBorder(IID_IRectangleBorder, (IUnknown**)&m_border)))
        return FALSE ;
    m_border->SetPos(bound_in_host.left, bound_in_host.top) ;
    m_border->SetSize(bound_in_host.right - bound_in_host.left, bound_in_host.bottom - bound_in_host.top) ;
    m_border->SetInset(m_border_inset.left, m_border_inset.top, m_border_inset.right, m_border_inset.bottom) ;

    if (FAILED(CreateTextServices(NULL, this, &pUnknown)))
        return false ;

    hr = pUnknown->QueryInterface(IID_ITextServices, (void**)&m_services) ;
    pUnknown->Release() ;

    if (FAILED(hr))
        return false ;

    // 输入的客户区矩形范围似乎无用，总是会调用ITextHost::TxGetClientRect来获取区域
    if (FAILED(m_services->OnTxInPlaceActivate(&bound_in_host)))
        return false ;

    LRESULT mask ;
    m_services->TxSendMessage(EM_GETEVENTMASK, 0, 0, &mask) ;
    mask |= ENM_SELCHANGE ;
    m_services->TxSendMessage(EM_SETEVENTMASK, 0, mask, NULL) ;

    m_call_back = new tomEditCallback(this) ;
    SetOLECallback(m_call_back) ;

    // 注册拖拽
    IDropTarget * dt = NULL ;
    if (S_OK == m_services->TxGetDropTarget(&dt))
    {
        RegisterDragDrop(dt) ;
        dt->Release() ;
    }

    return true ;
}

void tomEdit::InitDefaultCharFormat() 
{
    ZeroMemory(&m_default_char_format, sizeof(m_default_char_format)) ;

    m_default_char_format.cbSize = sizeof(m_default_char_format) ;
    m_default_char_format.dwMask = CFM_ALL ;
    //m_default_char_format.dwMask &= ~CFM_BACKCOLOR ;

    HFONT defult_font = (HFONT)::SendMessage(GetCenter()->GetHost(), WM_GETFONT, 0, 0) ;
    LOGFONT lf ;
    ::GetObject(defult_font, sizeof(LOGFONT), &lf) ;

    if (lf.lfWeight >= FW_BOLD)
        m_default_char_format.dwEffects |= CFE_BOLD ;
    if(lf.lfItalic)
        m_default_char_format.dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        m_default_char_format.dwEffects |= CFE_UNDERLINE;
    if(lf.lfStrikeOut)
        m_default_char_format.dwEffects |= CFE_STRIKEOUT;

    WLDC dc(this) ;
    LONG yPixPerInch = dc.GetDeviceCaps(LOGPIXELSY) ;
    // yHeight的单位是twip，1 inch = 1440 twips(参msdn)
    //m_default_char_format.yHeight = lf.lfHeight * 1440 / yPixPerInch ;
    m_default_char_format.yHeight = abs(lf.lfHeight) * 1440 / yPixPerInch ;

    m_default_char_format.yOffset = 0 ;

    m_default_char_format.crTextColor = RGB(0, 0, 255) ;

    // 当m_default_char_format.dwMask里没包含CFM_BACKCOLOR时，背景色无效；
    // 若包含CFM_BACKCOLOR，而编辑框的背景模式为透明，则当文字背景色和编辑框背景色一样时，文字背景透明；
    // 不知道在msdn上是否有说明
    //m_default_char_format.crBackColor = RGB(255, 0, 0) ;

    m_default_char_format.bCharSet = lf.lfCharSet ;
    m_default_char_format.bPitchAndFamily = lf.lfPitchAndFamily ;

    wcscpy_s(m_default_char_format.szFaceName, _countof(m_default_char_format.szFaceName), lf.lfFaceName) ;
}

void tomEdit::InitDefaultParaFormat() 
{
    ZeroMemory(&m_default_para_format, sizeof(m_default_para_format)) ;
    m_default_para_format.cbSize = sizeof(m_default_para_format) ;

    m_default_para_format.dwMask = PFM_ALL;
    m_default_para_format.wAlignment = PFA_LEFT;
    m_default_para_format.cTabCount = 1;
    m_default_para_format.rgxTabs[0] = lDefaultTab;
}

// 获得除去border剩余的区域
void tomEdit::GetNoborderBound(RECT &noborder_bound) 
{
    GetRectInHost(&noborder_bound) ;
    noborder_bound.left += m_border_inset.left ;
    noborder_bound.top += m_border_inset.top ;
    noborder_bound.right -= m_border_inset.right ;
    noborder_bound.bottom -= m_border_inset.bottom ;
}

// 获得客户区区域，即由ITextService负责绘制的区域
void tomEdit::GetClientBound(RECT &client_bound) 
{
    GetNoborderBound(client_bound) ;

    if (m_show_horz_scroll_bar)
        client_bound.bottom -= m_scroll_width ;
    if (m_show_vert_scroll_bar)
        client_bound.right -= m_scroll_width ;
}

// 获得文本区域，滚动条的范围与此区域对应
void tomEdit::GetTextBound(RECT &text_bound) 
{
    GetClientBound(text_bound) ;
    text_bound.left += (m_view_inset.left) ;
    text_bound.top += (m_view_inset.top) ;
    text_bound.right -= (m_view_inset.right) ;
    text_bound.bottom -= (m_view_inset.bottom) ;
}

void tomEdit::OnVScroll(LPSCROLLMSGINFO pSmi, WLScrollBar*) 
{
    UINT nSBCode = pSmi->nSBCode ;
    int nPos = pSmi->nPos ;
    TRACE(_T("OnVScroll: %d, %d\n"), nSBCode, nPos) ;
    WPARAM wParam = MAKEWPARAM(nSBCode, nPos) ;

    if (m_services != NULL)
        m_services->TxSendMessage(WM_VSCROLL, wParam, 0, NULL) ;
}

void tomEdit::OnHScroll(LPSCROLLMSGINFO pSmi, WLScrollBar*) 
{
    UINT nSBCode = pSmi->nSBCode ;
    int nPos = pSmi->nPos ;
    WPARAM wParam = MAKEWPARAM(nSBCode, nPos) ;

    if (m_services != NULL)
        m_services->TxSendMessage(WM_HSCROLL, wParam, 0, NULL) ;
}

//////////////////////////////////////////////////////////////////////////
// IUnknown Interface

HRESULT _stdcall tomEdit::QueryInterface(REFIID riid, void **ppvObject)
{
    bool support = false ;
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown))
    {
        support = true ;
        *ppvObject = (IUnknown *)this ;
    }
    else if (IsEqualIID(riid, IID_ITextHost)) 
    {
        support = true ;
        *ppvObject = (ITextHost *) this;
    }
//     else if (IsEqualIID(riid, IID_ITextHost2))
//     {
//         support = true ;
//         *ppvObject = (ITextHost2 *) this;
//     }

    if (support)
    {
        AddRef() ;
        hr = S_OK ;
    }

    return hr;
}

ULONG _stdcall tomEdit::AddRef(void)
{
    return ++ m_ref_count ;
}

ULONG _stdcall tomEdit::Release(void)
{
    ULONG result = -- m_ref_count ;

    if (m_ref_count == 0)
    {
        delete this ;
    }

    return result ;
}

//////////////////////////////////////////////////////////////////////////
// ITextHost Interface
//@cmember Get the DC for the host
HDC tomEdit::TxGetDC() 
{
    return ::GetDC(GetCenter()->GetHost()) ;
}

//@cmember Release the DC gotten from the host
INT tomEdit::TxReleaseDC(HDC hdc) 
{
    return ::ReleaseDC(GetCenter()->GetHost(), hdc) ;
}

//@cmember Show the scroll bar
BOOL tomEdit::TxShowScrollBar(INT fnBar, BOOL fShow) 
{
    // 未处理
    if (fnBar == SB_HORZ)
    {
        m_show_horz_scroll_bar = !!fShow ;
        RECT text_bound ;
        GetTextBound(text_bound) ;
        SCROLLINFO si ;
        si.fMask = SIF_PAGE ;
        si.nPage = text_bound.bottom - text_bound.top ;
        m_vert_scroll_bar.SetScrollInfo(&si) ;

        if (fShow)
        {
            // 修改垂直滚动条的区域大小和页大小
            m_vert_scroll_bar.SetWndLessPos(0, 0, 0, -m_scroll_width, SWLP_CY_OFFSET) ;
        }
        else
        {
            si.fMask = SIF_RANGE | SIF_POS ;
            si.nMin = si.nMax = si.nPos = 0 ;
            m_horz_scroll_bar.SetScrollInfo(&si) ;
            m_horz_scroll_bar.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;

            m_vert_scroll_bar.SetWndLessPos(0, 0, 0, m_scroll_width, SWLP_CY_OFFSET) ;
        }
    }
    else
    {
        m_show_vert_scroll_bar = !!fShow ;
        RECT text_bound ;
        GetTextBound(text_bound) ;
        SCROLLINFO si ;
        si.fMask = SIF_PAGE ;
        si.nPage = text_bound.right - text_bound.left ;
        m_horz_scroll_bar.SetScrollInfo(&si) ;

        if (fShow)
        {
            m_horz_scroll_bar.SetWndLessPos(0, 0, -m_scroll_width, 0, SWLP_CX_OFFSET) ;
        }
        else
        {
            si.fMask = SIF_RANGE | SIF_POS ;
            si.nMin = si.nMax = si.nPos = 0 ;
            m_vert_scroll_bar.SetScrollInfo(&si) ;
            m_vert_scroll_bar.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;

            m_horz_scroll_bar.SetWndLessPos(0, 0, m_scroll_width, 0, SWLP_CX_OFFSET) ;
        }
    }
    return TRUE ;
}

//@cmember Enable the scroll bar
BOOL tomEdit::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags) 
{
    // 未处理
    return TRUE ;
}

//@cmember Set the scroll range
BOOL tomEdit::TxSetScrollRange(
                               INT fnBar,
                               LONG nMinPos,
                               INT nMaxPos,
                               BOOL fRedraw) 
{
    SCROLLINFO si ;
    si.cbSize = sizeof(si) ;
    si.fMask = SIF_RANGE ;
    si.nMin = nMinPos ;
    si.nMax = nMaxPos - 1 ;

    if (fnBar == SB_HORZ)
    {
        m_horz_scroll_bar.SetScrollInfo(&si) ;
    }
    else
    {
        m_vert_scroll_bar.SetScrollInfo(&si) ;
    }

    return TRUE ;
}

//@cmember Set the scroll position
BOOL tomEdit::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw) 
{
    SCROLLINFO si ;
    si.cbSize = sizeof(si) ;
    si.fMask = SIF_POS ;
    si.nPos = nPos ;

    LONG min, max, pos, page ;
    m_services->TxGetVScroll(&min, &max, &pos, &page, NULL) ;

    LONG hmin, hmax, hpos, hpage ;
    m_services->TxGetHScroll(&hmin, &hmax, &hpos, &hpage, NULL) ;

    if (fnBar == SB_HORZ)
    {
         m_horz_scroll_bar.SetScrollInfo(&si) ;
    }
    else
    {
        m_vert_scroll_bar.SetScrollInfo(&si) ;
    }

    return TRUE ;
}

//@cmember InvalidateRect
void tomEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode) 
{
    UpdateView(prc) ;
}

//@cmember Send a WM_PAINT to the window
void tomEdit::TxViewChange(BOOL fUpdate) 
{
    UpdateView(NULL) ;
}

//@cmember Create the caret
BOOL tomEdit::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) 
{
    TRACE(_T("CreateCaret, %d, %d\n"), xWidth, yHeight) ;
    //return ::CreateCaret (GetCenter()->GetHost(), hbmp, xWidth, yHeight);
    return m_caret.CreateCaret(this, xWidth, yHeight) ;
}

//@cmember Show the caret
BOOL tomEdit::TxShowCaret(BOOL fShow) 
{
//     if(fShow)
//         return ::ShowCaret(GetCenter()->GetHost());
//     else
//         return ::HideCaret(GetCenter()->GetHost());
    if (fShow)
    {
        //BOOL res = ::ShowCaret(GetCenter()->GetHost()) ;

        CHARRANGE cr ;
        m_services->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL) ;
        if (cr.cpMin != cr.cpMax)
            return FALSE ;

        BOOL res = m_caret.ShowCaret() ;
        TRACE(_T("ShowCaret, %d\n"), res) ;
        return res ;
    }
    else
    {
        //BOOL res = ::HideCaret(GetCenter()->GetHost());
        BOOL res = m_caret.HideCaret() ;
        TRACE(_T("HideCaret, %d\n"), res) ;
        return res ;
    }
}

//@cmember Set the caret position
BOOL tomEdit::TxSetCaretPos(INT x, INT y) 
{
    TRACE(_T("SetCaretPos: %d, %d\n"), x, y) ;
    //return ::SetCaretPos(x, y);
    return m_caret.SetCaretPos(x, y) ;
}

//@cmember Create a timer with the specified timeout
BOOL tomEdit::TxSetTimer(UINT idTimer, UINT uTimeout) 
{
    return SetTimer(idTimer, uTimeout, NULL) ;
}

//@cmember Destroy a timer
void tomEdit::TxKillTimer(UINT idTimer) 
{
    KillTimer(idTimer) ;
}

//@cmember Scroll the content of the specified window's client area
void tomEdit::TxScrollWindowEx(
                               INT dx,
                               INT dy,
                               LPCRECT lprcScroll,
                               LPCRECT lprcClip,
                               HRGN hrgnUpdate,
                               LPRECT lprcUpdate,
                               UINT fuScroll) 
{
    // 未处理，似乎可以什么都不做
}

//@cmember Get mouse capture
void tomEdit::TxSetCapture(BOOL fCapture) 
{
    if (fCapture)
    {
        SetCapture() ;
    }
    else
    {
        ReleaseCapture() ;
    }
}

//@cmember Set the focus to the text window
void tomEdit::TxSetFocus() 
{
    SetFocus() ;
}

//@cmember Establish a new cursor shape
void tomEdit::TxSetCursor(HCURSOR hcur, BOOL fText) 
{
    ::SetCursor(hcur) ;
}

//@cmember Converts screen coordinates of a specified point to the client coordinates
BOOL tomEdit::TxScreenToClient (LPPOINT lppt) 
{
    // 这样写貌似有问题，因为仅仅是映射到宿主上，而不是无句柄窗口坐标上
    return ::ScreenToClient(GetCenter()->GetHost(), lppt) ;
}

//@cmember Converts the client coordinates of a specified point to screen coordinates
BOOL tomEdit::TxClientToScreen (LPPOINT lppt) 
{
    return ::ClientToScreen(GetCenter()->GetHost(), lppt) ;
}

//@cmember Request host to activate text services
HRESULT tomEdit::TxActivate( LONG * plOldState ) 
{
    return S_OK ;
}

//@cmember Request host to deactivate text services
HRESULT tomEdit::TxDeactivate( LONG lNewState ) 
{
    return S_OK ;
}

//@cmember Retrieves the coordinates of a window's client area
HRESULT tomEdit::TxGetClientRect(LPRECT prc) 
{
    // 这里直接return E_FAIL会导致无法用鼠标选择文本
    //return E_FAIL ;
    //GetRectInHost(prc) ;
    GetClientBound(*prc) ;
    return S_OK ;
}

//@cmember Get the view rectangle relative to the inset
HRESULT tomEdit::TxGetViewInset(LPRECT prc) 
{
    //return E_NOTIMPL ;
    // 这里的坐标单位是HIMETRIC(参msdn)
    ::SetRect(
        prc,
        DXtoHimetricX(m_view_inset.left, s_XPixelPerInch),
        DXtoHimetricX(m_view_inset.top, s_YPixelPerInch),
        DXtoHimetricX(m_view_inset.right, s_XPixelPerInch),
        DXtoHimetricX(m_view_inset.bottom, s_YPixelPerInch)
        ) ;
    return S_OK ;
}

//@cmember Get the default character format for the text
HRESULT tomEdit::TxGetCharFormat(const CHARFORMATW **ppCF ) 
{
    // 貌似只在初始化的时候被调用一次，以后再也不会被调用
    *ppCF = &m_default_char_format ;
    return S_OK ;
}

//@cmember Get the default paragraph format for the text
HRESULT tomEdit::TxGetParaFormat(const PARAFORMAT **ppPF) 
{
    *ppPF = &m_default_para_format ;
    return S_OK ;
}

//@cmember Get the background color for the window
COLORREF tomEdit::TxGetSysColor(int nIndex) 
{
    //TRACE(_T("GetSysColor: %d\n"), nIndex) ;
    if (COLOR_WINDOW == nIndex)
    {
        return RGB(255, 0, 0) ;
    }
    if (COLOR_WINDOWTEXT == nIndex)
    {
        return RGB(0, 255, 0) ;
    }
    return GetSysColor(nIndex) ;
}

//@cmember Get the background (either opaque or transparent)
HRESULT tomEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle) 
{
    *pstyle = m_background_transparent ? TXTBACK_TRANSPARENT : TXTBACK_OPAQUE ;
    return S_OK ;
}

//@cmember Get the maximum length for the text
HRESULT tomEdit::TxGetMaxLength(DWORD *plength) 
{
    // 随手抄的数字，未研究为什么
    *plength = (32 * 1024) - 1 ;
    return S_OK ;
}

//@cmember Get the bits representing requested scroll bars for the window
HRESULT tomEdit::TxGetScrollBars(DWORD *pdwScrollBar) 
{
    *pdwScrollBar =  GetStyle() & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);
    return S_OK ;
}

//@cmember Get the character to display for password input
HRESULT tomEdit::TxGetPasswordChar(TCHAR *pch) 
{
    *pch = _T('*') ;
    return S_OK ;
}

//@cmember Get the accelerator character
HRESULT tomEdit::TxGetAcceleratorPos(LONG *pcp) 
{
    *pcp = -1 ;
    return S_OK ;
}

//@cmember Get the native size
HRESULT tomEdit::TxGetExtent(LPSIZEL lpExtent) 
{
    //TRACE(_T("tomEdit::TxGetExtent, %u\n"), GetTickCount()) ;
    return E_NOTIMPL ;
}

//@cmember Notify host that default character format has changed
HRESULT tomEdit::OnTxCharFormatChange (const CHARFORMATW * pcf) 
{
    // 这里似乎没被执行到，即使调用EM_SETCHARFORMAT
    //memcpy(&m_default_para_format, pcf, pcf->cbSize) ;
    //m_default_char_format = *pcf ;
    return S_OK ;
}

//@cmember Notify host that default paragraph format has changed
HRESULT tomEdit::OnTxParaFormatChange (const PARAFORMAT * ppf) 
{
    //m_default_para_format = *ppf ;
    return S_OK ;
}

//@cmember Bulk access to bit properties
HRESULT tomEdit::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
    DWORD dwProperties = 0 ;

    //if (fRich)
    {
        dwProperties = TXTBIT_RICHTEXT ;
    }

    if (GetStyle() & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE ;
    }

    if (GetStyle() & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY ;
    }


    if (GetStyle() & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD ;
    }

    if (!(GetStyle() & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION ;
    }

    //if (fEnableAutoWordSel)
    {
    //    dwProperties |= TXTBIT_AUTOWORDSEL ;
    }

    // TXTBIT_VERTICAL目前不被TOM支持，参msdn
    //if (fVertical)
    //{
    //    dwProperties |= TXTBIT_VERTICAL ;
    //}

    // 当包含TXTBIT_WORDWRAP属性时，水平滚动功能失效
    if (m_word_wrap)
    {
        dwProperties |= TXTBIT_WORDWRAP ;
    }

    //if (fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP ;
    }

    //if (fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION ;
    }

    *pdwBits = dwProperties & dwMask ; 

    return S_OK ;
}

//@cmember Notify host of events
HRESULT tomEdit::TxNotify(DWORD iNotify, void *pv) 
{
    if (iNotify == EN_SELCHANGE)
    {
        SELCHANGE * sc = (SELCHANGE *)pv ;
        if (sc->seltyp != SEL_EMPTY)
        {
            m_caret.HideCaret() ;
        }
    }

    return S_OK ;
}

// Far East Methods for getting the Input Context
//#ifdef WIN95_IME
HIMC tomEdit::TxImmGetContext() 
{
    // 目前没发现作用
    return NULL ;
}

void tomEdit::TxImmReleaseContext( HIMC himc ) 
{
    // 啥都不做，不知道有没有问题
}
//#endif

//@cmember Returns HIMETRIC size of the control bar.
HRESULT tomEdit::TxGetSelectionBarWidth (LONG *lSelBarWidth) 
{
    // 目前没发现作用
    *lSelBarWidth = 0 ;
    return S_OK ;
}

// ITextHost2 Interface
//@cmember Is a double click in the message queue?
BOOL tomEdit::TxIsDoubleClickPending() 
{
    MSG msg;

    HWND host = GetCenter()->GetHost() ;
    if( PeekMessageA(&msg, host, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK,
        PM_NOREMOVE | PM_NOYIELD) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//@cmember Get the overall window for this control	 
HRESULT tomEdit::TxGetWindow(HWND *phwnd) 
{
    *phwnd = GetCenter()->GetHost() ;
    return S_OK ;
}

//@cmember Set control window to foreground
HRESULT tomEdit::TxSetForegroundWindow() 
{
    HWND host = GetCenter()->GetHost() ;
    if (!SetForegroundWindow(host))
    {
        ::SetFocus(host);
    }

    return S_OK ;
}

//@cmember Set control window to foreground
HPALETTE tomEdit::TxGetPalette() 
{
    return NULL ;
}



template<>
UILIB_EXPORT WLTomEditImpBase *CreateDefaultImp< WLTomEditImpBase >(IWLTomEdit * wrapper)

{
	tomEdit *ret = new tomEdit();
	ret->SetWrapper(wrapper);
	return ret;
};
