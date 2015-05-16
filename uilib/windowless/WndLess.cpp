#include "StdAfx.h"
#include "WndLess.h"
#include "common/utils/UIUtils.h"
#include <Imm.h>

#include "common/MemoryLeakChecker.h"

// #define _TRACE_WNDLESS_DRAW

CWndLessImp::CWndLessImp()
: 
m_dwStyle(0)
, m_uID(0)
, m_ptTopLeftInParent(0, 0)
, m_szClient(0, 0)
, m_pCenter(NULL)
, m_pParent(NULL)
, m_hPrevSiblingHwl(NULL)
, m_hNextSiblingHwl(NULL)
, m_hFirstChildHwl(NULL)
, m_font(NULL)
, m_display_listener(NULL)
, m_isCreated(FALSE)
{
#ifdef _DEBUG
	m_debugFrameColor = 0xffffff;
#endif
}

CWndLessImp::~CWndLessImp(void)
{
    if (m_pCenter != NULL)
    {
        TRACE(_T("You must destroy wndless before delete it!!")) ;
        ASSERT(FALSE) ;
    }
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implementations

BOOL CWndLessImp::Create(
					  DWORD dwStyle, 
					  int x, int y, int cx, int cy,
					  IWndLess * pParent,
					  UINT uID
					  ) 
{
	ASSERT(pParent != NULL) ;
	if (pParent == NULL)
		return FALSE ;

	if ((uID != (UINT)-1) && pParent->GetChild(uID) != NULL)
		return FALSE ;

	m_uID = uID ;
	m_pCenter = pParent->GetCenter() ;
	if (!m_pCenter->AddWndLess(m_wrapper))
		return FALSE ;

	m_pParent = pParent ;
	if (!m_pParent->AddChild(m_wrapper))
	{
		m_pCenter->DelWndLess(m_wrapper) ;
		return FALSE ;
	}

	m_dwStyle = dwStyle ;

	m_ptTopLeftInParent.SetPoint(x, y) ;
	m_szClient.SetSize(cx, cy) ;

	if (!m_wrapper->OnCreate(dwStyle, x, y, cx, cy, pParent))
	{
		m_pCenter->DelWndLess(m_wrapper) ;
		m_pParent->DelChild(m_wrapper) ;
		return FALSE ;
	}

    SIZE szOld = {0, 0} ;
    SIZE szNew = {m_szClient.cx, m_szClient.cy} ;
    m_wrapper->OnSized(szOld, szNew) ;

    m_isCreated = TRUE;

	if (m_dwStyle & WS_VISIBLE)
		m_wrapper->UpdateView() ;

	return TRUE ;
}

BOOL CWndLessImp::OnCreate(
						DWORD /*dwStyle*/, 
						int /*x*/, int /*y*/, int /*cx*/, int /*cy*/,
						IWndLess * /*pParent*/
						) 
{
	m_isCreated = TRUE;
	return TRUE ;
}

void CWndLessImp::Destroy() 
{
    ASSERT(m_isCreated);
    if (!m_isCreated)
        return;
	m_wrapper->OnDestroy() ;

	// 销毁所有孩子
	std::set<HWL>::iterator pos ;
	for (pos = m_setAllChildren.begin() ; pos != m_setAllChildren.end() ; )
	{
		(*(pos++))->Destroy() ;
	}

	CRect rcInParent ;
	m_wrapper->GetRectInParent(rcInParent) ;

	ASSERT(m_pCenter != NULL) ;
	if (m_pCenter == NULL)
	{
		TRACE(_T("Destroy WndLess Error: m_pCenter == NULL\n")) ;
		return ;
	}
	if (!m_pCenter->DelWndLess(m_wrapper))
	{
		TRACE(_T("Destroy WndLess Error: m_pCenter DelWndLess Failed\n")) ;
        m_pCenter = NULL ;
		return ;
	}
	m_pCenter = NULL;

	if (m_pParent == NULL)
	{
		TRACE(_T("Maybe Destroy WndLess Root or Error\n")) ;
		return ;
	}
	else
	{
		if (!m_pParent->DelChild(m_wrapper))
		{
			TRACE(_T("Destroy WndLess Error: m_pParent DelChild Failed\n")) ;
			return ;
		}
		m_hFirstChildHwl = NULL;
		m_hNextSiblingHwl = NULL;
		m_hPrevSiblingHwl = NULL;

		m_pParent->UpdateView(rcInParent) ;
		m_pParent = NULL;
	}
}

void CWndLessImp::OnDestroy() 
{
	// Do Nothing !!!!
}

IWndLessCenter * CWndLessImp::GetCenter() const 
{
	return m_pCenter ;
}

void CWndLessImp::SetCenter(IWndLessCenter *center)
{
	m_pCenter = center;
}

IWndLess * CWndLessImp::GetParent() const 
{
	return m_pParent ;
}

DWORD CWndLessImp::GetStyle() const 
{
	return m_dwStyle ;
}

UINT CWndLessImp::GetID() const 
{
	return m_uID ;
}

// Message Handler
BOOL CWndLessImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
	// Do Nothing !!!!
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
		{
			if (m_dwStyle & WS_DISABLED)
				bRes = TRUE ;
			else
				m_bLButtonDown = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			if (m_dwStyle & WS_DISABLED)
				bRes = TRUE ;
			else
			{
				if (m_bLButtonDown)
				{
					m_bLButtonDown = FALSE ;
					m_wrapper->SendMessage(WM_LBUTTONCLICK, wParam, lParam) ;
				}
			}
		}
		break ;

	case WM_RBUTTONDOWN :
		{
			if (m_dwStyle & WS_DISABLED)
				bRes = TRUE ;
			else
				m_bRButtonDown = TRUE ;
		}
		break ;

	case WM_RBUTTONUP :
		{
			if (m_dwStyle & WS_DISABLED)
				bRes = TRUE ;
			else
			{
				if (m_bRButtonDown)
				{
					m_bRButtonDown = FALSE ;
					m_wrapper->SendMessage(WM_RBUTTONCLICK, wParam, lParam) ;
				}
			}
		}
		break ;

	case WM_MOUSELEAVE :
		{
			m_bLButtonDown = FALSE ;
			m_bRButtonDown = FALSE ;
		}
		break ;
	}

	return bRes ;
}

