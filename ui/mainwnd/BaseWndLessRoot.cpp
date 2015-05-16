
#include "StdAfx.h"

#include "Resource.h"
#include "uilib\common\utils\ImageEx\ImageEx.h"
#include "uilib\common\utils\Graphics.h"
#include "uilib\common\MemoryLeakChecker.h"
#include "uilib\skin\ISkin.h"

#include "BaseWndLessRoot.h"
#include <Tlhelp32.h>
#include "utils/strutils.h"

BOOL BaseWndLessRoot::s_vista_upper ;

BaseWndLessRoot::BaseWndLessRoot(void)
	: m_wnd_style(WND_STYLE_CAPTION)		// 默认情况下有标题
{
	OSVERSIONINFOEX osv ;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX) ;
	::GetVersionEx((LPOSVERSIONINFO)&osv) ;
	if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		s_vista_upper = (osv.dwMajorVersion > 5) ;
	}

	m_title_height = 28 ;
	m_border_width = 5 ;
	m_nRndDia = 5 ;
	m_bDrawTitle = TRUE;
//  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME) ;

    m_max_size_valid        = FALSE ;
    m_max_position_valid    = FALSE ;
    m_min_track_size_valid  = FALSE ;
    m_max_track_size_valid  = FALSE ;
}

BaseWndLessRoot::~BaseWndLessRoot(void)
{
}

void BaseWndLessRoot::SetMaxSize(int cx, int cy) 
{
    m_max_size_valid = TRUE ;
    m_min_max_info.ptMaxSize.x = cx ;
    m_min_max_info.ptMaxSize.y = cy ;
}

void BaseWndLessRoot::SetMaxPosition(int x, int y) 
{
    m_max_position_valid = TRUE ;
    m_min_max_info.ptMaxPosition.x = x ;
    m_min_max_info.ptMaxPosition.y = y ;
}

void BaseWndLessRoot::SetMinTrackSize(int cx, int cy) 
{
    m_min_track_size_valid = TRUE ;
    m_min_max_info.ptMinTrackSize.x = cx ;
    m_min_max_info.ptMinTrackSize.y = cy ;
}

void BaseWndLessRoot::SetMaxTrackSize(int cx, int cy) 
{
    m_max_track_size_valid = TRUE ;
    m_min_max_info.ptMaxTrackSize.x = cx ;
    m_min_max_info.ptMaxTrackSize.y = cy ;
}

BOOL BaseWndLessRoot::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	switch (message)
	{
	case WM_SETTEXT :
		{
			SetWindowText((LPCTSTR)lParam);
		}
		break ;

	case WM_GETFONT :
		{
			*pResult = (LRESULT)m_hFont ;
			consumed = TRUE ;
		}
		break ;

	case WM_NCHITTEST :
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
			ScreenToClient(&point) ;
			*pResult = OnNcHitTest(point) ;
			consumed = TRUE ;
		}
		break ;

    case WM_GETMINMAXINFO :
        {
            OnGetMinMaxInfo((MINMAXINFO*)lParam) ;
            consumed = TRUE ;
        }
        break ;

	case WM_COMMAND :
		{
			consumed = OnCommand(wParam, lParam, pResult) ;
		}
		break ;

	case WM_NOTIFY :
		{
			consumed = OnNotify(wParam, lParam, pResult) ;
		}
		break ;

	case WM_NCCALCSIZE :
		{
			consumed = TRUE ;
		}
		break ;


	case WM_CLOSE :
		{
			OnClose() ;
			consumed = TRUE ;
		}
		break ;

	case WM_NCACTIVATE :
		{
			if (s_vista_upper)
			{
				*pResult = 1 ;
				consumed = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :

	case WM_NCLBUTTONDOWN :
	case WM_NCLBUTTONDBLCLK :
		{
			HWL hFocusChild = GetFocus() ;
			if (hFocusChild != NULL)
			{
				// 报告焦点窗口host被点击
				LRESULT lResult = 0 ;
				PollMessage(hFocusChild, WM_LDOWNHOST, 0, 0, &lResult) ;
			}
		}
		break ;


	}
	return consumed ;
}

void BaseWndLessRoot::OnSized(SIZE const &old_size, SIZE const &new_size) 
{
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, new_size.cx + 1, new_size.cy + 1, m_nRndDia, m_nRndDia) ;
	::SetWindowRgn(GetHost(), hRgn, TRUE) ;

	if (new_size.cx != old_size.cx)
	{
		RelayoutCaptionButton(new_size.cx) ;
	}

	if (0 != new_size.cx && 0 != new_size.cy )
	{
		PrepareBkgnd() ;
	}
}

