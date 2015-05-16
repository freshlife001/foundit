#include "StdAfx.h"
#include "WLEditReadOnly.h"
#include "common/utils/Graphics.h"
#include "common/wnds/Menu/hdmenu.h"
#include <boost/shared_array.hpp>
#include "common/utils/UIUtils.h"

WLEditReadOnlyImp::WLEditReadOnlyImp(void)
{
	m_bFocus = FALSE ;
	m_nCaretPos = 0 ;
	m_bShowCaret = FALSE ;

	m_nLeftMargin = 3 ;
	m_nTopMargin = 3 ;
	m_nRightMargin = 3 ;
	m_nBottomMargin = 3 ;
	m_nFixSel = 0 ;
	m_nLeftSel = 0 ;
	m_nRightSel = 0 ;

	m_bLDown = FALSE ;
	m_bRDown = FALSE ;
	m_bBeginSel = FALSE ;

	m_bPopMenu = FALSE ;

	m_nMaxChars = 0 ;		// 0表示不限制

	m_crTextNormalColor = hdutils::crTextColor ;
	m_crTextHighlightColor = hdutils::crTextColor ;
	m_crTextHighlightBkColor = Color::MakeARGB(255, 174, 196, 232) ;
	m_crBkgndColor = Color::MakeARGB(0, 0, 0, 0) ;

	m_nRowInterval = 10;
}

WLEditReadOnlyImp::~WLEditReadOnlyImp(void)
{
}

BOOL WLEditReadOnlyImp::Create(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent, UINT uID) 
{
	BOOL bRes = __super::Create(dwStyle, x, y, cx, cy, pParent, uID) ;

	if (bRes)
	{
		m_rcTextBound.SetRect(
			m_nLeftMargin, m_nTopMargin,
			cx - m_nRightMargin,
			cy - m_nBottomMargin
			) ;
	}

	return bRes ;
}

void WLEditReadOnlyImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;

	// 绘制背景
	OnDrawBkgnd(hDC, rcInHost, rcUpdate) ;

	HRGN hOldClipRgn = CreateRectRgn(0, 0, 0, 0) ;
	BOOL bNoClipNow = (0 == ::GetClipRgn(hDC, hOldClipRgn)) ;

	// 限制文字绘制区域
	CRect rcClip(m_rcTextBound) ;
	rcClip.OffsetRect(rcInHost.TopLeft()) ;
	HRGN hClipRgn = ::CreateRectRgnIndirect(&rcClip) ;
	::ExtSelectClipRgn(hDC, hClipRgn, RGN_AND) ;
	::DeleteObject(hClipRgn) ;
	//::SelectClipRgn(hDC, hClipRgn) ;

	int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;

	CRect rcText( m_rcTextBound ) ;
	int i = 0 ;


	// 设置文字颜色
	COLORREF crOldText = ::SetTextColor(hDC, m_crTextNormalColor) ;

	CString &text = GetWindowTextRef();
	int nTextLen = text.GetLength() ;
	LPCTSTR lpszText = text ;
	for (i = 0 ; i < nTextLen ; ++ i)
	{
		rcText = m_vecCharacterRect[i];

		// 转换成dc坐标
		CRect rcDraw(rcText) ;
		rcDraw.OffsetRect( rcInHost.TopLeft() ) ;

		TCHAR s[2] ;
		s[0] = *lpszText ;
		s[1] = 0 ;

		if ( isSelected( i ) && m_bFocus )
		{
			// 绘制高亮文字背景
			//HBRUSH hBrush = ::GetSysColorBrush(COLOR_HIGHLIGHT) ;
			//::FillRect(hDC, rcDraw, hBrush) ;
			DrawHighlightTextBkgnd( hDC, rcDraw ) ;

			// 绘制高亮文字
			COLORREF crOld = ::SetTextColor(hDC, m_crTextHighlightColor) ;
			::DrawText( hDC, s, 1, rcDraw, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX) ;
			::SetTextColor( hDC, crOld ) ;
		}
		else
		{
			::DrawText( hDC, s, 1, rcDraw, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX) ;
		}
		lpszText += 1 ;
	}

	// 恢复文字颜色
	::SetTextColor(hDC, crOldText) ;

	// 恢复DC
	::SetBkMode(hDC, nOldBkMode) ;

	// 取消限制范围
	if (bNoClipNow)
	{
		::SelectClipRgn(hDC, NULL) ;
	}
	else
	{
		::SelectClipRgn(hDC, hOldClipRgn) ;
	}
	::DeleteObject(hOldClipRgn) ;
}