// Operation
void CWndLessImp::_InternalOnDraw(HDC hDC, RECT const &rcUpdate) 
{
	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	RECT rcInsterset ;
	if (!m_wrapper->IsWndLessVisible() || !::IntersectRect(&rcInsterset, &rcInHost, &rcUpdate))
		return ;

#ifdef _TRACE_WNDLESS_DRAW
	TRACE(_T("--> %S::_InternalOnDraw [%p]\n"), typeid(*m_wrapper).name(), m_wrapper) ;
	CString s ;
	s.Format(_T("<-- %S::_InternalOnDraw [%p]"), typeid(*m_wrapper).name(), m_wrapper) ;
	hdutils::CPUPerformance cp(s) ;
#endif

	// 绘制自己
	m_wrapper->OnDraw(hDC, rcInsterset) ;

	// debug frame
#ifdef _DEBUG
if ( m_debugFrameColor != 0xffffff )
{
	HPEN hPen = CreatePen( PS_SOLID, 1, m_debugFrameColor ) ;
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen) ;	
	::MoveToEx(hDC, rcInHost.left, rcInHost.top, NULL) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.top) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.top) ;
	::DeleteObject(::SelectObject(hDC, hOldPen)) ;
}
else
{
#ifdef DEBUG_WL_LAYOUT
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)) ;
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen) ;	
	::MoveToEx(hDC, rcInHost.left, rcInHost.top, NULL) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.top) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.top) ;
	::DeleteObject(::SelectObject(hDC, hOldPen)) ;
#endif // end DEBUG_WL_LAYOUT
}
#endif // end _DEBUG

	// 绘制孩子
	m_wrapper->OnDrawChild(hDC, rcInsterset) ;
}

void CWndLessImp::OnDraw(HDC /*hDC*/, RECT const &/*rcUpdate*/) 
{
	// Do Nothing !!!!
}

void CWndLessImp::OnDrawChild(HDC hDC, RECT const &rcUpdate) 
{
	// Draw Children
// 	std::set<HWL>::reverse_iterator pos ;
// 	for (pos = m_setAllChildren.rbegin() ; pos != m_setAllChildren.rend() ; ++ pos)
// 	{
// 		(*pos)->_InternalOnDraw(hDC, rcUpdate) ;
// 	}

	HWL hChild = m_wrapper->GetFirstChildHwl() ;
	while (hChild != NULL)
	{
		hChild->_InternalOnDraw(hDC, rcUpdate) ;
		hChild = hChild->GetNextSiblingHwl() ;
	}
}

