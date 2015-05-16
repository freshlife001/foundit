// CtlPosMgrST.cpp: implementation of the CCtlPosMgrST class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtlPosMgrST.h"

#include "common/MemoryLeakChecker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtlPosMgrST::CCtlPosMgrST()
{

}

CCtlPosMgrST::~CCtlPosMgrST()
{

}

/////////////////////////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////////////////////////

BOOL CCtlPosMgrST::AddMapControl(
								 UINT nIDControl,
								 UINT nMask,
								 ...
								 ) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	va_list marker;

	int nPercent[4] ;
	va_start(marker, nMask) ;
	int i ;
	for (i = 0 ; i < 4 ; ++ i)
	{
		if (nMask & (1 << i))
		{
			nPercent[i] = va_arg(marker, int) ;
		}
	}
	va_end(marker);

	return AddMapControl(hCtrl, nMask, nPercent[0], nPercent[1], nPercent[2], nPercent[3]) ;
}

BOOL CCtlPosMgrST::AddMapControl(
								 HWND hCtrl,			// 子窗口句柄
								 UINT nMask,
								 ...
								 ) 
{
	if (hCtrl == NULL)
		return FALSE ;

	va_list marker;

	int nPercent[4] ;
	va_start(marker, nMask) ;
	int i ;
	for (i = 0 ; i < 4 ; ++ i)
	{
		if (nMask & (1 << i))
		{
			nPercent[i] = va_arg(marker, int) ;
		}
	}
	va_end(marker);

	return AddMapControl(hCtrl, nMask, nPercent[0], nPercent[1], nPercent[2], nPercent[3]) ;
}
/*
BOOL CCtlPosMgrST::AddMapControl(
								 UINT nIDControl,
								 BOOL bX, 
								 int nPercentX,
								 BOOL bY,
								 int nPercentY,
								 BOOL bCX,
								 int nPercentCX,
								 BOOL bCY,
								 int nPercentCY
								 ) 
{
	UINT nMask = 0 ;
	nMask |= (bX) ;
	nMask |= (bY << 1) ;
	nMask |= (bCX << 2) ;
	nMask |= (bCY << 3) ;

	return AddMapControl(nIDControl, nMask, nPercentX, nPercentY, nPercentCX, nPercentCY) ;
}

BOOL CCtlPosMgrST::AddMapControl(
								 UINT nIDControl,
								 UINT nMask,
								 int nPercentX,
								 int nPercentY,
								 int nPercentCX,
								 int nPercentCY
								 ) 
{
	CWnd *pHostWnd = CWnd::FromHandle(m_hWndHost) ;
	if (pHostWnd == NULL)
		return FALSE ;

	CWnd *pCtrl = pHostWnd->GetDlgItem(nIDControl) ;
	if (pCtrl == NULL)
		return FALSE ;

	// 记录父窗口客户区大小
	CRect rcParent ;
	pHostWnd->GetClientRect(rcParent) ;

	// 记录控件窗口位置
	CRect rcCtrl ;
	pCtrl->GetWindowRect(&rcCtrl) ;
	pHostWnd->ScreenToClient(&rcCtrl) ;

	SAMAP::iterator pos = m_mapSA.find(nIDControl) ;
	if (pos != m_mapSA.end())
	{
		m_mapSA[nIDControl]._nMask = nMask ;
		m_mapSA[nIDControl]._nPercentX = nPercentX ;
		m_mapSA[nIDControl]._nPercentY = nPercentY ;
		m_mapSA[nIDControl]._nPercentCX = nPercentCX ;
		m_mapSA[nIDControl]._nPercentCY = nPercentCY ;
		m_mapSA[nIDControl]._rcCtrl = rcCtrl ;
		m_mapSA[nIDControl]._szParent = rcParent.Size() ;
	}
	else
	{
		m_mapSA.insert(SAMAP::value_type(nIDControl, 
			STRETCH_ATTR(nMask, nPercentX, nPercentY, nPercentCX, nPercentCY, rcCtrl, rcParent.Size()))) ;
	}

	return TRUE ;
}
*/
BOOL CCtlPosMgrST::AddMapControl(
								 HWND hCtrl,
								 UINT nMask,			// 控件变化掩码：(cy << 3) | (cx << 2) | (y << 1) | (x)
								 int nPercentX,
								 int nPercentY,
								 int nPercentCX,
								 int nPercentCY
								 ) 
{
	CWnd *pHostWnd = CWnd::FromHandle(m_hWndHost) ;
	if (pHostWnd == NULL)
		return FALSE ;

	CWnd *pCtrl = CWnd::FromHandle(hCtrl) ;
	if (pCtrl == NULL)
		return FALSE ;

	// 记录父窗口客户区大小
	CRect rcParent ;
	pHostWnd->GetClientRect(rcParent) ;

	// 记录控件窗口位置
	CRect rcCtrl ;
	pCtrl->GetWindowRect(&rcCtrl) ;
	pHostWnd->ScreenToClient(&rcCtrl) ;

	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos != m_mapSA.end())
	{
		pos->second._nMask = nMask ;
		pos->second._nPercentX = nPercentX ;
		pos->second._nPercentY = nPercentY ;
		pos->second._nPercentCX = nPercentCX ;
		pos->second._nPercentCY = nPercentCY ;
		pos->second._rcCtrl = rcCtrl ;
		pos->second._szParent = rcParent.Size() ;
	}
	else
	{
		m_mapSA.insert(SAMAP::value_type(hCtrl, 
			STRETCH_ATTR(nMask, nPercentX, nPercentY, nPercentCX, nPercentCY, rcCtrl, rcParent.Size()))) ;
	}

	return TRUE ;
}