void WLEditReadOnlyImp::OnDrawBkgnd(HDC hDC, RECT const &rcInHost, RECT const &rcUpdate) 
{
	// 绘制背景
	CRect rcBkgnd(rcInHost) ;
	BYTE byA = m_crBkgndColor.GetA() ;
	if (byA != 0)
	{
		if (byA == 255)
		{
			HBRUSH hBrush = ::CreateSolidBrush(m_crBkgndColor.ToCOLORREF()) ;
			::FillRect(hDC, &rcBkgnd, hBrush) ;
			::DeleteObject(hBrush) ;
		}
		else		// 迫不得已，再用Gdiplus
		{
			Graphics g(hDC) ;
			SolidBrush sb(m_crBkgndColor) ;
			g.FillRectangle(&sb, rcBkgnd.left, rcBkgnd.top, rcBkgnd.Width(), rcBkgnd.Height()) ;
		}
	}
}

void WLEditReadOnlyImp::DrawHighlightTextBkgnd(HDC hDC, RECT const &rcBkgnd) 
{
	BYTE byA = m_crTextHighlightBkColor.GetA() ;
	if (byA != 0)
	{
		if (byA == 255)
		{
			HBRUSH hBrush = ::CreateSolidBrush(m_crTextHighlightBkColor.ToCOLORREF()) ;
			::FillRect(hDC, &rcBkgnd, hBrush) ;
			::DeleteObject(hBrush) ;
		}
		else		// 迫不得已，再用Gdiplus
		{
			Graphics g(hDC) ;
			SolidBrush sb(m_crTextHighlightBkColor) ;
			g.FillRectangle(&sb, rcBkgnd.left, rcBkgnd.top, rcBkgnd.right - rcBkgnd.left, rcBkgnd.bottom - rcBkgnd.top) ;
		}
	}
}

void WLEditReadOnlyImp::SendChangeCommand() 
{
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), EN_CHANGE), (LPARAM)m_wrapper) ;
}

UINT WLEditReadOnlyImp::OnGetDlgCode()
{
	//TRACE(_T("[%d]WLEditReadOnly::OnGetDlgCode\n"), GetTickCount()) ;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT uRes = 0;
	uRes |= DLGC_WANTARROWS ;
	uRes |= DLGC_HASSETSEL ;
	uRes |= DLGC_WANTCHARS ;
	return uRes ;
}

