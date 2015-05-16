#include "StdAfx.h"
#include "WLDropDownBtn.h"
#include "common/MemoryLeakChecker.h"

WLDropDownBtn::WLDropDownBtn(void)
:
m_dwStatus(BS_NORMAL)
// , m_bLDown(FALSE)
// , m_bPopMenu(FALSE)
{
}

WLDropDownBtn::~WLDropDownBtn(void)
{
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLDropDownBtn::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_MOUSEMOVE :
		{
			OnMouseMove(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSELEAVE :
		{
			//TRACE(_T("[%d]WLDropDownBtn::WM_MOUSELEAVE\n"), GetTickCount()) ;
			OnMouseLeave() ;
		}
		break ;

	case WM_LBUTTONDBLCLK :
		{
			//TRACE(_T("[%d]WM_LBUTTONDBLCLK\n"), GetTickCount()) ;
			OnLButtonDown(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONDOWN :
		{
			//TRACE(_T("[%d]LButtonDown\n"), GetTickCount()) ;
			OnLButtonDown(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			//TRACE(_T("[%d]WLDropDownBtn::LButtonUp\n"), GetTickCount()) ;
			//OnLButtonUp(wParam, CPoint(lParam)) ;
			//bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			//TRACE(_T("[%d]WM_CAPTURECHANGED\n"), GetTickCount()) ;
			//OnCaptureChanged() ;
			//bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLDropDownBtn::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	DDBCUSTOMDRAW cd ;
	cd.hdr.hWlFrom = this ;
	cd.hdr.idFrom = GetID() ;
	cd.hdr.code = NM_CUSTOMDRAW ;
	cd.hdc = hDC ;
	GetRectInHost(&cd.rc) ;
	cd.dwStatus = m_dwStatus ;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&cd) ;
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

void WLDropDownBtn::OnMouseMove(UINT nFlags, CPoint point) 
{
/*	if (m_bLDown)
	{
		if (PtInWndLess(point))
		{
			if (!(m_dwStatus & BS_SELECTED))
			{
				m_dwStatus |= BS_SELECTED ;
				UpdateView() ;
			}
		}
		else
		{
			if (m_dwStatus & BS_SELECTED)
			{
				m_dwStatus &= ~BS_SELECTED ;
				UpdateView() ;
			}
		}
	}
	else*/
	{
		if (!(m_dwStatus & BS_HOT))
		{
			m_dwStatus |= BS_HOT ;
			UpdateView() ;
		}
	}
}

void WLDropDownBtn::OnMouseLeave() 
{
	if (m_dwStatus & BS_HOT)
	{
		m_dwStatus &= ~BS_HOT ;
		UpdateView() ;
	}
}

void WLDropDownBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//TRACE(_T("[%d]WLDropDownBtn::OnLButtonDown\n"), GetTickCount()) ;
	SetFocus() ;
	m_dwStatus |= BS_SELECTED ;
	UpdateView() ;

	// 询问父窗口可否弹出菜单
	NMWLHDR hdr ;
	hdr.hWlFrom = this ;
	hdr.idFrom = GetID() ;
	hdr.code = DDBN_PREDROPDOWN ;
	LRESULT lResult = GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr) ;

	if (lResult != 0)
	{
		goto EXIT ;
	}

	DDBDROPDOWN dropdown ;
	dropdown.hdr.hWlFrom = this ;
	dropdown.hdr.idFrom = GetID() ;
	dropdown.hdr.code = DDBN_DROPDOWN ;
	GetRectInHost(&dropdown.rc) ;
	lResult = GetParent()->SendMessage(WM_NOTIFY, (WPARAM)dropdown.hdr.idFrom, (LPARAM)&dropdown) ;

	// 未处理
	if (lResult == 0)
	{
		// 向父申请菜单句柄
		HHDMENU hMenu = NULL ;
		DDBASKFORMENU afm ;
		afm.hdr.hWlFrom = this ;
		afm.hdr.idFrom = GetID() ;
		afm.hdr.code = DDBN_ASKFORMENU ;
		afm.pMenu = &hMenu ;
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&afm) ;

		// 弹出菜单
		if (hMenu != NULL)
		{
			CRect rcNotOverlap ;
			GetRectInScreen(rcNotOverlap) ;
			int nScreenX ;
			nScreenX = GetSystemMetrics(SM_CXSCREEN) ;
			TPMPARAMS tpmps ;
			tpmps.cbSize = sizeof(tpmps) ;
			::SetRect(
				&tpmps.rcExclude,
				0,
				rcNotOverlap.top - 1,
				nScreenX,
				rcNotOverlap.bottom + 1
				) ;

			// 记录下弹出菜单前的宿主窗口句柄，弹出菜单后检查其是否有效，
			// 如果无效则直接退出
			HWND hHost = GetCenter()->GetHost() ;

			int nMenuID = hdui::TrackPopupMenu(
				hMenu, 
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
				rcNotOverlap.left,
				rcNotOverlap.bottom,
				GetCenter()->GetHost(),
				&tpmps.rcExclude
				) ;

			hdui::DestroyMenu(hMenu) ;

			if (!IsWindow(hHost))
				return ;

			if (nMenuID != 0)
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(nMenuID, 0), 0) ;
		}
	}

    if (GetParent() != NULL)    // 防止此时DropDownBtn已被销毁
    {
        hdr.code = DDBN_POSTDROPDOWN ;
        GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr) ;
    }

EXIT:
	m_dwStatus &= ~BS_SELECTED ;

    if (GetParent() != NULL)    // 防止此时DropDownBtn已被销毁
    	UpdateView() ;
}
/*
void WLDropDownBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bLDown)
		return ;
	m_bLDown = FALSE ;

// 	if (GetCapture() == this)
// 		ReleaseCapture() ;

	if (!PtInWndLess(point))
	{
		goto EXIT ;
	}

	// 询问父窗口可否弹出菜单
	NMWLHDR hdr ;
	hdr.hWlFrom = this ;
	hdr.idFrom = GetID() ;
	hdr.code = DDBN_PREDROPDOWN ;
	LRESULT lResult = GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr) ;

	if (lResult != 0)
	{
		goto EXIT ;
	}

	// 向父申请菜单句柄
	HMENU hMenu = NULL ;
	DDBASKFORMENU afm ;
	afm.hdr.hWlFrom = this ;
	afm.hdr.idFrom = GetID() ;
	afm.hdr.code = DDBN_ASKFORMENU ;
	afm.pMenu = &hMenu ;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&afm) ;

	// 弹出菜单
	if (hMenu != NULL)
	{
		m_bPopMenu = TRUE ;

		CRect rcNotOverlap ;
		GetRectInScreen(rcNotOverlap) ;
		int nScreenX ;
		nScreenX = GetSystemMetrics(SM_CXSCREEN) ;
		TPMPARAMS tpmps ;
		tpmps.cbSize = sizeof(tpmps) ;
		::SetRect(
			&tpmps.rcExclude,
			0,
			rcNotOverlap.top - 1,
			nScreenX,
			rcNotOverlap.bottom + 1
			) ;
// 		int nMenuID = 0 ;
		int nMenuID = TrackPopupMenuEx(
			hMenu, 
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
			rcNotOverlap.left,
			rcNotOverlap.bottom,
			GetCenter()->GetHost(),
			&tpmps
			) ;

		MSG msg ;
		if (PeekMessage(&msg, (HWND)-1, 0, 0, PM_NOREMOVE))
		{
			TRACE(_T("")) ;
		}

		::DestroyMenu(hMenu) ;

		m_bPopMenu = FALSE ;

		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(nMenuID, 0), 0) ;
	}

	hdr.code = DDBN_POSTDROPDOWN ;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr) ;

EXIT:
	m_dwStatus &= ~BS_SELECTED ;
	UpdateView() ;
}

void WLDropDownBtn::OnCaptureChanged() 
{
	if (m_bLDown)
	{
		m_bLDown = FALSE ;
		m_dwStatus &= ~BS_SELECTED ;
		UpdateView() ;
	}
}*/