void BaseWndLessRoot::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	::BitBlt(
		hDC,
		rcUpdate.left, rcUpdate.top, rcUpdate.right - rcUpdate.left, rcUpdate.bottom - rcUpdate.top,
		m_bkgnd_dc,
		rcUpdate.left, rcUpdate.top,
		SRCCOPY
		) ;
}

BOOL BaseWndLessRoot::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (dwStyle & WS_THICKFRAME)
		m_wnd_style |= (WND_STYLE_BORDER | WND_STYLE_RESIZABLE) ;
	if (dwStyle & WS_MINIMIZEBOX)
		m_wnd_style |= WND_STYLE_MINIMIZEBOX ;
	if (dwStyle & WS_MAXIMIZEBOX)
		m_wnd_style |= WND_STYLE_MAXIMIZEBOX ;
	if (dwStyle & WS_SYSMENU)
		m_wnd_style |= WND_STYLE_SYSTEMMENU ;

	m_hFont = CreateFont(
		-12,						// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH,				// nPitchAndFamily
		_T("MS Shell Dlg 2")
		) ;

	EnableTooltips(TRUE) ;

	GainWindowTitle() ;
	InitImage() ;
	CalcBkgndGraySale() ;
	PrepareBorder() ;
	InitCaptionButton() ;
	InitCtrls();
	return __super::OnCreate(dwStyle, x, y, cx, cy, pParent) ;
}

void BaseWndLessRoot::OnDestroy() 
{
	if (m_hFont != NULL)
		DeleteObject(m_hFont) ;
}

void BaseWndLessRoot::OnMaximized(WNDSIZE enPrev) 
{
    if (m_wnd_style & WND_STYLE_MAXIMIZEBOX)
    {
        m_max_btn.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
        m_restore_btn.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
    }
}

void BaseWndLessRoot::OnRestored(WNDSIZE enPrev) 
{
    if (m_wnd_style & WND_STYLE_MAXIMIZEBOX)
    {
        m_max_btn.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
        m_restore_btn.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
    }
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

LRESULT BaseWndLessRoot::OnNcHitTest(POINT const &point) 
{
	// 优先测试最大化、最小化三个按钮
	CRect rcBtn ;
	if ( ((m_wnd_style & WND_STYLE_MINIMIZEBOX) && m_min_btn.PtInWndLess(point))
		|| ((m_wnd_style & WND_STYLE_MAXIMIZEBOX) && m_max_btn.PtInWndLess(point))
		|| ((m_wnd_style & WND_STYLE_SYSTEMMENU) && m_close_btn.PtInWndLess(point))
		)
	{
		return HTCLIENT ;
	}

	// 不可变大小的窗口或者窗口最大化后，不支持改变大小
	if (IsZoomed(GetHost()) || !(m_wnd_style & WND_STYLE_RESIZABLE))
	{
		if (point.y <= m_title_height && (m_wnd_style & WND_STYLE_CAPTION))
			return HTCAPTION ;
		return HTCLIENT ;
	}

	// 区域测试
	int pos = 0 ;
	UINT nHT[9] = {
		HTTOPLEFT, HTTOP, HTTOPRIGHT,
		HTLEFT, HTCLIENT, HTRIGHT,
		HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT
	} ;

	SIZE window_size ;
	GetSize(&window_size) ;

	if (point.x >= m_border_width)
	{
		++ pos ;
		if (point.x >= window_size.cx - m_border_width)
			++ pos ;
	}
	if (point.y >= m_border_width)
	{
		pos += 3 ;
		if (point.y >= window_size.cy - m_border_width)
			pos += 3 ;
	}

	if (pos == 4)
	{
		if (point.y <= m_title_height && (m_wnd_style & WND_STYLE_CAPTION))
			return HTCAPTION ;
	}

	return nHT[pos] ;
}

void BaseWndLessRoot::OnGetMinMaxInfo(MINMAXINFO* lpMMI) 
{
    CRect rcWorkArea ;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) ;

    if (m_max_size_valid)
    {
        lpMMI->ptMaxSize.x = m_min_max_info.ptMaxSize.x ;
        lpMMI->ptMaxSize.y = m_min_max_info.ptMaxSize.y ;
    }
    else
    {
        lpMMI->ptMaxSize.x = rcWorkArea.Width() + 4 ;
        lpMMI->ptMaxSize.y = rcWorkArea.Height() + 4 ;
    }

    if (m_max_position_valid)
    {
        lpMMI->ptMaxPosition.x = m_min_max_info.ptMaxPosition.x ;
        lpMMI->ptMaxPosition.y = m_min_max_info.ptMaxPosition.y ;
    }
    else
    {
        lpMMI->ptMaxPosition.x = rcWorkArea.left - 2 ;
        lpMMI->ptMaxPosition.y = rcWorkArea.top - 2 ;
    }

    if (m_min_track_size_valid)
    {
        lpMMI->ptMinTrackSize.x = m_min_max_info.ptMinTrackSize.x ;
        lpMMI->ptMinTrackSize.y = m_min_max_info.ptMinTrackSize.y ;
    }
    if (m_max_track_size_valid)
    {
        lpMMI->ptMaxTrackSize.x = m_min_max_info.ptMaxTrackSize.x ;
        lpMMI->ptMaxTrackSize.y = m_min_max_info.ptMaxTrackSize.y ;
    }

    //lpMMI->ptMaxTrackSize.x = rcWorkArea.Width() + 4 ;
    //lpMMI->ptMaxTrackSize.y = rcWorkArea.Width() + 4 ;
}