BOOL WLEditReadOnlyImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnLButtonDown(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnLButtonUp(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONDBLCLK :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnLButtonDblClk(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_RBUTTONDOWN :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnRButtonDown(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_RBUTTONUP :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnRButtonUp(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_CHAR :
		{
			OnChar(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_TIMER :
		{
			// OnTimer(wParam) ;
			bRes = TRUE ;
		}
		break ;

	case WM_SETFOCUS :
		{
			// 让得到焦点消息传上去
			OnSetFocus(reinterpret_cast<HWL>(wParam)) ;
			//bRes = TRUE ;
		}
		break ;

	case WM_KILLFOCUS :
		{
			// 让失去焦点消息传上去
			OnKillFocus(reinterpret_cast<HWL>(wParam)) ;
		}
		break ;

	case WM_KEYDOWN :
		{
			if (wParam == VK_TAB)
			{
				bRes = FALSE ;
			}
			else
			{
				bRes = OnKeyDown(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam)) ;
			}
		}
		break ;

	case WM_SETCURSOR :
		{
			bRes = OnSetCursor(lParam) ;
		}
		break ;

	case WM_MOUSEMOVE :
		{
			CPoint point(lParam);
			HostToClient(&point) ;
			OnMouseMove(static_cast<UINT>(wParam), point);
			bRes = TRUE ;
		}
		break ;

	case WM_GETDLGCODE :
		{
			*pResult = OnGetDlgCode() ;
			bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			OnCaptureChanged((HWL)lParam) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLEditReadOnlyImp::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	m_rcTextBound.right = szNew.cx - m_nRightMargin;
	m_rcTextBound.bottom = szNew.cy - m_nBottomMargin;

	CalculateCharaterRect();

	UpdateView() ;
}

BOOL WLEditReadOnlyImp::SetWindowText(LPCTSTR lpszText) 
{
	if (lpszText == NULL)
	{
		ASSERT(FALSE) ;
		return FALSE ;
	}
	// 清除非显示字符
	int nLength = _tcslen( lpszText ) ;
	boost::shared_array<TCHAR> pszNewText;
	pszNewText.reset( new TCHAR[nLength + 1]  );
	int i = 0 ;
	TCHAR c ;
	while (( c = *lpszText ) != 0)
	{
		//if ( iswprint( c ))
		if (hdutils::IsEditPrint(c))
		{
			pszNewText[i] = c ;
			++ i ;
		}
		++ lpszText ;
	}
	pszNewText[i] = 0 ;

	// 如果插入后的字符数会超过最大限制数，则退出
	//if ( nLength > 280 )
	//{
	//	return FALSE;
	//}
	WLDC dc(this) ;

	// 插入文本并计算位置
	CString &text = GetWindowTextRef();
	text = pszNewText.get();
	int nInsertTextLen = _tcslen( pszNewText.get() ) ;

	m_vecCharacterRect.clear();
	m_vecCharacterRect.resize( nInsertTextLen );
	// 获取每个字符的大小
	for (i = 0 ; i < nInsertTextLen ; ++ i)
	{
		TCHAR s[2] ;
		s[0] = pszNewText[i] ;
		s[1] = 0 ;
		CSize size = dc.GetTextExtent(s, 1) ;
		m_vecCharacterRect[i].SetRect( 0,0, size.cx, size.cy );

	}

	if ( nInsertTextLen <=0 )
	{
		return TRUE;
	}
	// 计算每个字符所在Rect
	CalculateCharaterRect();
	UpdateView() ;

	return TRUE ;
}

void WLEditReadOnlyImp::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 未获得焦点时左键点击只设置焦点
	if (!m_bFocus)
	{
		SetFocus() ;
		return ;
	}

	m_nCaretPos = HitTestCaretPos(point) ;
	m_bLDown = TRUE ;
	m_bBeginSel = TRUE ;

	// 判断是否按下shift键
	if (nFlags & MK_SHIFT)
	{
		if (m_nFixSel < m_nCaretPos)
		{
			m_nLeftSel = m_nFixSel ;
			m_nRightSel = m_nCaretPos ;
		}
		else
		{
			m_nLeftSel = m_nCaretPos ;
			m_nRightSel = m_nFixSel ;
		}
	}
	else
	{
		m_nLeftSel = m_nRightSel = m_nCaretPos ;
		m_nFixSel = m_nCaretPos ;
	}

	if (GetCapture() != m_wrapper)
		SetCapture() ;

	UpdateView() ;
}

void WLEditReadOnlyImp::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bRDown = TRUE ;
	if (m_bLDown)
		m_bLDown = FALSE ;

	int nCaretPos = HitTestCaretPos(point) ;
	if (nCaretPos < m_nLeftSel || nCaretPos > m_nRightSel)
	{
		m_nFixSel = m_nLeftSel = m_nRightSel = m_nCaretPos = nCaretPos ;
	}

	if (GetCapture() != m_wrapper)
		SetCapture() ;

	SetFocus() ;
	UpdateView() ;
}

void WLEditReadOnlyImp::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bRDown)
	{
		m_bRDown = FALSE ;

		if (GetCapture() == m_wrapper)
			ReleaseCapture() ;

		if (!m_rcTextBound.PtInRect(point))
			return ;

// 		if (!PtInWndLess(point))
// 			return ;

		// 弹出右键菜单
// 		CMenu menu ;
// 		menu.CreatePopupMenu() ;
// 		menu.AppendMenu(MF_BYCOMMAND | MF_STRING, 15, _T("复制")) ;

		BOOL bSelected = m_nRightSel > m_nLeftSel ;
		BOOL bCanRead = !(GetStyle() & ES_PASSWORD) ;
		BOOL bHasData = IsClipboardFormatAvailable(CF_UNICODETEXT) ;
		BOOL bEmpty = IsEmpty() ;

		HHDMENU hMenu = hdui::CreatePopupMenu() ;

		UINT uFlags = MF_BYCOMMAND | MF_STRING ;



		hdui::AppendMenu(hMenu, (bSelected && bCanRead) ? uFlags : (uFlags | MF_GRAYED), IDM_COPY, _T("复制")) ;

		hdui::AppendMenu(hMenu, MF_BYCOMMAND | MF_SEPARATOR, 0, NULL) ;

		hdui::AppendMenu(hMenu, !bEmpty ? uFlags : (uFlags | MF_GRAYED), IDM_SELECTALL, _T("全选")) ;

		CPoint ptScreen(point) ;
		ClientToScreen(&ptScreen) ;

		m_bPopMenu = TRUE ;
		int nMenuID = hdui::TrackPopupMenu(
			hMenu,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
			ptScreen.x, ptScreen.y,
			GetCenter()->GetHost(),
			NULL
			) ;
		m_bPopMenu = FALSE ;

		hdui::DestroyMenu(hMenu) ;

		// 处理菜单选项
		ProcessMenuCommand(nMenuID) ;
	}
}

void WLEditReadOnlyImp::ProcessMenuCommand(int nMenuID) 
{
	switch (nMenuID)
	{
	case 0 :
		break ;
	case IDM_COPY :
		{
			Copy() ;
		}
		break ;
	case IDM_SELECTALL :
		{
			SetSel(0, -1) ;
		}
		break ;
	}
}


void WLEditReadOnlyImp::OnSetFocus(HWL pOldWL)
{
	m_bFocus = TRUE ;
	m_bShowCaret = TRUE ;
	SetSel(0, -1) ;
	UpdateView() ;
}

void WLEditReadOnlyImp::OnKillFocus(HWL pNewWL)
{
	m_bFocus = FALSE ;
	m_bShowCaret = FALSE ;
	UpdateView() ;
}

BOOL WLEditReadOnlyImp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetCapture() == m_wrapper)
	{
		return FALSE ;
	}

	// 计算当前文本及文本位置

	BOOL bRes = TRUE ;
	if (nChar == VK_LEFT)
	{
		if (m_nCaretPos > 0)
		{
			int nOldCaretPos = m_nCaretPos ;
			-- m_nCaretPos ;

			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				if (!m_bBeginSel)
				{
					m_bBeginSel = TRUE ;
					m_nFixSel = nOldCaretPos ;
				}
				if (m_nCaretPos < m_nFixSel)
				{
					m_nLeftSel = m_nCaretPos ;
					m_nRightSel = m_nFixSel ;
				}
				else
				{
					m_nRightSel = m_nCaretPos ;
					m_nLeftSel = m_nFixSel ;
				}
			}
			else
			{
				if (m_bBeginSel)
					m_bBeginSel = FALSE ;
				m_nLeftSel = m_nRightSel = m_nCaretPos ;
			}

			bRes = TRUE ;
		}
	}
	else if (nChar == VK_RIGHT)
	{
		CString &text = GetWindowTextRef();
		if (m_nCaretPos < text.GetLength())
		{
			int nOldCaretPos = m_nCaretPos ;
			m_nCaretPos ++ ;

			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				if (!m_bBeginSel)
				{
					m_bBeginSel = TRUE ;
					m_nFixSel = nOldCaretPos ;
				}
				if (m_nCaretPos < m_nFixSel)
				{
					m_nLeftSel = m_nCaretPos ;
					m_nRightSel = m_nFixSel ;
				}
				else
				{
					m_nRightSel = m_nCaretPos ;
					m_nLeftSel = m_nFixSel ;
				}
			}
			else
			{
				if (m_bBeginSel)
					m_bBeginSel = FALSE ;
				m_nLeftSel = m_nRightSel = m_nCaretPos ;
			}

			bRes = TRUE ;
		}
	}
	else if (nChar == VK_HOME)
	{
		if (m_nCaretPos != 0 || m_nLeftSel != 0 || m_nRightSel != 0)
		{
			int nOldCaretPos = m_nCaretPos ;
			m_nCaretPos = 0 ;
			m_nLeftSel = m_nRightSel = m_nCaretPos ;

			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				if (!m_bBeginSel)
				{
					m_bBeginSel = TRUE ;
					m_nFixSel = nOldCaretPos ;
				}
				if (m_nCaretPos < m_nFixSel)
				{
					m_nLeftSel = m_nCaretPos ;
					m_nRightSel = m_nFixSel ;
				}
				else
				{
					m_nRightSel = m_nCaretPos ;
					m_nLeftSel = m_nFixSel ;
				}
			}
			else
			{
				if (m_bBeginSel)
					m_bBeginSel = FALSE ;
				m_nLeftSel = m_nRightSel = m_nCaretPos ;
			}

			bRes = TRUE ;
		}
	}
	else if (nChar == VK_END)
	{
		int nLastIndex = m_vecCharacterRect.size();
		if (m_nCaretPos != nLastIndex || m_nLeftSel != nLastIndex || m_nRightSel != nLastIndex)
		{
			int nOldCaretPos = m_nCaretPos ;
			m_nCaretPos = m_vecCharacterRect.size();
			m_nLeftSel = m_nRightSel = m_nCaretPos ;

			// 未实现，需要m_nFixSel
			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				if (!m_bBeginSel)
				{
					m_bBeginSel = TRUE ;
					m_nFixSel = nOldCaretPos ;
				}
				if (m_nCaretPos < m_nFixSel)
				{
					m_nLeftSel = m_nCaretPos ;
					m_nRightSel = m_nFixSel ;
				}
				else
				{
					m_nRightSel = m_nCaretPos ;
					m_nLeftSel = m_nFixSel ;
				}
			}
			else
			{
				if (m_bBeginSel)
					m_bBeginSel = FALSE ;
				m_nLeftSel = m_nRightSel = m_nCaretPos ;
			}

			bRes = TRUE ;
		}
	}
	else if (nChar == VK_RETURN)
	{
		bRes = FALSE ;
	}
	m_bShowCaret = TRUE ;
	UpdateView() ;