// 移除控件
BOOL CCtlPosMgrST::RemoveMapControl(
									UINT nIDControl
									) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	return RemoveMapControl(hCtrl) ;
}

// 移除控件
BOOL CCtlPosMgrST::RemoveMapControl(
					  HWND hCtrl
					  ) 
{
	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos == m_mapSA.end())
		return FALSE ;

	m_mapSA.erase(pos) ;
	return TRUE ;
}

// 锁定控件
BOOL CCtlPosMgrST::LockControl(
							   UINT nIDControl
							   ) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	return LockControl(hCtrl) ;
}

// 锁定窗口
BOOL CCtlPosMgrST::LockControl(
							   HWND hCtrl
							   ) 
{
	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos == m_mapSA.end())
		return FALSE ;

	pos->second._bLock = TRUE ;
	return TRUE ;
}

// 解锁控件
BOOL CCtlPosMgrST::UnLockControl(
								 UINT nIDControl
								 ) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	return UnLockControl(hCtrl) ;
}

// 解锁窗口
BOOL CCtlPosMgrST::UnLockControl(
								 HWND hCtrl
								 ) 
{
	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos == m_mapSA.end())
		return FALSE ;

	if (pos->second._bLock)
	{
		pos->second._bLock = FALSE ;

		// 更新位置属性
		CRect rcParent ;
		::GetClientRect(m_hWndHost, rcParent) ;
		pos->second._szParent = rcParent.Size() ;

// 		WINDOWPLACEMENT wpm ;
// 		wpm.length = sizeof(wpm) ;
// 		::GetWindowPlacement(m_hWndHost, &wpm) ;
// 		pos->second._szParent.cx = wpm.rcNormalPosition.right - wpm.rcNormalPosition.left ;
// 		pos->second._szParent.cy = wpm.rcNormalPosition.bottom - wpm.rcNormalPosition.top ;

		RECT rcCtrl ;
		::GetWindowRect(hCtrl, &rcCtrl) ;
		::ScreenToClient(m_hWndHost, (LPPOINT)&rcCtrl);
		::ScreenToClient(m_hWndHost, ((LPPOINT)&rcCtrl)+1);
		pos->second._rcCtrl = rcCtrl ;
	}

	return TRUE ;
}

// 更新控件位置
BOOL CCtlPosMgrST::UpdateMapControlPos(
										UINT nIDControl		// 控件ID
										) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	return UpdateMapControlPos(hCtrl) ;
}

// 更新控件位置
BOOL CCtlPosMgrST::UpdateMapControlPos(
										HWND hCtrl			// 子窗口句柄
										) 
{
	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos == m_mapSA.end())
		return FALSE ;

	// 更新位置属性
/*	if (!::IsIconic(m_hWndHost))
	{
		CRect rcParent ;
		::GetClientRect(m_hWndHost, rcParent) ;
		pos->second._szParent = rcParent.Size() ;
	}
	else
	{
		WINDOWPLACEMENT wp ;
		wp.length = sizeof(wp) ;
		::GetWindowPlacement(m_hWndHost, &wp) ;
		pos->second._szParent.cx = wp.rcNormalPosition.right - wp.rcNormalPosition.left ;
		pos->second._szParent.cy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top ;
	}*/
	pos->second._szParent = m_szHostClient ;

	RECT rcCtrl ;
	::GetWindowRect(hCtrl, &rcCtrl) ;
	::ScreenToClient(m_hWndHost, (LPPOINT)&rcCtrl);
	::ScreenToClient(m_hWndHost, ((LPPOINT)&rcCtrl)+1);
	pos->second._rcCtrl = rcCtrl ;

	return TRUE ;
}