BOOL BaseWndLessRoot::OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	WORD id = LOWORD(wParam) ;
	WORD code = HIWORD(wParam) ;

	// menu and accelerator command
	if (lParam == 0)
	{
		if (code == 0)
		{
			return OnMenuCommand(wParam, lParam, pResult) ;
		}
		else if (code == 1)
		{
			return OnAcceleratorCommand(wParam, lParam, pResult) ;
		}
		else
		{
			ASSERT(FALSE) ;
			return FALSE ;
		}
	}
	else	// control command
	{
		return OnControlCommand(id, code, (HWL)lParam, pResult) ;
	}
}

BOOL BaseWndLessRoot::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	return consumed ;
}

BOOL BaseWndLessRoot::OnMenuCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	return consumed ;
}

BOOL BaseWndLessRoot::OnAcceleratorCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	return consumed ;
}

BOOL BaseWndLessRoot::OnControlCommand(WORD id, WORD code, HWL wnd, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	if (code == BN_CLICKED)
	{
		if (id == ID_BTN_MIN)
		{
			::SendMessage(GetHost(), WM_SYSCOMMAND, SC_MINIMIZE, 0) ;
			consumed = TRUE ;
		}
		else if (id == ID_BTN_MAX)
		{
			::SendMessage(GetHost(), WM_SYSCOMMAND, SC_MAXIMIZE, 0) ;
			consumed = TRUE ;
		}
		else if (id == ID_BTN_RESTORE)
		{
			::SendMessage(GetHost(), WM_SYSCOMMAND, SC_RESTORE, 0) ;
			consumed = TRUE ;
		}
		else if (id == ID_BTN_CLOSE)
		{
			::SendMessage(GetHost(), WM_SYSCOMMAND, SC_CLOSE, 0) ;
			consumed = TRUE ;
		}
	}

	return consumed ;
}

void BaseWndLessRoot::OnClose() 
{
	CWnd * host = CWnd::FromHandlePermanent(GetHost()) ;
	host->EndModalLoop(0) ;

	::DestroyWindow(GetHost()) ;
}

//////////////////////////////////////////////////////////////////////////
// Inner Function

void BaseWndLessRoot::GainWindowTitle() 
{
	HWND host = GetHost() ;

	int title_length = ::GetWindowTextLength(host) ;
	if (title_length == 0)
		return ;

	TCHAR * title = new TCHAR[title_length + 1] ;
	::GetWindowText(host, title, title_length + 1) ;
	SetWindowText(title) ;
	delete [] title ;
}

void BaseWndLessRoot::InitImage() 
{
	m_img_min_btn = Shared_ImageEx::FromId(IDI_BTN_MIN_PNG) ;
	m_img_max_btn = Shared_ImageEx::FromId(IDI_BTN_MAX_PNG) ;
	m_img_restore_btn = Shared_ImageEx::FromId(IDI_BTN_RESTORE_PNG) ;
	m_img_close_btn = Shared_ImageEx::FromId(IDI_BTN_CLOSE_PNG) ;

	WLDC dc(this) ;
	Shared_ImageEx img_wnd_top = Shared_ImageEx::FromId(IDI_BASE_WND_BKGND_PNG) ;
	m_wnd_top_dc.CreateFromImage(dc, img_wnd_top, FALSE) ;
}