// 	dc.SelectObject(pOldFont) ;
	return bRes ;
}

int WLEditReadOnlyImp::HitTestCaretPos(CPoint point) 
{
	if ( point.x < m_rcTextBound.left )
	{
		point.x = m_rcTextBound.left;
	}
	else if ( point.x >= m_rcTextBound.right )
	{
		point.x = m_rcTextBound.right -1;
	}

	if ( point.y < m_rcTextBound.top )
	{
		point.y = m_rcTextBound.top;
	}
	else if ( point.y >= m_rcTextBound.bottom )
	{
		point.y = m_rcTextBound.bottom - 1;
	}
	int i = 0 ;
	if ( m_rcTextBound.PtInRect( point ))
	{
		std::vector<CRect>::iterator pos = m_vecCharacterRect.begin() ;
		CRect rcFormer( 0, 0, 0, 0 );
		for ( ; pos != m_vecCharacterRect.end() ; ++ pos, ++ i)
		{
			CRect rcTmp = *pos;
			rcTmp.InflateRect( 0, 0, 0, m_nRowInterval );
			if ( rcTmp.PtInRect( point ))
			{
				if ( point.x < rcTmp.CenterPoint().x )
				{
					return i  ;
				}
				else
					return i + 1 ;
			}
			if ( rcTmp.left < rcFormer.left )
			{
				if ( rcFormer.top <= point.y && rcFormer.bottom > point.y && rcFormer.right < point.x )
				{
					return i;
				}
			}
			rcFormer = rcTmp;
		}
	}
	else
	{
		ASSERT( FALSE );
	}
	return i;
}

