#include "StdAfx.h"
#include "WLSearchInput.h"

#include "uilib/common/utils/ImageEx/ImageEx.h"

WLSearchInput::WLSearchInput(void)
:
m_hOwner(NULL)
, m_hklBeforeSetFocus(NULL)
{
	m_crCueBanner = RGB(178, 210, 232) ;
}

WLSearchInput::~WLSearchInput(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

void WLSearchInput::SetSearchIcon(UINT uMagnifierId) 
{
	m_imgMagnifier = Shared_ImageEx::FromId(uMagnifierId) ;
}

void WLSearchInput::SetOwner(HWL hOwner) 
{
	m_hOwner = hOwner ;
}

void WLSearchInput::EndSearch() 
{
	ClearSearch() ;

	if (m_dwStatus & BES_EDIT)
	{
		RevertIME() ;
		__super::EndEdit() ;
	}

}

void WLSearchInput::BeginEdit() 
{
	SetEnglishIME() ;
	__super::BeginEdit() ;

	if (m_hOwner != NULL)
	{
		NMSEARCHINFO si ;
		si.hdr.hWlFrom = this ;
		si.hdr.idFrom = GetID() ;
		si.hdr.code = SBN_BEGINSEARCH ;
		m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
	}
}

void WLSearchInput::EndEdit() 
{
	if (m_dwStatus & BES_EDIT)
	{
		RevertIME() ;
		__super::EndEdit() ;
	}

	if (m_hOwner != NULL)
	{
		NMSEARCHINFO si ;
		si.hdr.hWlFrom = this ;
		si.hdr.idFrom = GetID() ;
		si.hdr.code = SBN_ENDSEARCH ;
		m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLSearchInput::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (!__super::OnCreate(dwStyle, x, y, cx, cy, pParent) )
		return FALSE ;

	m_wlEdit.SetMargin(0, 0, 0, 0) ;

	DWORD dwEditStyle = m_wlEdit.GetWndLessStyle() ;
	dwEditStyle |= WLEdit::WLES_NOHIDESEL ;
	m_wlEdit.SetWndLessStyle(dwEditStyle) ;

	m_wlEdit.AddTooltip(m_sCueBanner) ;

	int nSide = 8 ;
	int nOffset = (cy - nSide) / 2 ;
	if (!m_wlBtnClearSearchKeyword.Create(0, cx - nSide - nOffset, nOffset, 8, 8, this, CTRLID_BTN_CLEAR_SEARCH_KEYWORD)) 
		return FALSE ;

	m_wlBtnClearSearchKeyword.AddTooltip(_T("清除")) ;

	return TRUE ;
}

BOOL WLSearchInput::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_KILLFOCUS :
		{
			HWL hwlFocus = (HWL)wParam ;
			if (hwlFocus != NULL && IsChild(hwlFocus, TRUE))
			{
			}
			else
			{
				RevertIME() ;
			}
			return  FALSE ;
		}
		break ;
	case WM_LDOWNHOST :
		{
			//给父亲处理
			return  FALSE ;
		}
		break ;

	case WM_KEYDOWN :
		{
			if (wParam == VK_UP)
			{
				NMSEARCHINFO si ;
				si.hdr.hWlFrom = this ;
				si.hdr.idFrom = GetID() ;
				si.hdr.code = SBN_UP ;
				m_wlEdit.GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				bRes = TRUE ;
			}
			else if (wParam == VK_DOWN)
			{
				NMSEARCHINFO si ;
				si.hdr.hWlFrom = this ;
				si.hdr.idFrom = GetID() ;
				si.hdr.code = SBN_DOWN ;
				m_wlEdit.GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				bRes = TRUE ;
			}
			else if (wParam == VK_RETURN)
			{
				NMSEARCHINFO si ;
				si.hdr.hWlFrom = this ;
				si.hdr.idFrom = GetID() ;
				si.hdr.code = SBN_RETURN ;
				m_wlEdit.GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				EndSearch() ;

				bRes = TRUE ;
			}
			else if (wParam == VK_ESCAPE)
			{
				EndSearch() ;
				EndEdit();
				bRes = TRUE ;
			}
			return bRes ;
		}
		break ;

	case WM_COMMAND :
		{
			WORD wID = LOWORD(wParam) ;
			WORD wCode = HIWORD(wParam) ;
			if (wID == CTRLID_EDIT)
			{
				if (wCode == EN_CHANGE)
                {
                    // 文本变换，则显示编辑框
                    m_wlEdit.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
                    m_dwStatus |= BES_EDIT ;

					if (m_hOwner != NULL)
					{
						NMSEARCHINFO si ;
						si.hdr.hWlFrom = this ;
						si.hdr.idFrom = GetID() ;
						si.hdr.code = SBN_UPDATESEARCH ;
						m_wlEdit.GetWindowText(si.sKeyWord) ;
						m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
					}

					if (m_dwStatus & SBS_HASCONTENT)
					{
						if (m_wlEdit.IsEmpty())
						{
							ClearSearch() ;
						}
					}
					else
					{
						if (!m_wlEdit.IsEmpty())
						{
							m_dwStatus |= SBS_HASCONTENT ;
							m_wlEdit.SetWndLessPos(0, 0, -20, 0, SWLP_CX | SWLP_CX_OFFSET) ;
							m_wlBtnClearSearchKeyword.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
							UpdateView() ;
						}
					}
				}
			}
			else if (wID == CTRLID_BTN_CLEAR_SEARCH_KEYWORD)
			{
				if (wCode == BN_CLICKED)
				{
					OnBnClickClearSearchKeyword() ;
				}
			}
		}
		break ;

	case WM_NOTIFY :
		{
			bRes = OnNotify(wParam, lParam, pResult) ;
		}
		break ;

	case WM_ENDSEARCH :
		{
			EndSearch() ;
			EndEdit();
			bRes = TRUE ;
		}
		break ;
	}

	if (!bRes)
	{
		bRes = __super::OnWndMsg(message, wParam, lParam, pResult) ;
	}
	return bRes ;
}