void BaseWndLessRoot::CalcBkgndGraySale() 
{
	RECT bound = {0, 0, 300, 110} ;
	m_bkgnd_gray_scale = hdutils::GetRoughGrayScale(m_wnd_top_dc, &bound) ;
}

void BaseWndLessRoot::PrepareBorder() 
{
	WLDC dc(this) ;
	Shared_ImageEx imgBorder = Shared_ImageEx::FromId(IDI_WND_FRAME_PNG) ;
	if (imgBorder != NULL)
	{
		m_border_dc.Create32BitDC(dc.GetSafeHdc(), 90, 90) ;
		CCanvasDC cvOriginBorder ;
		cvOriginBorder.CreateFromImage(dc.GetSafeHdc(), imgBorder, TRUE) ;
		hdutils::AlphaSurfaceTile(
			m_border_dc, 0, 0, m_border_dc.GetWidth(), m_border_dc.GetHeight(),
			cvOriginBorder, 0, 0, cvOriginBorder.GetWidth(), cvOriginBorder.GetHeight(), TRUE, 3, 3, 3, 3
			) ;
	}
}

// 初始化标题栏按钮
void BaseWndLessRoot::InitCaptionButton() 
{
	if (m_wnd_style & WND_STYLE_MINIMIZEBOX)
	{
		if (!m_min_btn.Create(WS_VISIBLE, 0, 0, 0, 0, this, ID_BTN_MIN)) 
			return ;
		m_min_btn.SetStyle(WLButtonAL::FITFRGNDSIZE, TRUE) ;
		m_min_btn.SetFrgnd(m_img_min_btn, 3, WLButtonAL::FIXED) ;
		m_min_btn.AddTooltip(_T("最小化")) ;
	}

	if (m_wnd_style & WND_STYLE_MAXIMIZEBOX)
	{
		if (!m_max_btn.Create(WS_VISIBLE, 0, 0, 0, 0, this, ID_BTN_MAX)) 
			return ;
		m_max_btn.SetStyle(WLButtonAL::FITFRGNDSIZE, TRUE) ;
		m_max_btn.SetFrgnd(m_img_max_btn, 3, WLButtonAL::FIXED) ;
		m_max_btn.AddTooltip(_T("最大化")) ;

		if (!m_restore_btn.Create(WS_VISIBLE, 0, 0, 0, 0, this, ID_BTN_RESTORE)) 
			return ;
		m_restore_btn.SetStyle(WLButtonAL::FITFRGNDSIZE, TRUE) ;
		m_restore_btn.SetFrgnd(m_img_restore_btn, 3, WLButtonAL::FIXED) ;
		m_restore_btn.AddTooltip(_T("还原")) ;

		if ( ::IsZoomed( GetHost() ))
		{
			m_max_btn.SetWndLessPos( 0,0,0,0, SWLP_HIDE );
		}
		else
		{
			m_restore_btn.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
		}
	}

	if (m_wnd_style & WND_STYLE_SYSTEMMENU)
	{
		if (!m_close_btn.Create(WS_VISIBLE, 0, 0, 0, 0, this, ID_BTN_CLOSE)) 
			return ;
		m_close_btn.SetStyle(WLButtonAL::FITFRGNDSIZE, TRUE) ;
		m_close_btn.SetFrgnd(m_img_close_btn, 3, WLButtonAL::FIXED) ;
		m_close_btn.AddTooltip(_T("关闭")) ;
	}
}

void BaseWndLessRoot::InitCtrls()
{
}

void BaseWndLessRoot::PrepareBkgnd() 
{
	WLDC dc(this) ;
	SIZE window_size ;
	GetSize(&window_size) ;
	if (!m_bkgnd_dc.IsAvailable())
	{
		m_bkgnd_dc.CreateCompatibleDC(dc, window_size.cx, window_size.cy) ;
		m_bkgnd_dc.SelectFont(m_hFont) ;
	}
	else
	{
		m_bkgnd_dc.ChangeSize(window_size.cx, window_size.cy) ;
	}

	DrawBkgnd() ;
}