BOOL WLEditReadOnlyImp::OnSetCursor(UINT message)
{
	//TRACE(_T("[%d]WLEditReadOnly::OnSetCursor\n"), GetTickCount()) ;
	if (m_bPopMenu)
	{
		// 弹出菜单时光标由父决定，其实这样也不太合理，最好由菜单所属窗口决定
		// 但无伤大雅
		//TRACE(_T("[%d]WLEditReadOnly::OnSetCursor\n"), GetTickCount()) ;
		return FALSE ;
	}
	else
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM)) ;
	return TRUE ;
}

int WLEditReadOnlyImp::GetFirstVisiblePos() const 
{
	return 0 ;
}


void WLEditReadOnlyImp::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (m_bBeginSel)
	if (m_bLDown)
	{
		//m_bBeginSel = FALSE ;
		m_bLDown = FALSE ;

		if (GetCapture() == m_wrapper)
			ReleaseCapture() ;
	}
}

void WLEditReadOnlyImp::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (m_bBeginSel)
	if (m_bLDown)
	{
		m_nCaretPos = HitTestCaretPos(point) ;
		if (m_nCaretPos < m_nFixSel)
		{
			m_nLeftSel = m_nCaretPos ;
			m_nRightSel = m_nFixSel ;
		}
		else
		{
			m_nLeftSel = m_nFixSel ;
			m_nRightSel = m_nCaretPos ;
		}
		UpdateView() ;
	}
}

