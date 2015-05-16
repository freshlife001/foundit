#include "StdAfx.h"
#include "WLBubbleEdit.h"
#include "uilib/common/utils/ImageEx/ImageEx.h"
#include "uilib/common/utils/Graphics.h"

WLBubbleEdit::WLBubbleEdit(void)
:
m_dwStatus(BES_NORMAL)
, m_crText(hdutils::GetTextColor())
, m_crGlowTextHalo(RGB(255, 255, 255))
, m_crGlowTextBkgnd(RGB(200, 200, 200))
, m_crCueBanner(RGB(0x91, 0x91, 0x91))
{
	SetRect(&m_rcMargin, 1, 1, 1, 1) ;
}

WLBubbleEdit::~WLBubbleEdit(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

void WLBubbleEdit::SetBubbleImage(UINT nID) 
{
	m_imgBubble = Shared_ImageEx::FromId(nID) ;
	if (m_imgBubble != NULL)
	{
		int nHeight = m_imgBubble->GetHeight() ;
		SetWndLessPos(0, 0, 0, nHeight, SWLP_CY) ;
	}
}

void WLBubbleEdit::BeginEdit() 
{
	if (!IsWndLessEnable())
		return ;

	if (m_dwStatus & BES_EDIT)
	{
		m_wlEdit.SetFocus() ;
		return ;
	}

	m_dwStatus |= BES_EDIT ;
	if (IsChild(&m_wlEdit, FALSE))
	{
		// 记录编辑之前的文本
		GetWindowText(m_sTextBeforeEdit) ;

		m_wlEdit.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
		m_wlEdit.SetFocus() ;

		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), BEN_BEGINEDIT), (LPARAM)this) ;
	}
}

void WLBubbleEdit::EndEdit() 
{
	if (IsChild(&m_wlEdit, FALSE) && (m_dwStatus & BES_EDIT))
	{
		m_dwStatus = BES_NORMAL ;
		m_wlEdit.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
		UpdateView() ;

		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), BEN_ENDEDIT), (LPARAM)this) ;
	}
}

void WLBubbleEdit::LimitText(int nChars) 
{
	if (IsChild(&m_wlEdit, FALSE))
	{
		m_wlEdit.LimitText(nChars) ;
	}
}

void WLBubbleEdit::SetMargin(int left, int top, int right, int bottom) 
{
	::SetRect(&m_rcMargin, left, top, right, bottom) ;
	if (IsChild(&m_wlEdit, FALSE))
	{
		CSize szClient;
		GetSize(&szClient);
		m_wlEdit.SetWndLessPos(
			m_rcMargin.left, m_rcMargin.top,
			szClient.cx - m_rcMargin.left - m_rcMargin.right,
			szClient.cy - m_rcMargin.top - m_rcMargin.bottom,
			SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY
			) ;
	}
}

// 设置提示文本
void WLBubbleEdit::SetCueBanner(LPCTSTR lpszCueBanner) 
{
	m_sCueBanner = lpszCueBanner ;
}

void WLBubbleEdit::SetCueBannerColor(COLORREF crCueBanner) 
{
	m_crCueBanner = crCueBanner ;
}

