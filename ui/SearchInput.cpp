#include "StdAfx.h"
#include "SearchInput.h"

#include "uilib\common\utils\ImageEx\ImageEx.h"

CSearchInput::CSearchInput(void)
:
m_hOwner(NULL)
, m_hklBeforeSetFocus(NULL)
, m_hasContent(false)
{
	SetCueBannerColor( RGB(178, 210, 232) ) ;
}

CSearchInput::~CSearchInput(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

void CSearchInput::SetMagnifierImage(UINT uMagnifierId) 
{
	m_imgMagnifier = Shared_ImageEx::FromId(uMagnifierId) ;
}

void CSearchInput::SetOwner(HWL hOwner) 
{
	m_hOwner = hOwner ;
}

void CSearchInput::EndSearch() 
{
	ClearSearch() ;
	GetWLEdit()->SetWindowText(_T("")) ;
	EndEdit() ;
}

void CSearchInput::BeginEdit() 
{
	__super::BeginEdit() ;
}

void CSearchInput::EndEdit() 
{
	if (GetStatus() & IWLBubbleEdit::BES_EDIT)
	{
		__super::EndEdit() ;
	}
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL CSearchInput::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (!__super::OnCreate(dwStyle, x, y, cx, cy, pParent) )
		return FALSE ;

	GetWLEdit()->SetMargin(0, 0, 0, 0) ;

	DWORD dwEditStyle = GetWLEdit()->GetWndLessStyle() ;
	dwEditStyle |= WLEdit::WLES_NOHIDESEL ;
	GetWLEdit()->SetWndLessStyle(dwEditStyle) ;

	//GetWLEdit()->AddTooltip(m_sCueBanner) ;

	int nSide = 8 ;
	int nOffset = (cy - nSide) / 2 ;
	if (!m_wlBtnClearSearchKeyword.Create(0, cx - nSide - nOffset, nOffset, 8, 8, this, CTRLID_BTN_CLEAR_SEARCH_KEYWORD)) 
		return FALSE ;

	m_wlBtnClearSearchKeyword.AddTooltip(_T("Çå³ýËÑË÷")) ;

	return TRUE ;
}

BOOL CSearchInput::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
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
		}
	case WM_LDOWNHOST :
		{
			if (!GetWLEdit()->IsEmpty())
			{
				return bRes ;
			}
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
				GetWLEdit()->GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				bRes = TRUE ;
			}
			else if (wParam == VK_DOWN)
			{
				NMSEARCHINFO si ;
				si.hdr.hWlFrom = this ;
				si.hdr.idFrom = GetID() ;
				si.hdr.code = SBN_DOWN ;
				GetWLEdit()->GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				bRes = TRUE ;
			}
			else if (wParam == VK_RETURN)
			{
				NMSEARCHINFO si ;
				si.hdr.hWlFrom = this ;
				si.hdr.idFrom = GetID() ;
				si.hdr.code = SBN_RETURN ;
				GetWLEdit()->GetWindowText(si.sKeyWord) ;
				m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
				EndSearch() ;

				bRes = TRUE ;
			}
			else if (wParam == VK_ESCAPE)
			{
				EndSearch() ;
				bRes = TRUE ;
			}
			return bRes ;
		}
		break ;

	case WM_COMMAND :
		{
			WORD wID = LOWORD(wParam) ;
			WORD wCode = HIWORD(wParam) ;
			if (wID == GetWLEdit()->GetID())
			{
				GetWLEdit()->SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
				if (wCode == EN_CHANGE)
				{
					if (m_hOwner != NULL)
					{
						NMSEARCHINFO si ;
						si.hdr.hWlFrom = this ;
						si.hdr.idFrom = GetID() ;
						si.hdr.code = SBN_UPDATESEARCH ;
						GetWLEdit()->GetWindowText(si.sKeyWord) ;
						m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
					}

					if (m_hasContent)
					{
						if (GetWLEdit()->IsEmpty())
						{
							ClearSearch() ;
						}
					}
					else
					{
						if (!GetWLEdit()->IsEmpty())
						{
							m_hasContent = true ;
							GetWLEdit()->SetWndLessPos(0, 0, -20, 0, SWLP_CX | SWLP_CX_OFFSET) ;
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

// 	case WM_ENDSEARCH :
// 		{
// 			EndSearch() ;
// 			bRes = TRUE ;
// 		}
// 		break ;
	}

	if (!bRes)
	{
		bRes = __super::OnWndMsg(message, wParam, lParam, pResult) ;
	}
	return bRes ;
}

void CSearchInput::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	__super::OnDraw(hDC, rcUpdate);

	// »æÖÆ±³¾°
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;


	// »æÖÆ¿ÕÌáÊ¾
	if (!(GetStatus() & IWLBubbleEdit::BES_EDIT))
	{
		BOOL bEmpty = GetWLEdit()->IsEmpty() ;
		if (bEmpty)
		{
			CRect rcText(rcInHost) ;

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

		}
	}
}

void CSearchInput::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	int nSide = 8 ;
	int nOffset = (szNew.cy - nSide) / 2 ;
	m_wlBtnClearSearchKeyword.SetWndLessPos(szNew.cx - nSide - nOffset, nOffset, 0, 0, SWLP_X | SWLP_Y) ;
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

BOOL CSearchInput::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE;

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

void CSearchInput::OnBtnExitCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
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

void CSearchInput::OnBnClickClearSearchKeyword() 
{
	ClearSearch() ;
	GetWLEdit()->SetWindowText(_T("")) ;
	GetWLEdit()->SetFocus() ;
}

//////////////////////////////////////////////////////////////////////////
// Inner Functions

void CSearchInput::ClearSearch() 
{
	if (m_hasContent)
	{
		m_hasContent = false ;
		GetWLEdit()->SetWndLessPos(0, 0, 20, 0, SWLP_CX | SWLP_CX_OFFSET) ;
		m_wlBtnClearSearchKeyword.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
		UpdateView() ;

		if (m_hOwner != NULL)
		{
			NMSEARCHINFO si ;
			si.hdr.hWlFrom = this ;
			si.hdr.idFrom = GetID() ;
			si.hdr.code = SBN_ENDSEARCH ;
			m_hOwner->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;
		}
	}
}