// 改变控件位置
BOOL CCtlPosMgrST::ChangeMapControlPos(
									   UINT nIDControl, UINT nMask, ...
									   ) 
{
	HWND hCtrl = ::GetDlgItem(m_hWndHost, nIDControl) ;
	if (hCtrl == NULL)
		return FALSE ;

	va_list marker;

	int nOffset[4] ;
	va_start(marker, nMask) ;
	int i ;
	for (i = 0 ; i < 4 ; ++ i)
	{
		if (nMask & (1 << i))
		{
			nOffset[i] = va_arg(marker, int) ;
		}
	}
	va_end(marker);

	return ChangeMapControlPos(hCtrl, nMask, nOffset[0], nOffset[1], nOffset[2], nOffset[3]) ;
}

BOOL CCtlPosMgrST::ChangeMapControlPos(
									   HWND hCtrl, UINT nMask, ...
									   ) 
{
	if (hCtrl == NULL)
		return FALSE ;

	va_list marker;

	int nOffset[4] ;
	va_start(marker, nMask) ;
	int i ;
	for (i = 0 ; i < 4 ; ++ i)
	{
		if (nMask & (1 << i))
		{
			nOffset[i] = va_arg(marker, int) ;
		}
	}
	va_end(marker);

	return ChangeMapControlPos(hCtrl, nMask, nOffset[0], nOffset[1], nOffset[2], nOffset[3]) ;
}

BOOL CCtlPosMgrST::ChangeMapControlPos(
									   HWND hCtrl, UINT nMask, int x, int y, int cx, int cy
									   ) 
{
	SAMAP::iterator pos = m_mapSA.find(hCtrl) ;
	if (pos == m_mapSA.end())
		return FALSE ;

	RECT rcCtrl ;
	::GetWindowRect(hCtrl, &rcCtrl) ;
	::ScreenToClient(m_hWndHost, (LPPOINT)&rcCtrl);
	::ScreenToClient(m_hWndHost, ((LPPOINT)&rcCtrl)+1);

	CPoint point(rcCtrl.left, rcCtrl.top) ;
	CSize size(rcCtrl.right - rcCtrl.left, rcCtrl.bottom - rcCtrl.top) ;
	if (nMask & X)
		point.x += x ;
	if (nMask & Y)
		point.y += y ;
	if (nMask & CX)
		size.cx += cx ;
	if (nMask & CY) 
		size.cy += cy ;

	::SetWindowPos(hCtrl, NULL, point.x, point.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE) ;

	pos->second._szParent = m_szHostClient ;
	pos->second._rcCtrl.SetRect(point.x, point.y, point.x + size.cx, point.y + size.cy) ;
	return TRUE ;
}

void CCtlPosMgrST::HandleOnSize(UINT nType, int cx, int cy) 
{
	if (nType == SIZE_MINIMIZED)
		return ;
	m_szHostClient.cx = cx ;
	m_szHostClient.cy = cy ;

	int nCtrlCount = m_mapSA.size() ;
	if (nCtrlCount == 0)
		return ;

	HDWP hDwp = BeginDeferWindowPos(nCtrlCount) ;
	HWND hCtrl ;

	SAMAP::const_iterator pos ;
	for (pos = m_mapSA.begin() ; pos != m_mapSA.end() ; ++ pos)
	{
//		hCtrl = ::GetDlgItem(m_hWndHost, pos->first) ;
		hCtrl = pos->first ;
		if (hCtrl == NULL)
			continue ;

		STRETCH_ATTR const& sa = pos->second ;
		int x(sa._rcCtrl.left), y(sa._rcCtrl.top), ctrlcx(sa._rcCtrl.Width()), ctrlcy(sa._rcCtrl.Height()) ;
		if (!sa._bLock)		// 如果控件未被锁定，则不改变位置
		{
			if (sa._nMask & 1)
			{
				x += (cx - sa._szParent.cx) * sa._nPercentX / 100 ;
			}
			if (sa._nMask & 2)
			{
				y += (cy - sa._szParent.cy) * sa._nPercentY / 100 ;
			}
			if (sa._nMask & 4)
			{
				ctrlcx += (cx - sa._szParent.cx) * sa._nPercentCX / 100 ;
			}
			if (sa._nMask & 8)
			{
				ctrlcy += (cy - sa._szParent.cy) * sa._nPercentCY / 100 ;
			}
			hDwp = DeferWindowPos(hDwp, hCtrl, NULL, x, y, ctrlcx, ctrlcy, SWP_NOZORDER | SWP_NOACTIVATE) ;
		}
	}
	EndDeferWindowPos(hDwp) ;
}