// 更新窗口子区域
// pRect坐标系为窗口
void CWndLessImp::UpdateView(RECT const* pRect /*= NULL*/) 
{
	if (m_wrapper == NULL)
		return ;

	ASSERT(m_wrapper->GetCenter() != NULL) ;
	if (m_wrapper->GetCenter() == NULL)
		return ;

	if (!m_wrapper->IsWndLessVisible() || !m_wrapper->IsWndLessShow())
		return ;

	HWND hHost = m_wrapper->GetCenter()->GetHost() ;

	CRect rcClient(0, 0, 0, 0) ;
	m_wrapper->GetRectInHost(rcClient) ;
	if (pRect == NULL)
	{
		::InvalidateRect(hHost, rcClient, FALSE) ;
	}
	else
	{
		CRect rcUpdate(pRect) ;
		rcUpdate.OffsetRect(rcClient.TopLeft()) ;
		::InvalidateRect(hHost, rcUpdate, FALSE) ;
	}
}

BOOL CWndLessImp::IsWndLessVisible() const 
{
	return (m_dwStyle & WS_VISIBLE) ;
}

BOOL CWndLessImp::IsWndLessShow() const
{
	HWL hParent = m_wrapper->GetParent() ;
	RECT rcParent ;
	RECT rcInsterset ;
	m_wrapper->GetRectInParent(&rcInsterset) ;
	while (hParent != NULL)
	{
		if (!hParent->IsWndLessVisible())
			return FALSE ;

		hParent->GetRectInClient(&rcParent) ;
		if (!::IntersectRect(&rcInsterset, &rcInsterset, &rcParent))
			return FALSE ;

		hParent->ClientToParent(&rcInsterset) ;
		hParent = hParent->GetParent() ;
	}

	return TRUE ;
}

BOOL CWndLessImp::IsWndLessEnable() const 
{
	return !(m_dwStyle & WS_DISABLED) ;
}

BOOL CWndLessImp::IsWndLessWindow() const
{
    return m_isCreated;
}

HWL CWndLessImp::SetFocus() 
{
	return m_wrapper->GetCenter()->C_SetFocus(m_wrapper) ;
}

HWL CWndLessImp::GetFocus() const 
{
	return m_wrapper->GetCenter()->C_GetFocus() ;
}

HRESULT CWndLessImp::RegisterDragDrop(LPDROPTARGET pDropTarget) 
{
	return  m_wrapper->GetCenter()->C_RegisterDragDrop(this, pDropTarget) ;
}


HRESULT CWndLessImp::RevokeDragDrop() 
{
	return  m_wrapper->GetCenter()->C_RevokeDragDrop(this) ; 
}

HWL CWndLessImp::SetCapture() 
{
	return m_wrapper->GetCenter()->C_SetCapture(m_wrapper) ;
}

HWL CWndLessImp::GetCapture() const 
{
	return m_wrapper->GetCenter()->C_GetCapture() ;
}

HWL CWndLessImp::ReleaseCapture() 
{
	return m_wrapper->GetCenter()->C_ReleaseCapture(m_wrapper) ;
}

UINT_PTR CWndLessImp::SetTimer(
							UINT_PTR nIDEvent,
							UINT nElapse,
							TIMERPROC lpTimerFunc
							) 
{
	return m_wrapper->GetCenter()->C_SetTimer(m_wrapper, nIDEvent, nElapse, lpTimerFunc) ;
}

BOOL CWndLessImp::KillTimer(
						 UINT nIDEvent
						 ) 
{
	return m_wrapper->GetCenter()->C_KillTimer(m_wrapper, nIDEvent) ;
}

BOOL CWndLessImp::EnableWindow(BOOL bEnable) 
{
	BOOL bRes = m_dwStyle & WS_DISABLED ;	// 返回值与MSDN保持一致

	if (bEnable)
	{
		if (m_dwStyle & WS_DISABLED)
		{
			m_dwStyle &= ~WS_DISABLED ;
			m_wrapper->GetCenter()->C_EnableWindow(m_wrapper, bEnable) ;
			m_wrapper->SendMessage(WM_ENABLE, bEnable, 0) ;
			m_wrapper->UpdateView() ;
		}
	}
	else
	{
		if (!(m_dwStyle & WS_DISABLED))
		{
			m_dwStyle |= WS_DISABLED ;
			m_wrapper->GetCenter()->C_EnableWindow(m_wrapper, bEnable) ;
			m_wrapper->SendMessage(WM_ENABLE, bEnable, 0) ;
			m_wrapper->UpdateView() ;
		}
	}

	return bRes ;
}