void WLEditReadOnlyImp::SetMargin(
			   int nLeftMargin,
			   int nTopMargin,
			   int nRightMargin,
			   int nBottomMargin
			   ) 
{
	nLeftMargin != -1 ? (m_nLeftMargin = nLeftMargin) : 0 ;
	nTopMargin != -1 ? (m_nTopMargin = nTopMargin) : 0 ;
	nRightMargin != -1 ? (m_nRightMargin = nRightMargin) : 0 ;
	nBottomMargin != -1 ? (m_nBottomMargin = nBottomMargin) : 0 ;

	CSize szClient;
	GetSize(&szClient);
	m_rcTextBound.SetRect(
		m_nLeftMargin, m_nTopMargin,
		szClient.cx - m_nRightMargin,
		szClient.cy - m_nBottomMargin
		) ;
}
void WLEditReadOnlyImp::GetMargin(LPRECT lprcMargin) const
{
	if (lprcMargin == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}
	::SetRect(
		lprcMargin,
		m_nLeftMargin, m_nTopMargin, m_nRightMargin, m_nBottomMargin
		) ;
}

void WLEditReadOnlyImp::SetSel(int nStartChar, int nEndChar) 
{
	if (nStartChar < -1 || nEndChar < -1)
		return ;

	if (nStartChar == -1)
		nStartChar = m_vecCharacterRect.size();

	if (nEndChar == -1)
		nEndChar = m_vecCharacterRect.size();

	if (nStartChar > nEndChar)
		return ;

	m_nLeftSel = nStartChar ;
	m_nRightSel = nEndChar ;
	m_nCaretPos = nEndChar ;

	UpdateView() ;
}

void WLEditReadOnlyImp::LimitText(int nChars) 
{
	if (nChars >= 0)
		m_nMaxChars = nChars ;
}
BOOL WLEditReadOnlyImp::IsEmpty() const 
{ 
	CString text;
	GetWindowText(text);
	return text.IsEmpty() ; 
}

Color WLEditReadOnlyImp::SetBkgndColor(Color crBkgndColor) 
{
	Color crOldBkCOlor = m_crBkgndColor ;
	m_crBkgndColor = crBkgndColor ;
	return crOldBkCOlor ;
}

COLORREF WLEditReadOnlyImp::SetTextNormalColor(COLORREF crTextNormal) 
{
	COLORREF crOldTextNormalColor = m_crTextNormalColor ;
	m_crTextNormalColor = crTextNormal ;
	return crOldTextNormalColor ;
}

