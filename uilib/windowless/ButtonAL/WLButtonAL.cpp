#include "StdAfx.h"
#include "WLButtonAL.h"
#include "common/utils/Graphics.h"
#include "logex.h"
#include "common/MemoryLeakChecker.h"

// Image * WLButtonAL::s_pImgCheck = NULL ;
// Image * WLButtonAL::s_pImgRadio = NULL ;

BYTE WLButtonALImp::s_arPushStateMap[5][5] =
{
	// normal, hot, pushed, focus/default, disable
	{0, 0, 0, 0, 0},		// 1: normal
	{0, 0, 1, 0, 0},		// 2: normal, pushed
	{0, 1, 2, 0, 0},		// 3: normal, hot, pushed
	{0, 1, 2, 0, 3},		// 4: normal, hot, pushed, disable
	{0, 1, 2, 3, 4},		// 5: normal, hot, pushed, focus/default, disable
} ;

BYTE WLButtonALImp::s_arCheckStateMap[8][8] =
{
	// normal, normal_hot, normal_pushed, normal_disable, selected, selected_hot, selected_pushed, selected_disable
	{0, 0, 0, 0, 0, 0, 0, 0}, // 1: normal
	{0, 0, 0, 0, 1, 1, 1, 1}, // 2: normal, selected
	{0, 1, 2, 1, 2, 2, 2, 2}, // 3: normal, normal_hot, selected
	{0, 1, 2, 3, 2, 2, 2, 3}, // 4: normal, normal_hot, selected, disable
	{0, 1, 2, 2, 0, 0, 0, 0}, // 5: normal, hot, selected(未完成)
	{0, 1, 2, 2, 3, 3, 0, 0}, // 6: normal, hot, selected, disable(未完成)
	{0, 1, 2, 2, 3, 4, 0, 0}, // 7: normal, hot, selected, disable normal, disable selected(未完成)
	{0, 1, 2, 3, 4, 5, 6, 7}, // 8: normal, normal_hot, normal_pushed, normal_disable, selected, selected_hot, selected_pushed, selected_disable
} ;

WLButtonALImp::WLButtonALImp()
{
	m_dwBtnType = PUSH ;
	m_dwBtnStyle = NORMAL ;
	m_dwBkgndStyle = NOBKGND ;
	m_dwFrgndStyle = NOFRGND ;
	m_dwTextStyle = TS_CENTER ;

	m_pImgFrgnd = NULL ;
	m_nFrgndStateCount = 1 ;
	memset(m_arStateMap, 0, sizeof(m_arStateMap)) ;

	m_pImgCheckSign = NULL ;

	m_pImgIcon = NULL ;

	m_nLeftMargin = 0 ;		// 边缘
	m_nRightMargin = 0 ;
	m_nTopMargin = 0 ;
	m_nBottomMargin = 0 ;
	m_nSignTextSpace = 4 ;	// 符号和文本之间的间隙
	m_nIconTextSpace = 4 ;	// 图标和文字之间的间隙

	//m_pDCParentBkgnd = NULL ;

	m_nCheck = 0 ;

	m_bIconAciton = TRUE;

	int i ;
	for (i = 0 ; i < sizeof(m_crText) / sizeof(m_crText[0]) ; ++ i)
	{
		m_crText[i] = hdutils::crTextColor ;
	}
	for (i = 0 ; i < sizeof(m_crBkgnd) / sizeof(m_crBkgnd[0]) ; ++ i)
	{
		m_crBkgnd[i] = RGB(255, 255, 255) ;
	}
}

WLButtonALImp::~WLButtonALImp()
{
}

void WLButtonALImp::GuessStateMap(int nStateCount) 
{
	if (nStateCount < 1 || nStateCount > 6)
		return ;

	m_nFrgndStateCount = nStateCount ;
	if (m_dwBtnType == PUSH)
	{
		memcpy(m_arStateMap, s_arPushStateMap[nStateCount - 1], sizeof(s_arPushStateMap[0])) ;
	}
	else if (m_dwBtnType == CHECK || m_dwBtnType == RADIO)
	{
		memcpy(m_arStateMap, s_arCheckStateMap[nStateCount - 1], sizeof(s_arCheckStateMap[0])) ;
	}
	else
		return ;
}

void WLButtonALImp::SetFrgnd(
						 Image * pImgFrgnd,
						 int nStateCount,
						 DWORD dwFrgndStyle,
						 BOOL bDefer// = FALSE
						 ) 
{
//	if (m_pImgFrgnd != NULL)
//		delete m_pImgFrgnd ;
	m_pImgFrgnd = pImgFrgnd ;
	ASSERT(nStateCount >= 1 && nStateCount <= 6) ;
	GuessStateMap(nStateCount) ;

	m_dwFrgndStyle = dwFrgndStyle ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			//InvalidateRect(NULL, FALSE) ;
			UpdateView() ;
		}
	}
}