// 获得客户区在屏幕坐标系中的位置
void CWndLessImp::GetRectInScreen(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	m_pParent->GetRectInScreen(lpRect) ;
	lpRect->left += m_ptTopLeftInParent.x ;
	lpRect->top += m_ptTopLeftInParent.y ;
	lpRect->right = lpRect->left + m_szClient.cx ;
	lpRect->bottom = lpRect->top + m_szClient.cy ;
}

// 获得客户区在宿主坐标系中的位置
void CWndLessImp::GetRectInHost(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	if (m_pParent == NULL)
		return ;

	m_pParent->GetRectInHost(lpRect) ;
	lpRect->left += m_ptTopLeftInParent.x ;
	lpRect->top += m_ptTopLeftInParent.y ;
	lpRect->right = lpRect->left + m_szClient.cx ;
	lpRect->bottom = lpRect->top + m_szClient.cy ;
}

// 获得客户区在父窗口坐标系中的位置
void CWndLessImp::GetRectInParent(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	::SetRect(
		lpRect, 
		m_ptTopLeftInParent.x,
		m_ptTopLeftInParent.y,
		m_ptTopLeftInParent.x + m_szClient.cx,
		m_ptTopLeftInParent.y + m_szClient.cy
		) ;
}

// 获得客户区在客户区坐标系中的位置
void CWndLessImp::GetRectInClient(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	::SetRect(
		lpRect, 
		0,
		0,
		m_szClient.cx,
		m_szClient.cy
		) ;	
}

void CWndLessImp::ScreenToClient(LPPOINT lpPoint) const 
{
	ASSERT(lpPoint != NULL) ;
	if (lpPoint == NULL)
		return ;

	::ScreenToClient(GetCenter()->GetHost(), lpPoint) ;

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	lpPoint->x -= rcInHost.left ;
	lpPoint->y -= rcInHost.top ;
}

void CWndLessImp::ScreenToClient(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	HWND hHost = m_wrapper->GetCenter()->GetHost() ;
	::ScreenToClient(hHost, (LPPOINT)lpRect);
	::ScreenToClient(hHost, ((LPPOINT)lpRect)+1);

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	::OffsetRect(lpRect, -rcInHost.left, -rcInHost.top) ;
}

void CWndLessImp::ClientToScreen(LPPOINT lpPoint) const
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	lpPoint->x += rcInHost.left ;
	lpPoint->y += rcInHost.top ;
	::ClientToScreen(GetCenter()->GetHost(), lpPoint) ;
}

void CWndLessImp::ClientToScreen(LPRECT lpRect) const 
{
	if (lpRect == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	::OffsetRect(lpRect, rcInHost.left, rcInHost.top) ;

	HWND hHost = m_wrapper->GetCenter()->GetHost() ;
	::ClientToScreen(hHost, (LPPOINT)lpRect) ;
	::ClientToScreen(hHost, ((LPPOINT)lpRect) + 1) ;
}

void CWndLessImp::HostToClient(LPPOINT lpPoint) const
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	lpPoint->x -= rcInHost.left ;
	lpPoint->y -= rcInHost.top ;
}

void CWndLessImp::HostToClient(LPRECT lpRect) const 
{
	if (lpRect == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	::OffsetRect(lpRect, -rcInHost.left, -rcInHost.top) ;
}

void CWndLessImp::ClientToHost(LPPOINT lpPoint) const
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	lpPoint->x += rcInHost.left ;
	lpPoint->y += rcInHost.top ;
}

void CWndLessImp::ClientToHost(LPRECT lpRect) const
{
	if (lpRect == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	::OffsetRect(lpRect, rcInHost.left, rcInHost.top) ;
}

void CWndLessImp::ParentToClient(LPPOINT lpPoint) const 
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	lpPoint->x -= m_ptTopLeftInParent.x ;
	lpPoint->y -= m_ptTopLeftInParent.y ;
}

void CWndLessImp::ParentToClient(LPRECT lpRect) const 
{
	if (lpRect == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	::OffsetRect(lpRect, -m_ptTopLeftInParent.x, -m_ptTopLeftInParent.y) ;
}

void CWndLessImp::ClientToParent(LPPOINT lpPoint) const 
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	lpPoint->x += m_ptTopLeftInParent.x ;
	lpPoint->y += m_ptTopLeftInParent.y ;
}