// 获得刚好存放文字时的大小
void WLBubbleEdit::CalcWrapContentSize(LPSIZE lpSize) 
{
	if (lpSize == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	m_wlEdit.CalcWrapContentSize(lpSize) ;
	lpSize->cx += (m_rcMargin.left + m_rcMargin.right) ;
	lpSize->cy += (m_rcMargin.top + m_rcMargin.bottom) ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLBubbleEdit::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (!m_wlEdit.Create(
		0,
		m_rcMargin.left, m_rcMargin.top,
		cx - m_rcMargin.left - m_rcMargin.right,
		cy - m_rcMargin.top - m_rcMargin.bottom,
		this, CTRLID_EDIT
		))
		return FALSE ;
	m_wlEdit.SetFrameWidth(0) ;
	m_wlEdit.SetBkgndColor(Color(255, 255, 255, 255)) ;
	m_wlEdit.SetMargin(2, 2, 2, 2) ;

    return __super::OnCreate(dwStyle, x, y, cx, cy, pParent) ;
}

BOOL WLBubbleEdit::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_NOTIFY :
		{
			bRes = OnNotify(wParam, lParam, pResult) ;
		}
		break ;

	case WM_COMMAND :
		{
			bRes = OnCommand(wParam, lParam, pResult) ;
		}
		break ;

	case WM_MOUSEHOVER :
	case WM_MOUSEMOVE :
		{
			if (!(m_dwStatus & BES_HOT))
			{
				m_dwStatus |= BES_HOT ;
				UpdateView() ;
				bRes = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			//SetFocus() ;
			if (!(m_dwStatus & BES_PUSHED))
			{
				if (GetCapture() != this)
					SetCapture() ;

				m_dwStatus |= BES_PUSHED ;
				UpdateView() ;
				bRes = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONUP :
		{
			if (m_dwStatus & BES_PUSHED)
			{
				if (GetCapture() == this)
					ReleaseCapture() ;

				CRect rcInHost ;
				GetRectInHost(rcInHost) ;
				CPoint point(lParam) ;
				if (rcInHost.PtInRect(point))
				{
					BeginEdit() ;
				}

				bRes = TRUE ;
			}
		}
		break ;

	case WM_BEGINEDIT :
		{
			BeginEdit() ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSELEAVE :
		{
			if (m_dwStatus & BES_HOT)
			{
				m_dwStatus &= ~BES_HOT ;
				UpdateView() ;
				bRes = TRUE ;
			}
		}
		break ;

	case WM_KEYDOWN :
		{
			if (wParam == VK_RETURN && m_dwStatus == BES_EDIT)
			{
				EndEdit() ;
			}
			else if (wParam == VK_ESCAPE)
			{
				// 恢复之前的文本
				SetWindowText(m_sTextBeforeEdit) ;
				EndEdit() ;
			}
			bRes = TRUE ;
		}
		break ;

	case WM_SETFOCUS :
		{
			BeginEdit() ;
		}
		break ;

	case WM_KILLFOCUS :
		{
			if (wParam != (WPARAM)&m_wlEdit && m_dwStatus == BES_EDIT)
			{
				EndEdit() ;
				bRes = TRUE ;
			}
		}
		break ;

	case WM_LDOWNHOST :
		{
			//if (m_dwStatus & BES_EDIT)
			{
				EndEdit() ;
				GetParent()->SetFocus() ;
			}
			bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			if (m_dwStatus & BES_PUSHED)
			{
				m_dwStatus &= ~BES_PUSHED ;
				UpdateView() ;
			}
			bRes = TRUE ;
		}
		break ;

	case WM_BKGNDCHANGED :
		{
			if (GetStyle() & WS_GLOWTEXT)
			{
// 				CRect rc ;
// 				GetRectInHost(&rc) ;
// 				BYTE cbBkgnd = hdutils::GetBkgndRoughGrayScale((HDC)wParam, &rc) ;

				BYTE cbBkgnd = (BYTE)wParam ;
				if (cbBkgnd < 128)
				{
					m_crText = RGB(255, 255, 255) ;
					m_crGlowTextHalo = RGB(80, 80, 80) ;
				}
				else
				{
					m_crText = RGB(0, 0, 0) ;
					m_crGlowTextHalo = RGB(233, 233, 233) ;
					//m_crGlowTextHalo = RGB(200, 200, 200) ;
				}
				m_crGlowTextBkgnd = RGB(cbBkgnd, cbBkgnd, cbBkgnd) ;
			}
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLBubbleEdit::OnDraw(HDC hDC, RECT const &/*rcUpdate*/) 
{
	//if (!m_wlEdit.IsWndLessVisible() && m_imgBubble != NULL)
	//if (m_dwStatus != BES_EDIT)

// 	if (m_uID == 51)
// 	{
// 		TRACE(
// 			_T("[%d]WLBubbleEdit::OnDraw: %d, %d, %d, %d\n"),
// 			GetTickCount(), rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom
// 			) ;
// 	}

	{
		int nUnitIndex = 0 ;
		if (m_dwStatus & BES_EDIT)
			nUnitIndex = 1 ;
		else if (m_dwStatus & BES_PUSHED)
			nUnitIndex = 2 ;
		else if (m_dwStatus & BES_HOT)
			nUnitIndex = 1 ;

		int nUnitWidth = m_imgBubble->GetWidth() / 3 ;
		int nUnitHeight = m_imgBubble->GetHeight() ;
		int nPieceWidth = nUnitWidth / 3 ;

		CRect rcInHost ;
		GetRectInHost(rcInHost) ;

		{
			Graphics gph(hDC) ;
			gph.DrawImage(
				m_imgBubble,
				Rect(rcInHost.left, rcInHost.top, nPieceWidth, nUnitHeight),
				nUnitIndex * nUnitWidth, 0, nPieceWidth, nUnitHeight, UnitPixel, NULL
				) ;
			gph.DrawImage(
				m_imgBubble,
				Rect(rcInHost.left + nPieceWidth, rcInHost.top, rcInHost.Width() - nPieceWidth - nPieceWidth, nUnitHeight),
				nUnitIndex * nUnitWidth + nPieceWidth, 0, nPieceWidth, nUnitHeight, UnitPixel, NULL
				) ;
			gph.DrawImage(
				m_imgBubble,
				Rect(rcInHost.right - nPieceWidth, rcInHost.top, nPieceWidth, nUnitHeight),
				nUnitIndex * nUnitWidth + nPieceWidth + nPieceWidth, 0, nPieceWidth, nUnitHeight, UnitPixel, NULL
				) ;
		}

		CString sText ;
		GetWindowText(sText) ;

		BOOL bDrawCueBanner = FALSE ;
		COLORREF crOldText ;
		if (sText.IsEmpty())
		{
			bDrawCueBanner = TRUE ;
			crOldText = ::SetTextColor(hDC, m_crCueBanner) ;
			sText = m_sCueBanner ;
		}
		else
		{
			crOldText = ::SetTextColor(hDC, m_crText) ;
		}

		int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
		CRect rcText(rcInHost) ;
		CRect rcEditMargin ;
		m_wlEdit.GetMargin(&rcEditMargin) ;
		rcText.DeflateRect(
			m_rcMargin.left + rcEditMargin.left,
			m_rcMargin.top + rcEditMargin.top,
			m_rcMargin.left + rcEditMargin.left,
			m_rcMargin.bottom + rcEditMargin.bottom
			) ;

		if (GetStyle() & WS_GLOWTEXT)
		{
			hdutils::DrawGlowText(
				hDC,
				sText,
				sText.GetLength(),
				rcText,
				DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX,
				m_crGlowTextHalo,
				m_crGlowTextBkgnd
				) ;
		}
		else
		{
			::DrawText(
				hDC,
				sText,
				sText.GetLength(),
				rcText,
				DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX
				) ;
		}

		::SetBkMode(hDC, nOldBkMode) ;

		if (bDrawCueBanner)
		{
			::SetTextColor(hDC, crOldText) ;
		}
	}
}

void WLBubbleEdit::OnSized(SIZE const &/*szOld*/, SIZE const &szNew) 
{
	if (IsChild(&m_wlEdit, FALSE))
	{
		m_wlEdit.SetWndLessPos(
			m_rcMargin.left, m_rcMargin.top,
			szNew.cx - m_rcMargin.left - m_rcMargin.right, 
			szNew.cy - m_rcMargin.top - m_rcMargin.bottom,
			SWLP_CX | SWLP_CY
			) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

BOOL WLBubbleEdit::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (wParam)
	{
	case CTRLID_TOOLTIP :
		{
			LPNMHDR pNMHDR = (LPNMHDR)lParam ;

			switch (pNMHDR->code)
			{
			case TTN_GETDISPINFO :
				{
					LPNMTTDISPINFO lpnmtdi = (LPNMTTDISPINFO)lParam ;
					GetWindowText(m_sTooltipText) ;
					lpnmtdi->lpszText = (LPTSTR)m_sTooltipText.GetString() ;
				}
				break ;
			}
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

BOOL WLBubbleEdit::OnCommand(WPARAM wParam, LPARAM /*lParam*/, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	WORD wId = LOWORD(wParam) ;
	WORD wCode = HIWORD(wParam) ;

	switch (wId)
	{
	case CTRLID_EDIT :
		{
			if (wCode == EN_CHANGE)
			{
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), EN_CHANGE), (LPARAM)this) ;
				bRes = TRUE ;
			}
		}
		break ;
	}

	return bRes ;
}

BOOL WLBubbleEdit::SetWindowText(LPCTSTR lpszText) 
{
    BOOL res = m_wlEdit.SetWindowText(lpszText) ;
    UpdateView() ;
	return res ;
}

int WLBubbleEdit::GetWindowText(CString &sText) 
{
	return m_wlEdit.GetWindowText(sText) ;
}