void WLSearchInput::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	if (m_imgBubble == NULL)
		return ;

	int nUnitIndex = 0 ;
	if (m_dwStatus == BES_NORMAL)
		nUnitIndex = 0 ;
	else if (m_dwStatus & SBS_HASCONTENT)
		nUnitIndex = 2 ;
	else
		nUnitIndex = 1 ;

	// 绘制背景
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;

	int nUnitWidth = m_imgBubble->GetWidth() / 3 ;
	int nUnitHeight = m_imgBubble->GetHeight() ;
	int nPieceWidth = nUnitWidth / 3 ;

	int nX = nUnitIndex * nUnitWidth ;

	Graphics gph(hDC) ;
	gph.DrawImage(
		m_imgBubble,
		Rect(rcInHost.left, rcInHost.top, nPieceWidth, nUnitHeight),
		nX, 0, nPieceWidth, nUnitHeight, UnitPixel, NULL
		) ;
	gph.DrawImage(
		m_imgBubble,
		Rect(rcInHost.left + nPieceWidth, rcInHost.top, rcInHost.right - rcInHost.left - nPieceWidth - nPieceWidth, nUnitHeight),
		nX + nPieceWidth, 0, nPieceWidth - 1, nUnitHeight, UnitPixel, NULL
		) ;
	gph.DrawImage(
		m_imgBubble,
		Rect(rcInHost.right - nPieceWidth, rcInHost.top, nPieceWidth, nUnitHeight),
		nX + nUnitWidth - nPieceWidth, 0, nPieceWidth, nUnitHeight, UnitPixel, NULL
		) ;

	// 绘制空提示
	if (!(m_dwStatus & BES_EDIT))
	{
		BOOL bEmpty = m_wlEdit.IsEmpty() ;
		if (bEmpty && !m_sCueBanner.IsEmpty())
		{
			CRect rcText(rcInHost) ;
			CRect rcEditMargin ;
			m_wlEdit.GetMargin(&rcEditMargin) ;
			rcText.DeflateRect(
				/*m_rcMargin.left + rcEditMargin.left,*/
				4,
				m_rcMargin.top + rcEditMargin.top,
				m_rcMargin.left + rcEditMargin.left,
				m_rcMargin.bottom + rcEditMargin.bottom
				) ;

			int nXOffset = rcText.left ;
			if (m_imgMagnifier != NULL)
			{
				int nWidth = m_imgMagnifier->GetWidth() ;
				int nHeight = m_imgMagnifier->GetHeight() ;
				int nTop = (rcText.top + rcText.bottom - nHeight) / 2 ;
				Graphics gph(hDC) ;
				gph.DrawImage(
					m_imgMagnifier,
					Rect(rcText.left, nTop, nWidth, nHeight),
					0, 0, nWidth, nHeight, UnitPixel, NULL
					) ;

				rcText.left += (nWidth + 5) ;
			}

			int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
			COLORREF crOldText = ::SetTextColor(hDC, m_crCueBanner) ;

			::DrawText(hDC, m_sCueBanner, m_sCueBanner.GetLength(), rcText, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS) ;

			::SetTextColor(hDC, crOldText) ;
			::SetBkMode(hDC, nOldBkMode) ;
		}
	}
}