void CWndLessImp::ClientToParent(LPRECT lpRect) const 
{
	if (lpRect == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	::OffsetRect(lpRect, m_ptTopLeftInParent.x, m_ptTopLeftInParent.y) ;
}

void CWndLessImp::GetPos(LPPOINT lpPoint) const 
{
	if (lpPoint == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	lpPoint->x = m_ptTopLeftInParent.x ;
	lpPoint->y = m_ptTopLeftInParent.y ;
}

void CWndLessImp::GetSize(LPSIZE lpSize) const 
{
	if (lpSize == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	lpSize->cx = m_szClient.cx ;
	lpSize->cy = m_szClient.cy ;
}

void CWndLessImp::SetWndLessPos(int x, int y, int cx, int cy, UINT uFlags) 
{
	CPoint ptOld(m_ptTopLeftInParent) ;
	CSize szOld(m_szClient) ;

	if (uFlags & SWLP_X_OFFSET)
    {
        m_ptTopLeftInParent.x += x ;
    }
    else if (uFlags & SWLP_X)
    {
        m_ptTopLeftInParent.x = x ;
    }

	if (uFlags & SWLP_Y_OFFSET)
    {
        m_ptTopLeftInParent.y += y ;
    }
    else if (uFlags & SWLP_Y)
    {
        m_ptTopLeftInParent.y = y ;
    }

	if (uFlags & SWLP_CX_OFFSET)
    {
        m_szClient.cx += cx ;
    }
    else if (uFlags & SWLP_CX)
    {
        m_szClient.cx = cx ;
    }

	if (uFlags & SWLP_CY_OFFSET)
    {
        m_szClient.cy += cy ;
    }
    else if (uFlags & SWLP_CY)
    {
        m_szClient.cy = cy ;
    }

	if (uFlags & SWLP_DEFER)
	{
		return;
	};

	// 大小和位置是否改变
	bool bChangeSize = !!(m_szClient != szOld) ;
	bool bChangePos = !!(m_ptTopLeftInParent != ptOld) ;
    bool bOldStatus = !!(m_dwStyle & WS_VISIBLE) ;

	// 新的显示状态
	bool bNewStatus = bOldStatus ;
	if (uFlags & SWLP_SHOW)
		bNewStatus = true ;
	else if (uFlags & SWLP_HIDE)
		bNewStatus = false ;

	int nIndex = 
		((m_dwStyle & WS_VISIBLE) ? 4 : 0)
		+ (bNewStatus ? 2 : 0)
		+ ((bChangeSize || bChangePos) ? 1 : 0) ;

	// 改变窗口“显示/隐藏”属性
	if (bNewStatus)
		m_dwStyle |= WS_VISIBLE ;
	else
		m_dwStyle &= ~WS_VISIBLE ;

	int arUpdate[8] = {0, 0, 1, 1, 2, 2, 0, 3} ;
	int nUpdate = arUpdate[nIndex] ;

	// 重绘
	if (!(uFlags & SWLP_NOREDRAW))
	{
		if (nUpdate & 0x2)		// update old pos
		{
			if (m_pParent != NULL)
			{
				CRect rcOld(ptOld, szOld) ;
				m_pParent->UpdateView(rcOld) ;
			}
		}
		if (nUpdate & 0x1)		// update new pos
		{
			m_wrapper->UpdateView(NULL) ;
		}
	}



	// 这里应该发送窗口位置改变和大小改变消息
	if (bChangePos)
	{
		m_wrapper->OnMoved(ptOld, m_ptTopLeftInParent) ;

        if ((uFlags & SWLP_NOTIFYCHANGE) && m_display_listener != NULL)
            m_display_listener->OnWndLessPositionChanged(this, ptOld, m_ptTopLeftInParent) ;
	}
	if (bChangeSize)
	{
		m_wrapper->OnSized(szOld, m_szClient) ;

        if ((uFlags & SWLP_NOTIFYCHANGE) && m_display_listener != NULL)
            m_display_listener->OnWndLessSizeChanged(this, szOld, m_szClient) ;
	}
    if (bNewStatus != bOldStatus)
    {
        m_wrapper->OnFlashed(bOldStatus, bNewStatus) ;

        if ((uFlags & SWLP_NOTIFYCHANGE) && m_display_listener != NULL)
            m_display_listener->OnWndLessShowHideChanged(this, bOldStatus, bNewStatus) ;
    }
}

void CWndLessImp::OnMoved(POINT const &/*ptOld*/, POINT const &/*ptNew*/) 
{
	return ;
}

void CWndLessImp::OnSized(SIZE const &/*szOld*/, SIZE const &/*szNew*/) 
{
	return ;
}

BOOL CWndLessImp::AddChild(HWL hChild) 
{
	std::set<HWL>::const_iterator pos = m_setAllChildren.find(hChild) ;
	if (pos != m_setAllChildren.end())
		return FALSE ;

	if (hChild->GetID() != (UINT)-1)
	{
		m_mapID2Child.insert(std::make_pair(hChild->GetID(), hChild)) ;
	}
	else
	{
		m_setNoIDChildren.insert(hChild) ;
	}

	m_setAllChildren.insert(hChild) ;

	HWL hLastChildHwl = m_wrapper->GetLastChildHwl() ;
	if (hLastChildHwl == NULL)
	{
		m_hFirstChildHwl = hChild ;
	}
	else
	{
		hLastChildHwl->SetNextSiblingHwl(hChild) ;
		hChild->SetPrevSiblingHwl(hLastChildHwl) ;
	}

	return TRUE ;
}

BOOL CWndLessImp::DelChild(HWL hChild) 
{
	std::set<HWL>::iterator pos = m_setAllChildren.find(hChild) ;
	if (pos == m_setAllChildren.end())
		return FALSE ;

	UINT uChildID = hChild->GetID() ;
	if (uChildID != -1)
	{
		m_mapID2Child.erase(hChild->GetID()) ;
	}
	else
	{
		m_setNoIDChildren.erase(hChild) ;
	}

	m_setAllChildren.erase(pos) ;

	HWL hPrev = hChild->GetPrevSiblingHwl() ;
	HWL hNext = hChild->GetNextSiblingHwl() ;
	if (hPrev == NULL)
	{
		m_hFirstChildHwl = hNext ;
	}
	else
	{
		hPrev->SetNextSiblingHwl(hNext) ;
	}

	if (hNext != NULL)
	{
		hNext->SetPrevSiblingHwl(hPrev) ;
	}

	return TRUE ;
}

HWL CWndLessImp::GetChild(UINT uID) const
{
	if (uID == (UINT)-1)
		return NULL ;

	std::map<UINT, HWL>::const_iterator pos = m_mapID2Child.find(uID) ;
	if (pos == m_mapID2Child.end())
		return NULL ;

	return pos->second ;
}

BOOL CWndLessImp::IsChild(HWL hChild, BOOL bGrandson) const 
{
	if (!m_wrapper->GetCenter()->IsWndLess(hChild))
		return FALSE ;

	std::set<HWL>::const_iterator pos = m_setAllChildren.find(hChild) ;
	if (pos == m_setAllChildren.end())
	{
		if (!bGrandson)
			return FALSE ;

		for (
			pos = m_setAllChildren.begin() ;
			pos != m_setAllChildren.end() ;
			++ pos
			)
		{
			if ((*pos)->IsChild(hChild, bGrandson))
				return TRUE ;
		}
	}
	else
		return TRUE ;

	return FALSE ;
}

HWL CWndLessImp::GetFirstChildHwl() const 
{
	return m_hFirstChildHwl ;
}

HWL CWndLessImp::GetLastChildHwl() const 
{
	HWL hChild = m_hFirstChildHwl ;
	HWL hTemp = NULL ;
	while (hChild != NULL)
	{
		hTemp = hChild->GetNextSiblingHwl() ;
		if (hTemp == NULL)
			return hChild ;

		hChild = hTemp ;
	}

	return hChild ;
}

void CWndLessImp::SetFirstChildHwl(HWL hFirstChild) 
{
	m_hFirstChildHwl = hFirstChild ;
}

HWL CWndLessImp::GetPrevSiblingHwl() const 
{
	return m_hPrevSiblingHwl ;
}

HWL CWndLessImp::GetNextSiblingHwl() const 
{
	return m_hNextSiblingHwl ;
}

void CWndLessImp::SetPrevSiblingHwl(HWL hSibling) 
{
	m_hPrevSiblingHwl = hSibling ;
}

void CWndLessImp::SetNextSiblingHwl(HWL hSibling) 
{
	m_hNextSiblingHwl = hSibling ;
}

HWL CWndLessImp::_InternalHitTest(POINT const &pt) 
{
	// 即使Disable，也能被测试到
// 	if ((m_dwStyle & WS_DISABLED) || !PtInWndLess(pt))
// 		return NULL ;
	if (!m_wrapper->IsWndLessVisible() || !m_wrapper->PtInWndLess(pt))
		return NULL ;

	// 窗口被禁用后，只能测试到自己，而不能测试到孩子
	HWL hRes = NULL ;
	if (!(m_dwStyle & WS_DISABLED))
		hRes = HitTestChild(pt) ;

	if (hRes == NULL)
	{
		if (!(m_dwStyle & WS_ILLUSION))
			hRes = m_wrapper ;
	}

	return hRes ;
}

HWL CWndLessImp::HitTestChild(POINT const &pt)
{
	HWL hRes = NULL ;

/*	std::set<HWL>::const_iterator pos ;
	for (
		pos = m_setAllChildren.begin() ;
		pos != m_setAllChildren.end() ;
		++ pos
		)
	{
		if (!(*pos)->IsWndLessVisible())
			continue ;

		hRes = (*pos)->_InternalHitTest(pt) ;
		if (hRes != NULL)
			break ;
	}*/

	HWL hChild = NULL ;
	for (hChild  = m_wrapper->GetLastChildHwl() ; hChild != NULL ; hChild = hChild->GetPrevSiblingHwl())
	{
		if (!hChild->IsWndLessVisible())
			continue ;

		hRes = hChild->_InternalHitTest(pt) ;
		if (hRes != NULL)
			break ;
	}

	return hRes ;
}

// pt是宿主坐标系
BOOL CWndLessImp::PtInWndLess(POINT const &pt) const 
{
	CRect rcClient ;
	m_wrapper->GetRectInHost(rcClient) ;
	return rcClient.PtInRect(pt) ;
}

BOOL CWndLessImp::SetWindowText(LPCTSTR lpszText) 
{
	m_sText = lpszText ;
	m_wrapper->UpdateView() ;
	return TRUE ;
}

int CWndLessImp::GetWindowText(CString &sText) const 
{
	sText = m_sText ;
	return sText.GetLength() ;
}

CString & CWndLessImp::GetWindowTextRef()
{
	return m_sText;
}
void CWndLessImp::SetWndLessStyle(DWORD dwStyle) 
{
	m_dwStyle = dwStyle ;
}

DWORD CWndLessImp::GetWndLessStyle() const 
{
	return m_dwStyle ;
}

HFONT CWndLessImp::GetFont() const 
{
    return m_font ;
}

void CWndLessImp::SetFont(HFONT font) 
{
    m_font = font ;
}

LRESULT CWndLessImp::SendMessage(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_pCenter)
	{
		hdutils::TrackUIOperation(m_pCenter->GetHost(), message, wParam, lParam);
	}

	
	LRESULT lResult = 0 ;
	m_wrapper->OnWndMsg(message, wParam, lParam, &lResult) ;


	return lResult ;
}

// IME
void CWndLessImp::UpdateImeCompositionWindowPos(int x, int y) const
{
	HWND hHost = m_wrapper->GetCenter()->GetHost() ;
	HIMC hImc = ImmGetContext(hHost);
	if (hImc)
	{
		POINT point = {x, y} ;
		m_wrapper->ClientToHost(&point) ;

		COMPOSITIONFORM ci = {0} ;
		ci.dwStyle = CFS_RECT ;
		ci.ptCurrentPos.x = point.x ;
		ci.ptCurrentPos.y = point.y ;
		m_wrapper->GetRectInHost(&ci.rcArea) ;
		ImmSetCompositionWindow(hImc, &ci) ;

		ImmReleaseContext(hHost, hImc) ;
	}
}

// 用Tab键移动焦点到下一个孩子
void CWndLessImp::TabFocusToNextChild(HWL hChild, BOOL bPrevious) 
{
	// 获取得到焦点的子窗口，或者是有子孙获得焦点的子窗口
	for (;;)
	{
		if (hChild == NULL)
			break ;

		if (!m_wrapper->IsChild(hChild, FALSE))
			hChild = hChild->GetParent() ;
		else
			break ;
	}

	if (hChild == NULL)
		return ;

	HWL hNext = GetNextTabChild(hChild, bPrevious) ;
	if (hNext != hChild)
		hNext->SetFocus() ;
}

// 获得下一个Tab键移到的孩子
HWL CWndLessImp::GetNextTabChild(HWL hChild, BOOL bPrevious) 
{
	HWL hNext = NULL ;
	HWL hParent = hChild->GetParent() ;
	if (hParent == NULL)
		return hChild ;

	if (bPrevious)
	{
		hNext = hChild->GetPrevSiblingHwl() ;
		while (hNext != hChild)
		{
			if (hNext == NULL)
			{
				hNext = hParent->GetLastChildHwl() ;
				continue ;
			}

			if ((hNext->GetStyle() & (WS_TABSTOP | WS_VISIBLE)) == (WS_TABSTOP | WS_VISIBLE))
			{
				break ;
			}
			else
			{
				hNext = hNext->GetPrevSiblingHwl() ;
			}
		}

	}
	else
	{
		hNext = hChild->GetNextSiblingHwl() ;
		while (hNext != hChild)
		{
			if (hNext == NULL)
			{
				hNext = hParent->GetFirstChildHwl() ;
				continue ;
			}

			if ((hNext->GetStyle() & (WS_TABSTOP | WS_VISIBLE)) == (WS_TABSTOP | WS_VISIBLE))
			{
				break ;
			}
			else
			{
				hNext = hNext->GetNextSiblingHwl() ;
			}
		}
	}

	return hNext ;
}

HWL CWndLessImp::GetTopLevelParent() const 
{
	HWL hParent = m_wrapper->GetParent() ;
	HWL hTemp = NULL ;
	while (hParent != NULL)
	{
		hTemp = hParent->GetParent() ;
		if (hTemp == NULL)
			break ;

		hParent = hTemp ;
	}

	return hParent ;
}

BOOL CWndLessImp::AddTooltip(LPCTSTR lpszTip) 
{
	return m_wrapper->GetCenter()->AddTooltipEx(m_wrapper, lpszTip) ;
}

BOOL CWndLessImp::UpdateTooltip(LPCTSTR lpszTip) 
{
	return m_wrapper->GetCenter()->UpdateTooltipEx(m_wrapper, lpszTip) ;
}

BOOL CWndLessImp::ShowTooltip(BOOL bShow) 
{
	return m_wrapper->GetCenter()->ShowTooltipEx(m_wrapper, bShow) ;
}

BOOL CWndLessImp::RemoveTooltip() 
{
	return m_wrapper->GetCenter()->RemoveTooltipEx(m_wrapper) ;
}

BOOL CWndLessImp::SetZorder(HWL hInsertAfter) 
{
	HWL hParent = m_wrapper->GetParent() ;
	if (hParent == NULL)
		return FALSE ;

	// hInsertAfter为NULL时，即HWL_TOP
	if (hInsertAfter == HWL_BOTTOM)
	{
		hInsertAfter = hParent->GetLastChildHwl() ;
	}

	if (hInsertAfter == m_wrapper)
		return FALSE ;

	if (hParent->IsChild(hInsertAfter, FALSE) || hInsertAfter == HWL_TOP)
	{
		// 先从链表中脱离出来
		{
			HWL hOldPrevSibling = GetPrevSiblingHwl() ;
			if (hInsertAfter == hOldPrevSibling)
				return TRUE ;

			HWL hOldNextSibling = GetNextSiblingHwl() ;
			if (hOldPrevSibling != NULL)
			{
				hOldPrevSibling->SetNextSiblingHwl(hOldNextSibling) ;
			}
			else
			{
				hParent->SetFirstChildHwl(hOldNextSibling) ;
			}

			if (hOldNextSibling != NULL)
				hOldNextSibling->SetPrevSiblingHwl(hOldPrevSibling) ;
		}

		// 再插到链表中
		{
			HWL hNextSibling = NULL ;
			if (hInsertAfter == HWL_TOP)
			{
				hNextSibling = hParent->GetFirstChildHwl() ;
				hParent->SetFirstChildHwl(m_wrapper) ;
			}
			else
			{
				hNextSibling = hInsertAfter->GetNextSiblingHwl() ;
				hInsertAfter->SetNextSiblingHwl(m_wrapper) ;
			}

			m_wrapper->SetPrevSiblingHwl(hInsertAfter) ;
			m_wrapper->SetNextSiblingHwl(hNextSibling) ;

			if (hNextSibling != NULL)
				hNextSibling->SetPrevSiblingHwl(m_wrapper) ;
		}
	}

	return FALSE ;
}


template<>
UILIB_EXPORT WndLessImpBase *CreateDefaultImp<WndLessImpBase>(IWndLess *wrapper)
{
	CWndLessImp* ret = new CWndLessImp();
	ret->SetWrapper(wrapper);
	return ret;
};