// 设置符号
void WLButtonALImp::SetSign(
						Image * pImgCheckSign,
						BOOL bDefer// = FALSE
						) 
{
// 	if (m_pImgCheckSign != NULL)
// 		delete m_pImgCheckSign ;
	m_pImgCheckSign = pImgCheckSign ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetIcon(
						Image * pImgIcon,
						BOOL bDefer,// = FALSE
						BOOL bAction// = TRUE
						) 
{
// 	if (m_pImgIcon != NULL)
// 		delete m_pImgIcon ;
	m_pImgIcon = pImgIcon ;
	m_bIconAciton = bAction;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetType(
						DWORD dwBtnType,
						BOOL bDefer// = FALSE
						) 
{
	m_dwBtnType = dwBtnType ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetStyle(
						 DWORD dwBtnStyle,
						 BOOL bDefer// = FALSE
						 ) 
{
	m_dwBtnStyle = dwBtnStyle ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetBkgndStyle(
							  DWORD dwBkgndStyle,
							  BOOL bUpdate// = FALSE
							  ) 
{
	m_dwBkgndStyle = dwBkgndStyle ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bUpdate)
			UpdateView() ;
	}
}

void WLButtonALImp::SetTextStyle(
							 DWORD dwTextStyle,
							 BOOL bDefer// = FALSE
							 ) 
{
	m_dwTextStyle = dwTextStyle ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetMargin(
						  int nLeft,
						  int nTop,
						  int nRight,
						  int nBottom,
						  BOOL bDefer// = FALSE
						  ) 
{
	m_nLeftMargin = nLeft ;
	m_nTopMargin = nTop ;
	m_nRightMargin = nRight ;
	m_nBottomMargin = nBottom ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetSignTextSpace(
								 int nSignTextSpace,
								 BOOL bDefer// = FALSE
								 )
{
	if (m_nSignTextSpace == nSignTextSpace)
		return ;
	m_nSignTextSpace = nSignTextSpace ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetIconTextSpace(
								 int nIconTextSpace,
								 BOOL bDefer// = FALSE
								 )
{
	if (m_nIconTextSpace == nIconTextSpace)
		return ;
	m_nIconTextSpace = nIconTextSpace ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		if (!bDefer)
		{
			RecalcLayout() ;
			UpdateView() ;
		}
	}
}

void WLButtonALImp::SetBkgndColor(DWORD dwMask, ...) 
{
	va_list marker;

	va_start(marker, dwMask) ;
	int i ;
	for (i = 0 ; i < 6 ; ++ i)
	{
		if (dwMask & (1 << i))
		{
			m_crBkgnd[i] = va_arg(marker, COLORREF) ;
		}
	}
	va_end(marker);
}

void WLButtonALImp::SetTextColor(DWORD dwMask, ...) 
{
	va_list marker;

	va_start(marker, dwMask) ;
	int i ;
	for (i = 0 ; i < 8 ; ++ i)
	{
		if (dwMask & (1 << i))
		{
			m_crText[i] = va_arg(marker, COLORREF) ;
		}
	}
	va_end(marker);
}

void WLButtonALImp::EndDeferRecalcLayout() 
{
	//ASSERT(::IsWindow(GetSafeHwnd())) ;

	//if (::IsWindow(GetSafeHwnd()))
	{
		RecalcLayout() ;
		UpdateView() ;
	}
}

void WLButtonALImp::RecalcLayout() 
{
	BOOL bMayChangeSize = FALSE ;
	CSize szOld, szNew ;
	//CRect rcWnd ;
	//GetWindowRect(rcWnd) ;
	GetSize(&szOld);;
	//szOld = m_szClient ;
	szNew = szOld ;

	// 计算按钮大小(push按钮和pushlike的check、radio按钮有效)
	if (((m_dwBtnType == PUSH) || (m_dwBtnStyle & PUSHLIKE)) && (m_dwBtnStyle & FITFRGNDSIZE))
	{
		switch (m_dwFrgndStyle)
		{
		case NOFRGND :
		case FITXY :
			bMayChangeSize = FALSE ;
			break ;

		case FIXED :
			{
				if (m_pImgFrgnd == NULL || m_pImgFrgnd->GetLastStatus() != Ok)
					return ;

				szNew.cx = m_pImgFrgnd->GetWidth() / m_nFrgndStateCount ;
				szNew.cy = m_pImgFrgnd->GetHeight() ;
				bMayChangeSize = TRUE ;
			}
			break ;

		case FITX :
			{
				if (m_pImgFrgnd == NULL || m_pImgFrgnd->GetLastStatus() != Ok)
					return ;

				szNew.cy = m_pImgFrgnd->GetHeight() ;
				bMayChangeSize = TRUE ;
			}
			break ;

		case FITY :
			{
				if (m_pImgFrgnd == NULL || m_pImgFrgnd->GetLastStatus() != Ok)
					return ;

				szNew.cx = m_pImgFrgnd->GetWidth() / m_nFrgndStateCount;
				bMayChangeSize = TRUE ;
			}
			break ;

		default:
			ASSERT(FALSE) ;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// >>> calc [sign & icon & text] 's pos and size

	// 计算图标文本所占区域
	CSize szSignIconText ;
	CPoint ptSign ;
	CPoint ptIcon ;
	CPoint ptText ;
	CalcSignIconText(szSignIconText, ptSign, ptIcon, ptText) ;

	CPoint ptSignIconText ;
	if (m_dwBtnStyle & FITTEXTSIZE)
	{
		if (m_dwBtnStyle & FITFRGNDSIZE)
		{
			if (m_dwFrgndStyle == FIXED)
			{
				TRACE(_T("Error Style !!! Can not fit fixed foreground while fit text size.\n")) ;
				ASSERT(FALSE) ;
			}
			else if (m_dwFrgndStyle == FITX)
			{
				szNew.cx = m_nLeftMargin + szSignIconText.cx + m_nRightMargin ;
				ptSignIconText.x = m_nLeftMargin ;
				ptSignIconText.y = (szNew.cy - szSignIconText.cy) / 2 ;
			}
			else if (m_dwFrgndStyle == FITY)
			{
				szNew.cy = m_nTopMargin + szSignIconText.cy + m_nBottomMargin ;
				ptSignIconText.x = (szNew.cx - szSignIconText.cx) / 2 ;
				ptSignIconText.y = m_nTopMargin ;
			}
		}
		else
		{
			szNew.cx = m_nLeftMargin + szSignIconText.cx + m_nRightMargin ;
			szNew.cy = m_nTopMargin + szSignIconText.cy + m_nBottomMargin ;
			ptSignIconText.x = m_nLeftMargin ;
			ptSignIconText.y = m_nTopMargin ;

			bMayChangeSize = TRUE ;
		}
	}

	// 如果按钮风格是自适应文本大小，则文本风格无效
	if (!(m_dwBtnStyle & FITTEXTSIZE))
	{
		if (m_dwTextStyle & TS_LEFT)
		{
			ptSignIconText.x = m_nLeftMargin ;
		}
		else if (m_dwTextStyle & TS_RIGHT)
		{
			ptSignIconText.x = szNew.cx - szSignIconText.cx - m_nRightMargin ;
		}
		else	// TS_HCENTER
		{
			ptSignIconText.x = (szNew.cx - szSignIconText.cx + m_nLeftMargin - m_nRightMargin) / 2 ;
		}

		if (m_dwTextStyle & TS_TOP)
		{
			ptSignIconText.y = m_nTopMargin ;
		}
		else if (m_dwTextStyle & TS_BOTTOM)
		{
			ptSignIconText.y = szNew.cy - szSignIconText.cy - m_nBottomMargin ;
		}
		else	// TS_VCENTER
		{
			ptSignIconText.y = (szNew.cy - szSignIconText.cy + m_nTopMargin - m_nBottomMargin) / 2 ;
		}
/*		if (m_dwTextStyle & TS_CENTER)
		{
			// 自动居中
			ptSignIconText.x = (szNew.cx - szSignIconText.cx) / 2 ;
			ptSignIconText.y = (szNew.cy - szSignIconText.cy) / 2 ;
		}
		else
		{
			if (!(m_dwBtnStyle & TS_VERTICAL))
			{
				if (m_dwTextStyle & TS_NEAR)
				{
					ptSignIconText.x = m_nLeftMargin ;
					ptSignIconText.y = (szNew.cy - szSignIconText.cy) / 2 ;		// y方向自动居中
				}
				else if (m_dwTextStyle & TS_FAR)
				{
					ptSignIconText.x = szNew.cx - szSignIconText.cx ;
					ptSignIconText.y = (szNew.cy - szSignIconText.cy) / 2 ;		// y方向自动居中
				}
				else
				{
					ASSERT(FALSE) ;
				}
			}
			else
			{
				if (m_dwTextStyle & TS_NEAR)
				{
					ptSignIconText.x = (szNew.cx - szSignIconText.cx) / 2 ;
					ptSignIconText.y = 0 ;
				}
				else if (m_dwTextStyle & TS_FAR)
				{
					ptSignIconText.x = (szNew.cx - szSignIconText.cx) / 2 ;
					ptSignIconText.y = szNew.cy - szSignIconText.cy ;
				}
				else
				{
					ASSERT(FALSE) ;
				}
			}
		}*/
	}

	m_ptSign.x = ptSignIconText.x + ptSign.x ;
	m_ptSign.y = ptSignIconText.y + ptSign.y ;
	m_ptIcon.x = ptSignIconText.x + ptIcon.x ;
	m_ptIcon.y = ptSignIconText.y + ptIcon.y ;
	m_ptText.x = ptSignIconText.x + ptText.x ;
	m_ptText.y = ptSignIconText.y + ptText.y ;

	// <<< calc [sign & icon & text] 's pos and size
	//////////////////////////////////////////////////////////////////////////

	if (!bMayChangeSize)
		return ;

	if (szOld == szNew)
		return ;

	//SetWindowPos(NULL, 0, 0, szNew.cx, szNew.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE) ;
	SetWndLessPos(0, 0, szNew.cx, szNew.cy, SWLP_CX | SWLP_CY) ;

// 	NMSIZE nm ;
// 	nm.hdr.hwndFrom = GetSafeHwnd() ;
// 	nm.hdr.idFrom = GetDlgCtrlID() ;
// 	nm.hdr.code = NM_SIZECHANGED ;
// 	nm.szOld = szOld ;
// 	nm.szNew = szNew ;
// 	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)nm.hdr.code, (LPARAM)&nm) ;
}

// 目前只计算文本格式为水平时的情况
void WLButtonALImp::CalcSignIconText(
								 CSize &szSignIconText,
								 CPoint &ptSign,
								 CPoint &ptIcon,
								 CPoint &ptText
								 ) 
{
	WLDC dc(this) ;

	szSignIconText.SetSize(0, 0) ;

	BOOL bHasSign = FALSE ;
	CSize szSign(0, 0) ;
	Image * pImgSign = NULL ;
	if (!(m_dwBtnStyle & PUSHLIKE))		// 有pushlike属性则不绘制符号
	{
/*		switch (m_dwBtnType)
		{
		case CHECK :
			pImgSign = s_pImgCheck ;
			break ;

		case RADIO :
			pImgSign = s_pImgRadio ;
			break ;
		}*/
		pImgSign = m_pImgCheckSign ;
	}
	if (pImgSign != NULL && pImgSign->GetLastStatus() == Ok)
	{
		ptSign.x = szSignIconText.cx ;

		bHasSign = TRUE ;
		szSign.cx = pImgSign->GetWidth() / 8 ;
		szSign.cy = pImgSign->GetHeight() ;
		szSignIconText.cx = szSign.cx ;
		szSignIconText.cy = szSign.cy ;
	}

	BOOL bHasIcon = FALSE ;
	CSize szIcon(0, 0) ;
	if (m_pImgIcon != NULL && m_pImgIcon->GetLastStatus() == Ok)
	{
		if (bHasSign)
			szSignIconText.cx += m_nSignTextSpace ;

		ptIcon.x = szSignIconText.cx ;

		bHasIcon = TRUE ;
		szIcon.cx = m_pImgIcon->GetWidth() ;
		szIcon.cy = m_pImgIcon->GetHeight() ;
		szSignIconText.cx += szIcon.cx ;
		if (szSignIconText.cy < szIcon.cy)
			szSignIconText.cy = szIcon.cy ;
	}

	BOOL bHasText = FALSE ;
	CSize szText(0, 0) ;
	CString sText ;
	GetWindowText(sText) ;
	if (!sText.IsEmpty())
	{
		if (bHasIcon)
			szSignIconText.cx += m_nIconTextSpace ;
		else if (bHasSign)
			szSignIconText.cx += m_nSignTextSpace ;

		bHasText = TRUE ;

		ptText.x = szSignIconText.cx ;

		//CFont * pOldFont = dc.SelectObject(/*GetParent()->*/GetFont()) ;
		szText = dc.GetTextExtent(sText) ;
		//dc.SelectObject(pOldFont) ;
		szSignIconText.cx += szText.cx ;

		if (szSignIconText.cy < szText.cy)
			szSignIconText.cy = szText.cy ;
	}

	if (bHasSign)
	{
		ptSign.y = (szSignIconText.cy - szSign.cy) / 2 ;
	}
	if (bHasIcon)
	{
		ptIcon.y = (szSignIconText.cy - szIcon.cy) / 2 ;
	}
	if (bHasText)
	{
		// - 1是为了纠正字体高度误差
		ptText.y = (szSignIconText.cy - szText.cy) / 2 ;
	}
}
/*
BEGIN_MESSAGE_MAP(WLButtonAL, CButton)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &WLButtonAL::OnNMCustomdraw)
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, &WLButtonAL::OnBnClicked)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()
*/


//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

void CalcGlowColor(BYTE bkgnd_gray_scale, COLORREF & text, COLORREF & text_halo, COLORREF & text_bkgnd)
{
	if (bkgnd_gray_scale < 128)
	{
		text = RGB(255, 255, 255) ;
		text_halo = RGB(80, 80, 80) ;
	}
	else
	{
		text = RGB(0, 0, 0) ;
		text_halo = RGB(233, 233, 233) ;
	}
	text_bkgnd = RGB(bkgnd_gray_scale, bkgnd_gray_scale, bkgnd_gray_scale) ;
}

BOOL WLButtonALImp::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if ((dwStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
	{
		m_dwBtnType = CHECK ;
		m_dwTextStyle = TS_LEFT ;
		//dwStyle &= ~BS_AUTOCHECKBOX ;
		//SetWindowLong(m_hWnd, GWL_STYLE, dwStyle) ;
	}
	else if ((dwStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
	{
		m_dwBtnType = RADIO ;
		m_dwTextStyle = TS_LEFT ;
		//dwStyle &= ~BS_AUTORADIOBUTTON ;
		//SetWindowLong(m_hWnd, GWL_STYLE, dwStyle) ;
	}

	if (dwStyle & BS_PUSHLIKE)
	{
		m_dwBtnStyle |= PUSHLIKE ;
		m_dwTextStyle = TS_CENTER ;
	}

	if (dwStyle & WS_GLOWTEXT)
	{
		BYTE cbBkgnd = (BYTE)::SendMessage(GetCenter()->GetHost(), WM_QUERYBKGND, 0, 0) ;
		CalcGlowColor(cbBkgnd, m_crGlowText, m_crGlowTextHalo, m_crGlowTextBkgnd) ;
	}

	RecalcLayout() ;

	return TRUE ;
}

BOOL WLButtonALImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_COMMAND + WM_REFLECT_BASE :
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				*pResult = OnBnClicked() ;
				bRes = TRUE ;
			}
		}
		break ;

	case BM_GETCHECK :
		{
			*pResult = m_nCheck ;
			bRes = TRUE ;
		}
		break ;

	case BM_SETCHECK :
		{
			m_nCheck = int(wParam) ;
			UpdateView() ;

			if ((m_dwBtnType == RADIO) && (wParam == 1))
			{
				//HWND hWndCtrl = GetSafeHwnd() ;
				//while (hWndCtrl != NULL && !(GetWindowLong(hWndCtrl, GWL_STYLE) & WS_GROUP))
				HWL hCtrl = m_wrapper ;
				if (!(hCtrl->GetStyle() & WS_GROUP))
				{
					hCtrl = hCtrl->GetPrevSiblingHwl() ;
					while (hCtrl != NULL && !(hCtrl->GetStyle() & WS_GROUP))
					{
						if (hCtrl->SendMessage(WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
						{
							// not select button
							hCtrl->SendMessage(BM_SETCHECK, 0, 0L);
						}
						hCtrl = hCtrl->GetPrevSiblingHwl() ;
					}
				}

				hCtrl = GetNextSiblingHwl() ;
				while (hCtrl != NULL && !(hCtrl->GetStyle() & WS_GROUP))
				{
					if (hCtrl->SendMessage(WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
					{
						// not select button
						hCtrl->SendMessage(BM_SETCHECK, 0, 0L);
					}
					hCtrl = hCtrl->GetNextSiblingHwl() ;
				} 
			}
		}
		break ;

	case WM_GETDLGCODE :
		if (m_dwBtnType == RADIO)
		{
			*pResult = DLGC_RADIOBUTTON ;
			bRes = TRUE ;
		}
		else if (m_dwBtnType == CHECK)
		{
			*pResult = DLGC_BUTTON ;
			bRes = TRUE ;
		}
		break ;

/*	case WM_SETTEXT :
		{
			LRESULT lRes = CButton::WindowProc(message, wParam, lParam);
			RecalcLayout() ;
			return lRes ;
		}
		break ;*/

	case WM_SETCURSOR :
		{
			bRes = OnSetCursor() ;
		}
		break ;

	case WM_BKGNDCHANGED :
		{
			if (GetStyle() & WS_GLOWTEXT)
			{
				BYTE cbBkgnd = (BYTE)wParam ;
				CalcGlowColor(cbBkgnd, m_crGlowText, m_crGlowTextHalo, m_crGlowTextBkgnd) ;
			}
			bRes = TRUE ;
		}
		break ;

	default :
		break ;
	}

	if (!bRes)
	{
		bRes = __super::OnWndMsg(message, wParam, lParam, pResult) ;
	}

	return bRes ;
}

void WLButtonALImp::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	if (szNew.cx > 20480  || szNew.cy > 20480)
	{
		TCHAR wndName[255];
		::GetWindowText(GetCenter()->GetHost(), wndName, 255);
		LOG_ERROR_C(_T("invalid size ") << GetID() << _T(" ,host name ") << wndName);
		return;
	}
	RecalcLayout() ;
}

BOOL WLButtonALImp::SetWindowText(LPCTSTR lpszText) 
{
	if (!__super::SetWindowText(lpszText))
		return FALSE ;

	RecalcLayout() ;

	return TRUE ;
}

//////////////////////////////////////////////////////////////////////////
// WLButton Implement

BOOL WLButtonALImp::DrawBySelf(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) 
{
/*	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0 ;
	switch (pNMCD->dwDrawStage)
	{
	case CDDS_PREERASE :
		*pResult = CDRF_SKIPDEFAULT ;
		{
			HDC hDC = pNMCD->hdc ;
			CRect rc = pNMCD->rc ;
			UINT uState = pNMCD->uItemState ;

			HDC hMemDC = ::CreateCompatibleDC(hDC) ;
			HBITMAP hMemBmp = ::CreateCompatibleBitmap(hDC, rc.Width(), rc.Height()) ;
			HBITMAP hOldMemBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp) ;

			Draw(hMemDC, rc, uState) ;
			::BitBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), hMemDC, 0, 0, SRCCOPY) ;

			::SelectObject(hMemDC, hOldMemBmp) ;
			::DeleteObject(hMemBmp) ;
			::DeleteDC(hMemDC) ;
		}
		break ;
	}*/
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;
	Draw(hDC, rcInHost, dwStatus) ;
	return TRUE ;
}

void WLButtonALImp::Draw(HDC hDC, CRect const &rc, DWORD dwState) 
{
	switch (m_dwBtnType)
	{
	case PUSH :
		DrawPush(hDC, rc, dwState, TRUE) ;
		break ;

	case CHECK :
		DrawCheckRadio(hDC, rc, dwState) ;
		break ;

	case RADIO :
		DrawCheckRadio(hDC, rc, dwState) ;
		break ;

	case LINK :
		DrawLink(hDC, rc, dwState) ;
		break ;

	default :
		ASSERT(FALSE) ;
	}
}

void WLButtonALImp::DrawPush(HDC hDC, CRect const &rc, DWORD dwState, BOOL bNormalOrCheck) 
{
	int nStatusIndex = 0 ;
	int nCheck = 0 ;
	BOOL bIsDisable = FALSE ;
	if (bNormalOrCheck)
	{
		if (dwState & BS_DISABLED)
		{
			nStatusIndex = 4 ;
			bIsDisable = TRUE ;
		}
		else if (dwState & BS_SELECTED)
			nStatusIndex = 2 ;
		else if (dwState & BS_HOT)
			nStatusIndex = 1 ;
		else if (dwState & BS_FOCUS)
			nStatusIndex = 3 ;
		else
		{
			if (GetStyle() & BS_DEFPUSHBUTTON)
				nStatusIndex = 3 ;
			else
				nStatusIndex = 0 ;
		}
	}
	else
	{
		nCheck = GetCheck() ;

		if (dwState & BS_DISABLED)
		{
			nStatusIndex = 3 ;
			bIsDisable = TRUE ;
		}
		else if (dwState & BS_SELECTED)
			nStatusIndex = 2 ;
		else if (dwState & BS_HOT)
			nStatusIndex = 1 ;
		else
			nStatusIndex = 0 ;

		if (nCheck)
			nStatusIndex += 4 ;
	}
	int nImageIndex = m_arStateMap[nStatusIndex] ;

	// 绘制背景
	switch (m_dwBkgndStyle)
	{
	case NOBKGND :
		break ;
	case HANDPAINT :
		{
			HBRUSH hBr = ::CreateSolidBrush(m_crBkgnd[nStatusIndex]) ;
			::FillRect(hDC, &rc, hBr) ;
			::DeleteObject(hBr) ;
		}
		break ;

	case TRANSLUCENT :
		{
//			if (m_dcBkgnd.GetSafeHdc() != NULL)
//				::BitBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), m_dcBkgnd, 0, 0, SRCCOPY) ;
/*			if (m_pDCParentBkgnd != NULL && m_pDCParentBkgnd->GetSafeHdc() != NULL)
			{
				CRect rcWndInParent ;
				GetWindowRect(rcWndInParent) ;
				GetParent()->ScreenToClient(rcWndInParent) ;
				::BitBlt(
					hDC,
					rc.left, rc.top, rc.Width(), rc.Height(),
					m_pDCParentBkgnd->GetSafeHdc(),
					rcWndInParent.left, rcWndInParent.top,
					SRCCOPY
					) ;
			}*/
		}
		break ;

	default :
		ASSERT(FALSE) ;
	}

	Graphics * pG = new Graphics(hDC) ;
	//const ImageAttributes& imgAttr = SchemeHelper::GetSchemeImageAttributes();

	// 绘制前景
	if (m_pImgFrgnd != NULL && m_pImgFrgnd->GetLastStatus() == Ok)
	{
		int nUnitWidth = m_pImgFrgnd->GetWidth() / m_nFrgndStateCount ;
		int nUnitHeight = m_pImgFrgnd->GetHeight() ;
		int nPieceWidth = nUnitWidth / 3 ;
		int nPieceHeight = nUnitHeight / 3;
		switch (m_dwFrgndStyle)
		{
		case NOFRGND :
			break ;

		case FIXED :
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left, rc.top, nUnitWidth, nUnitHeight),
				nImageIndex * nUnitWidth, 0, nUnitWidth, nUnitHeight, UnitPixel, NULL
				) ;
			break ;

		case FITX :
/*			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left, rc.top, nPieceWidth, rc.Height()),
				nImageIndex * nUnitWidth, 0, nPieceWidth, nUnitHeight, UnitPixel,
				NULL
				) ;
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left + nPieceWidth, rc.top, rc.Width() - nPieceWidth - nPieceWidth, rc.Height()),
				nImageIndex * nUnitWidth + nPieceWidth, 0, nPieceWidth - 1, nUnitHeight, UnitPixel,
				NULL
				) ;
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.right - nPieceWidth, rc.top, nPieceWidth, rc.Height()),
				nImageIndex * nUnitWidth + nPieceWidth + nPieceWidth, 0, nPieceWidth, nUnitHeight, UnitPixel,
				NULL
				) ;*/
			hdutils::LineStretch(
				hDC,
				rc.left, rc.top, rc.Width(), rc.Height(),
				m_pImgFrgnd,
				nImageIndex * nUnitWidth, 0, nUnitWidth, nUnitHeight, NULL, 3, 3
				) ;
			break ;

		case FITY :				// 未实现
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left, rc.top, rc.Width(), nPieceHeight ),
				nImageIndex * nUnitWidth, 0, nUnitWidth, nPieceHeight, UnitPixel,
				NULL
				) ;
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left, rc.top + nPieceHeight, rc.Width(), rc.Height() - nPieceHeight - nPieceHeight ),
				nImageIndex * nUnitWidth, nPieceHeight, nUnitWidth, nPieceHeight - 1, UnitPixel,
				NULL
				) ;
			pG->DrawImage(
				m_pImgFrgnd,
				Rect(rc.left, rc.bottom - nPieceHeight, rc.Width(), nPieceHeight ),
				nImageIndex * nUnitWidth, nPieceHeight + nPieceHeight, nUnitWidth, nPieceHeight, UnitPixel,
				NULL
				) ;
			break ;

		case FITXY :			// 未实现
			break ;

		default :
			ASSERT(FALSE) ;
		}
	}

	// 绘制图标
	CPoint ptIcon(m_ptIcon) ;
	CPoint ptText(m_ptText) ;

	if (bNormalOrCheck)
	{
		if (dwState & BS_SELECTED)
		{
			if(m_bIconAciton) ptIcon.Offset(1, 1) ;
			ptText.Offset(1, 1) ;
		}
	}
	else
	{
		if (nCheck || (dwState & BS_SELECTED))
		{
			if(m_bIconAciton) ptIcon.Offset(1, 1) ;
			ptText.Offset(1, 1) ;
		}
	}

	if (m_pImgIcon != NULL && m_pImgIcon->GetLastStatus() == Ok)
	{
		/*ImageAttributes  imgAttr;
		ColorMatrix colorMa;
		for ( int i = 0; i < 5; i++ )
		{
			for ( int j = 0; j < 5; j++ )
			{
				colorMa.m[i][j] = (i==j ) ? 1.f : 0.0f;
			}
		}
		if ( bIsDisable )
		{
			colorMa.m[3][3] = 0.5;
		}
		imgAttr.SetColorMatrix( &colorMa );*/
		pG->DrawImage(
			m_pImgIcon,
			Rect( rc.left + ptIcon.x, rc.top + ptIcon.y, m_pImgIcon->GetWidth(), m_pImgIcon->GetHeight()),
			0, 0, m_pImgIcon->GetWidth(), m_pImgIcon->GetHeight(),
			UnitPixel,
			//&imgAttr
			NULL
			) ;
	}

	delete pG ;
	pG = NULL ;

	// 绘制文字
	CString sText ;
	GetWindowText(sText) ;
	if (!sText.IsEmpty())
	{
		int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
		//HFONT hOldFont = (HFONT)::SelectObject(hDC, (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0)) ;

		COLORREF crOldText = ::SetTextColor(hDC, m_crText[nStatusIndex]) ;

		if (GetStyle() & WS_GLOWTEXT)
		{
			::SetTextColor(hDC, m_crGlowText) ;
			RECT rcText ;
			rcText.left = rc.left + ptText.x ;
			rcText.top = rc.top + ptText.y ;
			rcText.right = rc.right ;
			rcText.bottom = rc.bottom ;
			hdutils::DrawGlowText(
				hDC,
				sText,
				sText.GetLength(),
				&rcText,
				DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX,
				m_crGlowTextHalo,
				m_crGlowTextBkgnd
				) ;
		}
		else
		{
			::TextOut(hDC, rc.left + ptText.x, rc.top + ptText.y, sText.GetString(), sText.GetLength()) ;
		}

		::SetTextColor(hDC, crOldText) ;

		//::SelectObject(hDC, hOldFont) ;
		::SetBkMode(hDC, nOldBkMode) ;
	}
}

void WLButtonALImp::DrawCheckRadio(HDC hDC, CRect const &rc, DWORD dwState) 
{
	if (m_dwBtnStyle & PUSHLIKE)
	{
		DrawPush(hDC, rc, dwState, FALSE) ;
		return ;
	}

	// 绘制背景
	switch (m_dwBkgndStyle)
	{
	case NOBKGND :
		break ;
	case HANDPAINT :
		{
			HBRUSH hBr = ::CreateSolidBrush(m_crBkgnd[0]) ;
			::FillRect(hDC, &rc, hBr) ;
			::DeleteObject(hBr) ;
		}
		break ;

	case TRANSLUCENT :
		{
// 			if (m_dcBkgnd.GetSafeHdc() != NULL)
// 				::BitBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), m_dcBkgnd, 0, 0, SRCCOPY) ;
/*			if (m_pDCParentBkgnd != NULL && m_pDCParentBkgnd->GetSafeHdc() != NULL)
			{
				CRect rcWndInParent ;
				GetWindowRect(rcWndInParent) ;
				GetParent()->ScreenToClient(rcWndInParent) ;
				::BitBlt(
					hDC,
					rc.left, rc.top, rc.Width(), rc.Height(),
					m_pDCParentBkgnd->GetSafeHdc(),
					rcWndInParent.left, rcWndInParent.top,
					SRCCOPY
					) ;
			}*/
		}
		break ;

	default :
		ASSERT(FALSE) ;
	}

	// 获得状态索引
	int nStatusIndex = 0 ;

	int nCheck = GetCheck() ;

	if (dwState & BS_DISABLED)
		nStatusIndex = 3 ;
	else if (dwState & BS_SELECTED)
		nStatusIndex = 2 ;
	else if (dwState & BS_HOT)
		nStatusIndex = 1 ;
	else
		nStatusIndex = 0 ;

	if (nCheck)
		nStatusIndex += 4 ;

	// 不绘制前景

	//const ImageAttributes& imgAttr = SchemeHelper::GetSchemeImageAttributes();
	Graphics * pG = new Graphics(hDC) ;
	// 绘制符号
//	Image * pImgSign = bCheckOrRadio ? s_pImgCheck : s_pImgRadio ;
	Image * pImgSign = m_pImgCheckSign ;
	if (pImgSign != NULL && pImgSign->GetLastStatus() == Ok)
	{
		int nUnitWidth = pImgSign->GetWidth() / 8 ;
		int nUnitHeight = pImgSign->GetHeight() ;

		pG->DrawImage(
			pImgSign,
			Rect(rc.left + m_ptSign.x, rc.top + m_ptSign.y, nUnitWidth, nUnitHeight),
			nStatusIndex * nUnitWidth, 0, nUnitWidth, nUnitHeight, UnitPixel, 
			NULL
			) ;
	}

	// 绘制图标
	if (m_pImgIcon != NULL && m_pImgIcon->GetLastStatus() == Ok)
	{
		pG->DrawImage(
			m_pImgIcon,
			rc.left + m_ptIcon.x, rc.top + m_ptIcon.y, m_pImgIcon->GetWidth(), m_pImgIcon->GetHeight()
			) ;
	}

	delete pG ;
	pG = NULL ;

	// 绘制文字
	CString sText ;
	GetWindowText(sText) ;
	if (!sText.IsEmpty())
	{
		int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
		//HFONT hOldFont = (HFONT)::SelectObject(hDC, (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0)) ;

		COLORREF crOldText = ::SetTextColor(hDC, m_crText[nStatusIndex]) ;
//		UINT uOldTextAlign = ::SetTextAlign(hDC, TA_BOTTOM) ;
		::TextOut(hDC, rc.left + m_ptText.x, rc.top + m_ptText.y, sText.GetString(), sText.GetLength()) ;
		::SetTextColor(hDC, crOldText) ;

		//::SelectObject(hDC, hOldFont) ;
		::SetBkMode(hDC, nOldBkMode) ;
	}
}

void WLButtonALImp::DrawLink(HDC hDC, CRect const &rc, DWORD dwState) 
{
	int nStatusIndex = 0 ;
	if (dwState & BS_DISABLED)
		nStatusIndex = 4 ;
	else if (dwState & BS_SELECTED)
		nStatusIndex = 2 ;
	else if (dwState & BS_HOT)
		nStatusIndex = 1 ;
	else if (dwState & BS_FOCUS)
		nStatusIndex = 3 ;
	else
		nStatusIndex = 0 ;

	// 绘制背景
	switch (m_dwBkgndStyle)
	{
	case NOBKGND :
		break ;
	case HANDPAINT :
		{
			HBRUSH hBr = ::CreateSolidBrush(m_crBkgnd[0]) ;
			::FillRect(hDC, &rc, hBr) ;
			::DeleteObject(hBr) ;
		}
		break ;

	case TRANSLUCENT :
		{
// 			if (m_dcBkgnd.GetSafeHdc() != NULL)
// 				::BitBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), m_dcBkgnd, 0, 0, SRCCOPY) ;

/*			if (m_pDCParentBkgnd != NULL && m_pDCParentBkgnd->GetSafeHdc() != NULL)
			{
				CRect rcWndInParent ;
				GetWindowRect(rcWndInParent) ;
				GetParent()->ScreenToClient(rcWndInParent) ;
				::BitBlt(
					hDC,
					rc.left, rc.top, rc.Width(), rc.Height(),
					m_pDCParentBkgnd->GetSafeHdc(),
					rcWndInParent.left, rcWndInParent.top,
					SRCCOPY
					) ;
			}*/
		}
		break ;

	default :
		ASSERT(FALSE) ;
	}

	// 不绘制前景

	Graphics * pG = new Graphics(hDC) ;

	// 绘制图标
	if (m_pImgIcon != NULL && m_pImgIcon->GetLastStatus() == Ok)
	{
		pG->DrawImage(
			m_pImgIcon,
			m_ptIcon.x, m_ptIcon.y, m_pImgIcon->GetWidth(), m_pImgIcon->GetHeight()
			) ;
	}

	delete pG ;
	pG = NULL ;

	// 绘制文字
	CString sText ;
	GetWindowText(sText) ;
	if (!sText.IsEmpty())
	{
		int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
		//HFONT hOldFont = (HFONT)::SelectObject(hDC, (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0)) ;

		COLORREF crOldText = ::SetTextColor(hDC, m_crText[nStatusIndex]) ;

		CSize szText ;
		::GetTextExtentPoint32(hDC, sText, sText.GetLength(), &szText) ;

		::TextOut(hDC, rc.left + m_ptText.x, rc.top + m_ptText.y, sText.GetString(), sText.GetLength()) ;

		// 绘制下划线
		if (dwState & BS_HOT)
		{
			HPEN hPen = CreatePen(PS_SOLID, 1, m_crText[nStatusIndex]) ;
			HGDIOBJ hOldPen = ::SelectObject(hDC, hPen) ;
			::MoveToEx(hDC, rc.left + m_ptText.x, rc.top + m_ptText.y + szText.cy - 1, NULL) ;
			::LineTo(hDC, rc.left + m_ptText.x + szText.cx, rc.top + m_ptText.y + szText.cy - 1) ;
			::DeleteObject(::SelectObject(hDC, hOldPen)) ;
		}

		::SetTextColor(hDC, crOldText) ;

		//::SelectObject(hDC, hOldFont) ;
		::SetBkMode(hDC, nOldBkMode) ;
	}
}

BOOL WLButtonALImp::OnSetCursor()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_dwBtnType == LINK)
	{
		HCURSOR hCursor = ::LoadCursor(NULL, IDC_HAND) ;
		::SetCursor(hCursor) ;

		return TRUE ;
	}

	return FALSE ;
}

BOOL WLButtonALImp::OnBnClicked()
{
	switch (m_dwBtnType)
	{
	case CHECK :
		SetCheck(1 - m_nCheck) ;
		break ;

	case RADIO :
		SetCheck(1) ;
		break ;

	case LINK :
		if (!m_sUrl.IsEmpty())
		{
			ShellExecute(NULL, _T("open"), m_sUrl, NULL, NULL, SW_SHOW) ;
		}
		break ;
	}

	return FALSE ;
}


template<>
UILIB_EXPORT WLButtonALImpBase *CreateDefaultImp< WLButtonALImpBase >(IWLButtonAL * wrapper)

{
	WLButtonALImp *ret = new WLButtonALImp();
	ret->SetWrapper(wrapper);
	return ret;
};