void WLEditReadOnlyImp::Copy() 
{
	if (GetStyle() & ES_PASSWORD)
		return ;

	if (m_nLeftSel >= m_nRightSel)
		return ;

	if (!OpenClipboard(GetCenter()->GetHost()))
		return ;
	EmptyClipboard() ;

	int nLength = m_nRightSel - m_nLeftSel ;
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, (nLength + 1) * sizeof(wchar_t)) ;
	if (hData == NULL)
		return ;

	wchar_t * pszText = (wchar_t *)GlobalLock(hData) ;
	CString &text = GetWindowTextRef();
	memcpy(pszText, text.GetString() + m_nLeftSel, nLength * sizeof(wchar_t)) ;
	pszText[nLength] = 0 ;
	GlobalUnlock(hData) ;

	SetClipboardData(CF_UNICODETEXT, hData) ;
	CloseClipboard() ;
}

void WLEditReadOnlyImp::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetSel(0, -1) ;
}

void WLEditReadOnlyImp::OnCaptureChanged(HWL hWl) 
{
	m_bLDown = FALSE ;
	//m_bBeginSel = FALSE ;
}

bool WLEditReadOnlyImp::isSelected( int nIndex )
{
	return nIndex >= m_nLeftSel && nIndex < m_nRightSel;
}

void WLEditReadOnlyImp::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetCapture() == m_wrapper)
	{
		return ;
	}

	if (m_bBeginSel)
		m_bBeginSel = FALSE ;

	// 计算当前文本及文本位置
	CDC dc ;
	HDC hDC = ::GetWindowDC(GetCenter()->GetHost()) ;
	dc.Attach(hDC) ;

	CFont * pFont = CFont::FromHandle((HFONT)::SendMessage(GetCenter()->GetHost(), WM_GETFONT, 0, 0)) ;
	CFont * pOldFont = dc.SelectObject(pFont) ;

	 if (nChar == 1)	// Ctrl + A
	{
		// 全选
		SetSel(0, -1) ;
	}
	else if (nChar == 3)	// Ctrl + C
	{
		// 复制
		Copy() ;
	}
	m_bShowCaret = TRUE ;
	UpdateView() ;

	dc.SelectObject(pOldFont) ;
	dc.Detach() ;
	::ReleaseDC(GetCenter()->GetHost(), hDC) ;
}

void WLEditReadOnlyImp::CalculateCharaterRect()
{
	if ( m_vecCharacterRect.size() > 0 )
	{
		// 计算每个字符所在Rect
		int nWidth = 0;
		int nHeight = 0;
		m_vecRowsHeight.clear();
		int i ;
		for (i = 0 ; i < (int)m_vecCharacterRect.size() ; ++ i)
		{
			CSize size( m_vecCharacterRect[i].Width(), m_vecCharacterRect[i].Height() );

			m_vecCharacterRect[i] = CRect( 0, 0, size.cx, size.cy );

			nWidth += size.cx;
			if ( nWidth > m_rcTextBound.Width() || i == 0 )
			{
				nWidth = size.cx;
				if ( i != 0 )
				{
					nHeight += ( m_vecRowsHeight.back() + m_nRowInterval );
				}
				m_vecRowsHeight.push_back( size.cy );
			}
			else
			{
				if ( size.cy > m_vecRowsHeight.back() )
				{
					m_vecRowsHeight.back() = size.cy;
				}
			}
			m_vecCharacterRect[i].OffsetRect( m_nLeftMargin + nWidth - size.cx, m_nTopMargin + nHeight );
		}
		m_rcTextBound.bottom = nHeight + m_nTopMargin + m_vecRowsHeight.back();
		SetWndLessPos(0, 0, 0, m_rcTextBound.bottom + m_nBottomMargin, SWLP_CY|SWLP_DEFER);
		//m_szClient.cy = m_rcTextBound.bottom + m_nBottomMargin;
	}
}


template<>
UILIB_EXPORT WLEditReadOnlyImpBase *CreateDefaultImp< WLEditReadOnlyImpBase >(IWLEditReadOnly * wrapper)

{
	WLEditReadOnlyImp *ret = new WLEditReadOnlyImp();
	ret->SetWrapper(wrapper);
	return ret;
};