void BaseWndLessRoot::DrawBkgnd()
{
	SIZE window_size ;
	GetSize(&window_size) ;
	ISkin const & skin = GetSkinMgr()->GetSkin();

	if (skin.GetMode() == ISkin::Default)
	{
		int wnd_top_height = m_wnd_top_dc.GetHeight() ;

		hdutils::LineTile(
			m_bkgnd_dc,
			0, 0, m_bkgnd_dc.GetWidth(), m_wnd_top_dc.GetHeight(),
			m_wnd_top_dc,
			0, 0, m_wnd_top_dc.GetWidth(), m_wnd_top_dc.GetWidth(),
			0, 0
			) ;

		if (window_size.cy > wnd_top_height)
		{
			Graphics gph(m_bkgnd_dc) ;
			SolidBrush sb(Color(255, 0xf3, 0xfb, 0xff)) ;
			gph.FillRectangle(&sb, 0, wnd_top_height, window_size.cx, window_size.cy - wnd_top_height) ;
		}

	}
	else
	{
		// 绘制皮肤
		skin.Draw(m_bkgnd_dc, 0, 0, window_size.cx, window_size.cy) ;

	}

	// 绘制边框
	hdutils::AlphaSurfaceTile(
		m_bkgnd_dc,
		0, 0, window_size.cx, window_size.cy,
		m_border_dc,
		0, 0, m_border_dc.GetWidth(), m_border_dc.GetHeight(),
		TRUE
		) ;

	

	// 绘制标题
	if ( m_bDrawTitle )
	{
		DrawTitle(m_bkgnd_dc, m_bkgnd_gray_scale) ;
	}
}

void BaseWndLessRoot::DrawTitle(HDC hdc, BYTE bkgnd_gray_scale) 
{
	SIZE window_size ;
	GetSize(&window_size) ;

	// 绘制图标
	::DrawIconEx(hdc, 8, (m_title_height - 16) / 2, m_hIcon, 16, 16, 0, 0, DI_NORMAL) ;

	CString sTitle ;
	GetWindowText(sTitle) ;
	CRect rcTitle(30, 6, window_size.cx - 76, 30) ;

	COLORREF crText, crHalo, crBkgnd ;
	if (bkgnd_gray_scale < 128)
	{
		crText = RGB(255, 255, 255) ;
		crHalo = RGB(80, 80, 80) ;
	}
	else
	{
		crText = RGB(0, 0, 0) ;
		crHalo = RGB(233, 233, 233) ;
	}
	crBkgnd = RGB(bkgnd_gray_scale, bkgnd_gray_scale, bkgnd_gray_scale) ;
	COLORREF crOldText = ::SetTextColor(hdc, crText) ;
	hdutils::DrawGlowText(hdc, sTitle, sTitle.GetLength(), rcTitle, DT_SINGLELINE | DT_END_ELLIPSIS, crHalo, crBkgnd) ;
	::SetTextColor(hdc, crOldText) ;
}

void BaseWndLessRoot::RelayoutCaptionButton(int width) 
{
	RECT temp ;
	int x = 4 ;
	if (m_wnd_style & WND_STYLE_SYSTEMMENU)
	{
		m_close_btn.GetRectInParent(&temp) ;
		x += (temp.right - temp.left) ;
		m_close_btn.SetWndLessPos(width - x, 1, 0, 0, SWLP_X | SWLP_Y) ;
	}

	if (m_wnd_style & WND_STYLE_MAXIMIZEBOX)
	{
		m_max_btn.GetRectInParent(&temp) ;
		x += (temp.right - temp.left) ;
		m_max_btn.SetWndLessPos(width - x, 1, 0, 0, SWLP_X | SWLP_Y) ;
		m_restore_btn.SetWndLessPos(width - x, 1, 0, 0, SWLP_X | SWLP_Y) ;
	}

	if (m_wnd_style & WND_STYLE_MINIMIZEBOX)
	{
		m_min_btn.GetRectInParent(&temp) ;
		x += (temp.right - temp.left) ;
		m_min_btn.SetWndLessPos(width - x, 1, 0, 0, SWLP_X | SWLP_Y) ;
	}
}

BOOL BaseWndLessRoot::PollMessage(HWL hChild, UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	HWL hCur = hChild ;
	while (hCur != NULL)
	{
		bRes = hCur->OnWndMsg(message, wParam, lParam, pResult) ;
		if (bRes)
			break ;
		hCur = hCur->GetParent() ;
	}

	return bRes ;
}