void WLSearchInput::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	int nSide = 8 ;
	int nOffset = (szNew.cy - nSide) / 2 ;
	m_wlBtnClearSearchKeyword.SetWndLessPos(szNew.cx - nSide - nOffset, nOffset, 0, 0, SWLP_X | SWLP_Y) ;
	__super::OnSized( szOld, szNew );
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

BOOL WLSearchInput::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = __super::OnNotify(wParam, lParam, pResult) ;
	if (bRes)
		return TRUE ;

	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
	switch (wParam) 
	{
	case CTRLID_BTN_CLEAR_SEARCH_KEYWORD :
		{
			switch (pNMWLHDR->code)
			{
			case NM_CUSTOMDRAW :
				{
					OnBtnExitCustomDraw(pNMWLHDR, pResult) ;
					bRes = TRUE ;
				}
				break ;
			}
		}
		break ;
	}

	return bRes ;
}

void WLSearchInput::OnBtnExitCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	WLButton::LPNMCDWLBTNINFO pBtnInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;
	*pResult = 1 ;

	COLORREF crCross ;
	if (pBtnInfo->dwStatus == WLButton::BS_NORMAL)
		crCross = RGB(172, 190, 214) ;
	else
		crCross = RGB(51, 153, 255) ;

	HPEN hPen = ::CreatePen(PS_SOLID, 2, crCross) ;
	HGDIOBJ hOldPen = ::SelectObject(pBtnInfo->hDC, hPen) ;

	RECT &rcBtn = pBtnInfo->rc ;
	::MoveToEx(pBtnInfo->hDC, rcBtn.left, rcBtn.top, NULL) ;
	::LineTo(pBtnInfo->hDC, rcBtn.right - 1, rcBtn.bottom - 1) ;
	::MoveToEx(pBtnInfo->hDC, rcBtn.right - 1, rcBtn.top, NULL) ;
	::LineTo(pBtnInfo->hDC, rcBtn.left, rcBtn.bottom - 1) ;

	::SelectObject(pBtnInfo->hDC, hOldPen) ;
	::DeleteObject(hPen) ;
}

void WLSearchInput::OnBnClickClearSearchKeyword() 
{
	EndSearch() ;
	EndEdit();
}

//////////////////////////////////////////////////////////////////////////
// Inner Functions

void WLSearchInput::ClearSearch() 
{
	if (m_dwStatus & SBS_HASCONTENT)
	{
		m_dwStatus &= ~SBS_HASCONTENT ;
		m_wlEdit.SetWndLessPos(0, 0, 20, 0, SWLP_CX | SWLP_CX_OFFSET) ;
		m_wlEdit.SetWindowText(_T("")) ;
		m_wlBtnClearSearchKeyword.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
		UpdateView() ;
	}
}
// 设置英文输入法
void WLSearchInput::SetEnglishIME() 
{
// 	m_hklBeforeSetFocus = GetKeyboardLayout(0) ;
// 	HKL hklNew = (HKL)((DWORD)(m_hklBeforeSetFocus) & 0x0000ffff) ;
// 
// 	CString sLangId ;
// 	sLangId.Format(_T("%08x"), (DWORD)hklNew) ;
// 	LoadKeyboardLayout(sLangId, KLF_ACTIVATE | KLF_SETFORPROCESS) ;
// 	ActivateKeyboardLayout(hklNew, KLF_SETFORPROCESS) ;
}

// 还原输入法
void WLSearchInput::RevertIME() 
{
// 	if (m_hklBeforeSetFocus != NULL)
// 	{
// 		CString sLangId ;
// 		sLangId.Format(_T("%08x"), (DWORD)m_hklBeforeSetFocus) ;
// 		LoadKeyboardLayout(sLangId, KLF_ACTIVATE | KLF_SETFORPROCESS) ;
// 		ActivateKeyboardLayout(m_hklBeforeSetFocus, KLF_SETFORPROCESS) ;
// 	}
}
