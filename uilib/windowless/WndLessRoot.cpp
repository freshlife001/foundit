#include "StdAfx.h"
#include "WndLessRoot.h"
#include "common/MemoryLeakChecker.h"

CWndLessRootImp::CWndLessRootImp()
: 
m_hHost(NULL)
, m_hImc(NULL)
, m_hTooltip(NULL)
, m_hFocusChild(NULL)
, m_hCaptureChild(NULL)
, m_hHotChild(NULL)
, m_bIsTrackMouse(FALSE)
, m_enWndSize(RESTORED)
, m_timerid_alloter(1, 0xffffffff)
, m_register_drag_drop(FALSE)
, m_drop_target(NULL)
{
    m_drop_target = new RootDropTarget(this) ;
    m_drop_target->AddRef() ;
}

CWndLessRootImp::~CWndLessRootImp(void)
{
    m_drop_target->Release() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLessCenter Implementations
// With Host
BOOL CWndLessRootImp::CreateEx(HWND hHost) 
{
	if (m_hHost != NULL)
		return FALSE ;

	if (!::IsWindow(hHost))
		return FALSE ;

	m_hHost = hHost ;

	RECT rcClient ;
	::GetClientRect(m_hHost, &rcClient) ;
	::ClientToScreen(m_hHost, (LPPOINT)&rcClient) ;
	::ClientToScreen(m_hHost, ((LPPOINT)&rcClient) + 1) ;

	// 创建顶层无句柄窗口(不调用IWndLess的创建方法)
	{
		if (!m_wrapper->AddWndLess(m_wrapper))
			return FALSE ;

		
		DWORD dwStyle = ::GetWindowLong(hHost, GWL_STYLE) ;
		dwStyle &= ~WS_CHILD;
		dwStyle |= WS_VISIBLE ;
		m_wrapper->SetWndLessStyle(dwStyle);

		int cx = rcClient.right - rcClient.left;
		int cy = rcClient.bottom - rcClient.top;
		m_wrapper->SetWndLessPos(0, 0, cx, cy, SWLP_X|SWLP_Y|SWLP_CX|SWLP_CY|SWLP_DEFER);
		m_wrapper->SetCenter(m_wrapper);
		m_wrapper->OnCreate(dwStyle, 0, 0, cx, cy, NULL) ;

		SIZE szOld = {0, 0} ;
		SIZE szNew = {cx, cy} ;
		m_wrapper->OnSized(szOld, szNew) ;
	}



	m_wrapper->DisableIme() ;
	m_wrapper->EnableIme() ;

	return TRUE ;
}

HWND CWndLessRootImp::GetHost() const 
{
	return m_hHost ;
}

// Manage
BOOL CWndLessRootImp::AddWndLess(HWL hChild) 
{
	std::set<HWL>::iterator pos = m_setChildTree.find(hChild) ;
	if (pos != m_setChildTree.end())
		return FALSE ;

	m_setChildTree.insert(hChild) ;
	return TRUE ;
}

BOOL CWndLessRootImp::DelWndLess(HWL hChild) 
{
	std::set<HWL>::iterator pos = m_setChildTree.find(hChild) ;
	if (pos == m_setChildTree.end())
        return FALSE ;

    // 清除拖拽
    C_RevokeDragDrop(hChild) ;

	if (m_hFocusChild == hChild)
		m_hFocusChild = NULL ;
	if (m_hHotChild == hChild)
		m_hHotChild = NULL ;
	if (m_hCaptureChild == hChild)
	{
		if (::GetCapture() == m_hHost)
			::ReleaseCapture() ;
		m_hCaptureChild = NULL ;
	}

	m_setChildTree.erase(pos) ;

	// 清除tooltip
	m_mapChildTooltips.erase(hChild) ;

	// 清除timer
    std::map<WndLessTimerId, UINT_PTR>::iterator iter ;
    for (iter = m_timerid_map_wndless2win.begin() ; iter != m_timerid_map_wndless2win.end() ; )
    {
        if (iter->first.m_wndless == hChild)
        {
            ::KillTimer(m_hHost, iter->second) ;
            m_timerid_alloter.FreeId(iter->second) ;

            m_timerid_map_win2wndless.erase(iter->second) ;
            m_timerid_map_wndless2win.erase(iter ++) ;
        }
        else
        {
            ++ iter ;
        }
    }

	return TRUE ;
}

BOOL CWndLessRootImp::IsWndLess(HWL hChild) const 
{
	if (hChild == NULL)
		return FALSE ;

	return m_setChildTree.find(hChild) != m_setChildTree.end() ;
}

LRESULT CWndLessRootImp::SendMessageEx(
								 HWL hChild, 
								 UINT message, 
								 WPARAM wParam, 
								 LPARAM lParam
								 ) 
{
	LRESULT lResult = 0 ;
	if (!m_wrapper->IsWndLess(hChild))
		return lResult ;

	hChild->OnWndMsg(message, wParam, lParam, &lResult) ;
	return lResult ;
}

HWL CWndLessRootImp::C_SetFocus(HWL hChild) 
{
	// TRACE(_T("C_SetFocus: 0x%08p\n"), hChild) ;
	ASSERT(::IsWindow(m_hHost)) ;
	if (!::IsWindow(m_hHost))
		return NULL ;
	if (!m_wrapper->IsWndLess(hChild))
		return NULL ;

	if (::GetFocus() == m_hHost && m_hFocusChild == hChild)
		return NULL ;

	LRESULT lResult ;
	if (m_hFocusChild != NULL)
	{
		// 让失去焦点消息传上去
		PollMessage(m_hFocusChild, WM_KILLFOCUS, (WPARAM)hChild, 0, &lResult) ;
		//m_hFocusChild->OnWndMsg(WM_KILLFOCUS, (WPARAM)hChild, 0, &lResult) ;
	}

	HWL hPrevFocusChild = m_hFocusChild ;
	m_hFocusChild = NULL ;

	if (hChild != NULL && m_wrapper->IsWndLess(hChild))
	{
		// 输入法的启用和禁用
		LRESULT l = m_wrapper->SendMessageEx(hChild, WM_GETDLGCODE, 0, 0) ;
		if (l & DLGC_WANTCHARS)
		{
			m_wrapper->EnableIme() ;
		}
		else
		{
			m_wrapper->DisableIme() ;
		}

		//hChild->OnWndMsg(WM_SETFOCUS, (WPARAM)hPrevFocusChild, 0, &lResult) ;
		//m_hFocusChild = hChild ;

		if (::GetFocus() == m_hHost)
		{
			hChild->OnWndMsg(WM_SETFOCUS, (WPARAM)hPrevFocusChild, 0, &lResult) ;
			m_hFocusChild = hChild ;
		}
		else
		{
			m_hFocusChild = hChild ;
			::SetFocus(m_hHost) ;	// 这里会让m_hFocusChild获得焦点
			::SetActiveWindow(m_hHost) ;
		}
	}

	return hPrevFocusChild ;
}

HWL CWndLessRootImp::C_GetFocus() const 
{
	return m_hFocusChild ;
}

HWL CWndLessRootImp::C_SetCapture(HWL hChild) 
{
	ASSERT(::IsWindow(m_hHost)) ;
	if (!::IsWindow(m_hHost))
		return NULL ;
	if (!m_wrapper->IsWndLess(hChild))
		return NULL ;
	if (m_hCaptureChild == hChild)
		return NULL ;

	if (::GetCapture() != m_hHost)
	{
		::SetCapture(m_hHost) ;
	}

	LRESULT lResult ;
	if (m_hCaptureChild != NULL)
		m_hCaptureChild->OnWndMsg(WM_CAPTURECHANGED, 0, (LPARAM)hChild, &lResult) ;

	HWL hPrevCapture = m_hCaptureChild ;
	m_hCaptureChild = hChild ;
	return hPrevCapture ;
}

HWL CWndLessRootImp::C_GetCapture() const 
{
	return m_hCaptureChild ;
}

HWL CWndLessRootImp::C_ReleaseCapture(HWL hChild) 
{
	if (!m_wrapper->IsWndLess(hChild))
		return NULL ;

	if (m_hCaptureChild != hChild)
		return NULL ;

	if (::GetCapture() == m_hHost)
	{
		::ReleaseCapture() ;
	}

	// 在处理释放消息时清空m_hCaptureChild

	return hChild ;
}

UINT_PTR CWndLessRootImp::C_SetTimer(
								 HWL hChild,
								 UINT_PTR nIDEvent,
								 UINT nElapse,
								 TIMERPROC lpTimerFunc
								 ) 
{
	ASSERT(::IsWindow(m_hHost)) ;
	if (!::IsWindow(m_hHost))
		return (UINT_PTR)0 ;

    WndLessTimerId wndless_timer_id(hChild, nIDEvent) ;
    std::map<WndLessTimerId, UINT_PTR>::iterator iter = m_timerid_map_wndless2win.find(wndless_timer_id) ;

    if (iter != m_timerid_map_wndless2win.end())
    {
        // 已存在时重置定时器
        ::SetTimer(m_hHost, iter->second, nElapse, lpTimerFunc) ;
        return iter->second ;
    }
    else
    {
        // 不存在则创建新定时器
        UINT_PTR win_timer_id = 0 ;
        if (!m_timerid_alloter.AllocateId(win_timer_id))
            return 0 ;

        m_timerid_map_wndless2win.insert(std::make_pair(wndless_timer_id, win_timer_id)) ;
        m_timerid_map_win2wndless.insert(std::make_pair(win_timer_id, wndless_timer_id)) ;
        ::SetTimer(m_hHost, win_timer_id, nElapse, lpTimerFunc) ;
        return win_timer_id ;
    }
}

BOOL CWndLessRootImp::C_KillTimer(
							  HWL hChild,
							  UINT nIDEvent
							  ) 
{
	ASSERT(::IsWindow(m_hHost)) ;
	if (!::IsWindow(m_hHost))
		return FALSE ;

    WndLessTimerId wndless_timer_id(hChild, nIDEvent) ;
    std::map<WndLessTimerId, UINT_PTR>::iterator iter = m_timerid_map_wndless2win.find(wndless_timer_id) ;
    if (iter == m_timerid_map_wndless2win.end())
        return FALSE ;

    if (!::KillTimer(m_hHost, iter->second))
        return FALSE ;

    m_timerid_alloter.FreeId(iter->second) ;
    m_timerid_map_win2wndless.erase(iter->second) ;
    m_timerid_map_wndless2win.erase(iter) ;

    return TRUE ;
}

void CWndLessRootImp::C_EnableWindow(
								  HWL hChild,
								  BOOL bEnable
								  ) 
{
	ASSERT(::IsWindow(m_hHost)) ;
	if (!::IsWindow(m_hHost))
		return ;

	if (!m_wrapper->IsWndLess(hChild))
		return ;

	if (bEnable)
	{
	}
	else
	{
		if (m_hCaptureChild == hChild)
			m_wrapper->C_ReleaseCapture(hChild) ;
		if (m_hFocusChild == hChild)
			m_hFocusChild = NULL ;
	}
}

BOOL CWndLessRootImp::PreDispatchMessage(
									 UINT message, 
									 WPARAM wParam, 
									 LPARAM lParam, 
									 LRESULT * pResult
									 ) 
{
	switch (message)
	{
	case WM_DESTROY :
		{
            if (m_register_drag_drop)
            {
                m_wl2droptarget.clear() ;
                ::RevokeDragDrop(m_wrapper->GetHost()) ;
                m_register_drag_drop = FALSE ;
            }

			Destroy() ;
			return TRUE ;
		}
		break ;

	case WM_NOTIFY :
		{
			// Tooltip Notify Message
			if (wParam == CTRLID_TOOLTIP)
			{
				ProcessTooltipNotify(wParam, lParam, pResult) ;
			}
		}
		break ;

	case WM_SIZE :
		{
			if (wParam != SIZE_MINIMIZED)
			{
				int cx, cy ;
				cx = LOWORD(lParam) ;
				cy = HIWORD(lParam) ;
				if (m_hTooltip != NULL)
				{
					TOOLINFO ti = {0} ;
					ti.cbSize = sizeof(ti) ;
					CPoint point;
					m_wrapper->GetPos(&point);
					SetRect(&(ti.rect),
						point.x,
						point.y,
						point.x + cx,
						point.y + cy
						) ;
					ti.uId = CTRLID_TOOLTIP ;
					ti.hwnd = m_wrapper->GetHost();
					::SendMessage(m_hTooltip, TTM_NEWTOOLRECT, 0, (LPARAM)&ti) ;
				}

				if (wParam == SIZE_RESTORED)
				{
					if (m_enWndSize != RESTORED)
					{
						WNDSIZE enPrev = m_enWndSize ;
						m_enWndSize = RESTORED ;
						m_wrapper->OnRestored(enPrev) ;
					}
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					if (m_enWndSize != MAXIMIZED)
					{
						WNDSIZE enPrev = m_enWndSize ;
						m_enWndSize = MAXIMIZED ;
						m_wrapper->OnMaximized(enPrev) ;
					}
				}

				m_wrapper->SetWndLessPos(0, 0, cx, cy, SWLP_CX | SWLP_CY) ;
			}
			else
			{
				if (m_enWndSize != MINIMIZED)
				{
					WNDSIZE enPrev = m_enWndSize ;
					m_enWndSize = MINIMIZED ;
					m_wrapper->OnMinimized(enPrev) ;
				}
			}
		}
		break ;
	}

	return FALSE ;
}

BOOL CWndLessRootImp::DispatchMessage(
								  UINT message, 
								  WPARAM wParam, 
								  LPARAM lParam, 
								  LRESULT * pResult
								  ) 
{
	//DWORD dwBegin = GetTickCount() ;

	BOOL bRes = FALSE ;
	*pResult = 0 ;

	if (m_wrapper->PreDispatchMessage(message, wParam, lParam, pResult))
		return TRUE ;

	switch (message)
	{
	case WM_CAPTURECHANGED :
		{
			// 外部抢夺了鼠标控制权
			if (m_hCaptureChild != NULL)
			{
				bRes = m_hCaptureChild->OnWndMsg(WM_CAPTURECHANGED, 0, -1, pResult) ;

				CPoint ptCursor ;
				GetCursorPos(&ptCursor) ;
				CRect rcWnd ;
				m_hCaptureChild->GetRectInScreen(rcWnd) ;
				if (!rcWnd.PtInRect(ptCursor))
				{
					TrackTooltip(m_hCaptureChild, FALSE, message, wParam, lParam) ;
					m_hCaptureChild->OnWndMsg(WM_MOUSELEAVE, WPARAM(m_hCaptureChild), 0, pResult) ;
				}

				m_hCaptureChild = NULL ;
				return bRes ;
			}
		}
		break ;

	case WM_SETFOCUS :
		{
			// 宿主窗口获得焦点时，焦点子窗口也获得焦点
			//TRACE(_T("WM_SETFOCUS: 0x%08p\n"), m_hFocusChild) ;
			if (m_hFocusChild != NULL)
			{
				bRes = m_hFocusChild->OnWndMsg(WM_SETFOCUS, (WPARAM)-1, 0, pResult) ;
				return bRes ;
			}
		}
		break ;

	case WM_KILLFOCUS :
		{
			//TRACE(_T("Host WM_KILLFOCUS: 0x%08p\n"), m_hFocusChild) ;
			// 外部抢夺了焦点
			if (m_hFocusChild != NULL)
			{
				//bRes = m_hFocusChild->OnWndMsg(WM_KILLFOCUS, (WPARAM)-1, 0, pResult) ;
				bRes = PollMessage(m_hFocusChild, WM_KILLFOCUS, (WPARAM)-1, 0, pResult) ;
				return bRes ;
			}
		}
		break ;

	case WM_PAINT :
		{
			PAINTSTRUCT ps ;
			HDC hDC = ::BeginPaint(m_hHost, &ps) ;

// 			LARGE_INTEGER liFrequency ;
// 			QueryPerformanceFrequency(&liFrequency) ;
// 
// 			LARGE_INTEGER liCounter ;
// 			QueryPerformanceCounter(&liCounter) ;
// 
// 			TRACE(
// 				_T(">>>>Enter Paint[%p]: left=%d right=%d top=%d bottom=%d\n"),
// 				m_wrapper, ps.rcPaint.left, ps.rcPaint.right, ps.rcPaint.top, ps.rcPaint.bottom
// 				) ;

			m_wrapper->_InternalOnDraw(hDC, ps.rcPaint) ;

// 			LARGE_INTEGER liCounter2 ;
// 			QueryPerformanceCounter(&liCounter2) ;
// 			DWORD dwCost = liCounter2.LowPart - liCounter.LowPart ;
// 			TRACE(_T("<<<<Leave Paint[%p], cost [%u i] %u ms\n"), m_wrapper, dwCost, (dwCost) * 1000 / liFrequency.LowPart) ;

			::EndPaint(m_hHost, &ps) ;

			*pResult = 0 ;
			return TRUE ;
		}
		break ;

	case WM_TIMER :
		{
            std::map<UINT_PTR, WndLessTimerId>::iterator iter = m_timerid_map_win2wndless.find((UINT_PTR)wParam) ;
            if (iter != m_timerid_map_win2wndless.end())
            {
                bRes = iter->second.m_wndless->OnWndMsg(WM_TIMER, iter->second.m_id, lParam, pResult) ;
                return bRes ;
            }
		}
		break ;

	case WM_INPUTLANGCHANGEREQUEST :
		{
			if (m_hFocusChild == NULL)
				return TRUE ;

			//LRESULT l = SendMessage(m_hFocusChild, WM_GETDLGCODE, 0, 0) ;
			*pResult = ::DefWindowProc(m_hHost, WM_INPUTLANGCHANGEREQUEST, wParam, lParam) ;
// 			if (!(l & DLGC_WANTCHARS))
// 			{
// 				ImmDisableIME(-1) ;
// 			}
			return TRUE ;
		}
		break ;

// 	case WM_IME_SETCONTEXT :
// 		{
// 			*pResult = ::DefWindowProc(m_hHost, WM_INPUTLANGCHANGEREQUEST, wParam, lParam) ;
// 			return TRUE ;
// 		}
// 		break ;
	}

	// 键盘消息发给焦点窗口
	if ((message >= WM_KEYFIRST && message <= WM_KEYLAST))
	{
		if (m_hFocusChild != NULL)
		{
			//bRes = m_hFocusChild->OnWndMsg(message, wParam, lParam, pResult) ;
			bRes = PollMessage(m_hFocusChild, message, wParam, lParam, pResult) ;
			return bRes ;
		}
    }
    // WM_MOUSEWHEEL消息先发给焦点窗口，如果得不到响应，发给鼠标下面的窗口
    else if (message == WM_MOUSEWHEEL)
    {
        if (m_hFocusChild != NULL)
        {
            bRes = PollMessage(m_hFocusChild, message, wParam, lParam, pResult) ;
        }

        if (!bRes && m_hHotChild != NULL)
        {
            bRes = PollMessage(m_hHotChild, message, wParam, lParam, pResult) ;
        }

        return bRes ;
    }
	else if (
		message >= WM_MOUSEFIRST && message <= WM_MOUSELAST
		)
	{
		if (m_hCaptureChild != NULL)
		{
			//bRes = m_hCaptureChild->OnWndMsg(message, wParam, lParam, pResult) ;
			bRes = PollMessage(m_hCaptureChild, message, wParam, lParam, pResult) ;
			return bRes ;
		}
		else
		{
			CPoint pt(lParam) ;
			HWL hCurHotChild = _InternalHitTest(pt) ;	// 这里获取
			if (message == WM_MOUSEMOVE /*&& m_hCaptureChild == NULL*/)
			{
				if (!m_bIsTrackMouse)
				{
					TRACKMOUSEEVENT tme ;
					tme.cbSize = sizeof(tme) ;
					tme.hwndTrack = m_wrapper->GetHost() ;
					tme.dwFlags = TME_LEAVE ;
					tme.dwHoverTime = HOVER_DEFAULT ;
					m_bIsTrackMouse = _TrackMouseEvent(&tme) ;
				}
			}

			if (m_hHotChild != hCurHotChild)
			{
				if (m_hHotChild != NULL)
				{
					//TRACE(_T("MouseLeave: 0x%08p\n"), m_hHotChild) ;
					TrackTooltip(m_hHotChild, FALSE, message, wParam, lParam) ;
					//m_hHotChild->OnWndMsg(WM_MOUSELEAVE, (WPARAM)hCurHotChild, 0, pResult) ;
					PollMessage(m_hHotChild, WM_MOUSELEAVE, (WPARAM)m_hHotChild, (LPARAM)hCurHotChild, pResult) ;
				}
				if (hCurHotChild != NULL)
				{
					//TRACE(_T("MouseEnter: 0x%08p\n"), hCurHotChild) ;
					//hCurHotChild->OnWndMsg(message, wParam, lParam, pResult) ;
					TrackTooltip(hCurHotChild, TRUE, message, wParam, lParam) ;
					PollMessage(hCurHotChild, WM_MOUSEHOVER, WPARAM(m_hHotChild), LPARAM(hCurHotChild), pResult) ;
					PollMessage(hCurHotChild, message, wParam, lParam, pResult) ;
				}
				m_hHotChild = hCurHotChild ;
				return TRUE ;
			}
			else
			{
				if (m_hHotChild != NULL) 
				{
					//bRes = m_hHotChild->OnWndMsg(message, wParam, lParam, pResult) ;
					bRes = PollMessage(m_hHotChild, message, wParam, lParam, pResult) ;
				}
				return bRes ;
			}

/*			else
			{
				TRACE(_T("Mouse Msg[%d]: 0x%08p\n"), message, m_hHotChild) ;
				if (m_hHotChild != NULL)
				{
					bRes = m_hHotChild->OnWndMsg(message, wParam, lParam, pResult) ;
					return bRes ;
				}
			}*/
		}
	}
	else if (message == WM_SETCURSOR)
	{
		// 当鼠标控制权被捕获后，不再发送WM_SETCURSOR消息请求鼠标形状
		// 模仿操作系统
		if (m_hCaptureChild == NULL)
		{
			//WORD wHitTest = LOWORD(lParam) ;
			//WORD wMM = HIWORD(lParam) ;

			CPoint pt ;
			GetCursorPos(&pt) ;
			::ScreenToClient(m_hHost, &pt) ;
			HWL hCurHotChild = m_wrapper->_InternalHitTest(pt) ;	// 这里获取
			//TRACE(_T("[%d]HotChild = 0x%p\n"), GetTickCount(), hCurHotChild) ;
			if (hCurHotChild != NULL)
			{
				// 往上传
				bRes = PollMessage(hCurHotChild, WM_SETCURSOR, wParam, lParam, pResult) ;
				//bRes = hCurHotChild->OnWndMsg(WM_SETCURSOR, wParam, lParam, pResult) ;
				return bRes ;
			}
		}
	}
	else if (message == WM_GETDLGCODE)
	{
		if (m_hFocusChild != NULL)
		{
			bRes = m_hFocusChild->OnWndMsg(WM_GETDLGCODE, wParam, lParam, pResult) ;
			return bRes ;
		}
	}
	else if (message == WM_MOUSELEAVE)
	{
		//TRACE(_T("Host MouseLeave: 0x%08p\n"), m_hHotChild) ;
		m_bIsTrackMouse = FALSE ;
		if (m_hHotChild != NULL)
		{
			TrackTooltip(m_hHotChild, FALSE, WM_MOUSELEAVE, wParam, lParam) ;
			bRes = PollMessage(m_hHotChild, WM_MOUSELEAVE, WPARAM(m_hHotChild), 0, pResult) ;
			m_hHotChild = NULL ;
			return bRes ;
		}
	}

	bRes = m_wrapper->OnWndMsg(message, wParam, lParam, pResult) ;

	//TRACE(_T("Dispatch Message cost %d ms\n"), GetTickCount() - dwBegin) ;

	return bRes ;
}

void CWndLessRootImp::EnableIme() 
{
	HWND hHost = m_wrapper->GetHost() ;
	if (m_hImc)
	{
		// Associate the input context with testing window
		ImmAssociateContext(hHost, m_hImc);
		m_hImc = NULL;
	}
}

void CWndLessRootImp::DisableIme() 
{
	if (m_hImc == NULL)
	{
		HWND hHost = m_wrapper->GetHost() ;
		m_hImc = ImmGetContext(hHost);

		// Remove association the testing 
		if (m_hImc)
			ImmAssociateContext(m_wrapper->GetHost(), NULL);

		// Release input context
		ImmReleaseContext(m_wrapper->GetHost(), m_hImc);
	}
}

void CWndLessRootImp::EnableTooltips(BOOL bEnable) 
{
	if (bEnable)
	{
		if (m_hTooltip == NULL)
		{
			HWND hHost = m_wrapper->GetHost() ;
			m_hTooltip = ::CreateWindowEx(
				WS_EX_TOPMOST | WS_EX_TRANSPARENT, 
				TOOLTIPS_CLASS, 
				NULL,
				WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				hHost,
				NULL,
				NULL,
				NULL
				) ;
			::SendMessage(m_hTooltip, WM_SETFONT, (WPARAM)::SendMessage(hHost, WM_GETFONT, 0, 0), 0) ;
			::SendMessage(m_hTooltip, TTM_SETMAXTIPWIDTH, 0, 200) ;

			TOOLINFO ti;
			unsigned int uid = CTRLID_TOOLTIP;       // for ti initialization

			/* GET COORDINATES OF THE MAIN CLIENT AREA */
			CRect rcInHost ;
			m_wrapper->GetRectInHost(rcInHost) ;

			/* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = /*TTF_SUBCLASS | */TTF_TRANSPARENT ;
			ti.hwnd = hHost;
			ti.hinst = NULL;
			ti.uId = uid;
			ti.lpszText = LPSTR_TEXTCALLBACK;
			// ToolTip control will cover the whole window
			ti.rect.left = rcInHost.left;    
			ti.rect.top = rcInHost.top;
			ti.rect.right = rcInHost.right;
			ti.rect.bottom = rcInHost.bottom;

			/* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
			::SendMessage(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
		}
	}
	else
	{
		::DestroyWindow(m_hTooltip) ;
		m_hTooltip = NULL ;
		m_mapChildTooltips.clear() ;
	}
}

BOOL CWndLessRootImp::AddTooltipEx(HWL hChild, LPCTSTR lpszTip) 
{
	if (m_hTooltip == NULL)
		return FALSE ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(hChild) ;
	if (pos != m_mapChildTooltips.end())
	{
		if (lpszTip != LPSTR_TEXTCALLBACK && lpszTip != LPSTR_TEXTCALLBACK_OWNER)
		{
			pos->second.type = 0 ;		// 0代表不是回调，是固定提示文本
			pos->second.tip_text = lpszTip ;
		}
		else
		{
			pos->second.type = (int)lpszTip ;
			pos->second.tip_text.Empty() ;
		}
		return TRUE ;
	}

	TOOLTIPINFO tti ;
	if (lpszTip != LPSTR_TEXTCALLBACK && lpszTip != LPSTR_TEXTCALLBACK_OWNER)
	{
		tti.type = 0 ;
		tti.tip_text = lpszTip ;
	}
	else
	{
		tti.type = (int)lpszTip ;
		tti.tip_text.Empty() ;
	}
	m_mapChildTooltips.insert(std::make_pair(hChild, tti)) ;
	return TRUE ;
}

BOOL CWndLessRootImp::UpdateTooltipEx(HWL hChild, LPCTSTR lpszTip) 
{
	if (m_hTooltip == NULL)
		return FALSE ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(hChild) ;
	if (pos == m_mapChildTooltips.end())
		return FALSE ;

	if (lpszTip != LPSTR_TEXTCALLBACK && lpszTip != LPSTR_TEXTCALLBACK_OWNER)
	{
		pos->second.type = 0 ;
		pos->second.tip_text = lpszTip ;
	}
	else
	{
		pos->second.type = (int)lpszTip ;
		pos->second.tip_text.Empty() ;
	}
	return TRUE ;
}

BOOL CWndLessRootImp::ShowTooltipEx(HWL hChild, BOOL bShow) 
{
	if (m_hTooltip == NULL)
		return FALSE ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(hChild) ;
	if (pos == m_mapChildTooltips.end())
		return FALSE ;

	// 判断鼠标是否在当前子窗口上
	RECT rcChildInScreen ;
	hChild->GetRectInScreen(&rcChildInScreen) ;
	POINT ptCursor ;
	::GetCursorPos(&ptCursor) ;
	if (!::PtInRect(&rcChildInScreen, ptCursor))
		return FALSE ;

	if (pos->second.show != bShow)
	{
		pos->second.show = bShow ;
		TrackTooltip(hChild, bShow, WM_MOUSEMOVE, 0, 0) ;
	}

	return TRUE ;
}

BOOL CWndLessRootImp::RemoveTooltipEx(HWL hChild) 
{
	if (m_hTooltip == NULL)
		return FALSE ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(hChild) ;
	if (pos == m_mapChildTooltips.end())
		return FALSE ;

	m_mapChildTooltips.erase(pos) ;
	return TRUE ;
}

// virtual
HRESULT CWndLessRootImp::C_RegisterDragDrop(HWL hChild, LPDROPTARGET pDropTarget) 
{
    if (NULL == hChild || !m_wrapper->IsChild(hChild, TRUE))
        return DRAGDROP_E_INVALIDHWND ;

    if (pDropTarget == NULL)
        return E_INVALIDARG ;

    if (!m_register_drag_drop)
    {
        HRESULT hr = ::RegisterDragDrop(m_wrapper->GetHost(), m_drop_target) ;
        if (S_OK != hr)
            return hr ;

        m_register_drag_drop = TRUE ;
    }

    if (m_wl2droptarget.find(hChild) != m_wl2droptarget.end())
        return DRAGDROP_E_ALREADYREGISTERED ;

    m_wl2droptarget.insert(std::make_pair(hChild, pDropTarget)) ;

    return S_OK ;
}

// virtual 
// 孩子销毁时要不要自动取消呢？
HRESULT CWndLessRootImp::C_RevokeDragDrop(HWL hChild) 
{
    if (!m_register_drag_drop)
        return DRAGDROP_E_NOTREGISTERED ;

    if (NULL == hChild || !m_wrapper->IsChild(hChild, TRUE))
        return DRAGDROP_E_INVALIDHWND ;

    std::map<HWL, IDropTarget*>::iterator pos = m_wl2droptarget.find(hChild) ;
    if (pos == m_wl2droptarget.end())
        return DRAGDROP_E_NOTREGISTERED ;

    m_wl2droptarget.erase(pos) ;

    if (m_wl2droptarget.empty())
    {
        m_register_drag_drop = FALSE ;
        ::RevokeDragDrop(m_wrapper->GetHost()) ;
    }

    return S_OK ;
}

IDropTarget * CWndLessRootImp::GetChildDropTarget(HWL hChild) const 
{
    std::map<HWL, IDropTarget*>::const_iterator pos = m_wl2droptarget.find(hChild) ;
    if (pos != m_wl2droptarget.end())
        return pos->second ;

    return NULL ;
}

BOOL CWndLessRootImp::PollMessage(HWL hChild, UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
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

BOOL CWndLessRootImp::ProcessTooltipNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) 
{
	if (m_hTooltip == NULL || m_hHotChild == NULL)
		return FALSE ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(m_hHotChild) ;
	if (pos == m_mapChildTooltips.end())
		return FALSE ;

	if (!pos->second.show)
		return TRUE ;

	LPNMHDR lpNMHDR = (LPNMHDR)lParam ;
	switch (lpNMHDR->code)
	{
	case TTN_GETDISPINFO :
		{
			if (pos->second.type == (int)LPSTR_TEXTCALLBACK)
			{
				m_hHotChild->SendMessage(WM_NOTIFY, wParam, lParam) ;
			}
			else if (pos->second.type == (int)LPSTR_TEXTCALLBACK_OWNER)
			{
				HWL hParent = m_hHotChild->GetParent() ;
				if (hParent != NULL)
				{
					hParent->SendMessage(WM_NOTIFY, (WPARAM)(m_hHotChild->GetID()), lParam) ;
				}
			}
			else
			{
				LPNMTTDISPINFO lpnmtdi = (LPNMTTDISPINFO)lParam ;
				lpnmtdi->lpszText = (LPTSTR)(pos->second.tip_text.GetString()) ;
			}
		}
		break ;
	}

	return TRUE ;
}

void CWndLessRootImp::TrackTooltip(HWL hChild, BOOL bEnter, UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_hTooltip == NULL)
		return ;

	MAP_HWL2STRING::iterator pos = m_mapChildTooltips.find(hChild) ;
	if (pos == m_mapChildTooltips.end())
		return ;

	if (bEnter)
	{
		MSG msg ;
		msg.hwnd = m_wrapper->GetHost() ;
		msg.message = message ;
		msg.wParam = wParam ;
		msg.lParam = lParam ;
		msg.time = ::GetMessageTime() ;
		msg.pt = CPoint(::GetMessagePos()) ;
		::SendMessage(m_hTooltip, TTM_RELAYEVENT, 0, (LPARAM)&msg) ;
	}
	else
	{
		::SendMessage(m_hTooltip, TTM_POP, 0, 0) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implementations

BOOL CWndLessRootImp::Create(
						 DWORD /*dwStyle*/, 
						 int /*x*/, int /*y*/, int /*cx*/, int /*cy*/,
						 IWndLess * /*pParent*/,
						 UINT /*uID*/
						 ) 
{
	ASSERT(FALSE) ;
	return FALSE ;
}

IWndLessCenter * CWndLessRootImp::GetCenter() const 
{
	return (IWndLessCenter*)m_wrapper ;
}

// Operation
void CWndLessRootImp::_InternalOnDraw(HDC hDC, RECT const &rcUpdate) 
{
	//DWORD dwBegin = GetTickCount() ;

	CRect rcInHost ;
	m_wrapper->GetRectInHost(rcInHost) ;
	RECT rcInsterset ;
	if (!m_wrapper->IsWndLessVisible() || !::IntersectRect(&rcInsterset, &rcInHost, &rcUpdate))
		return ;

	//TRACE(_T("[%p]Draw 0 cost %d ms \n"), m_wrapper, GetTickCount() - dwBegin) ;

	HDC hMemDC = ::CreateCompatibleDC(hDC) ;
	HBITMAP hMemBmp = ::CreateCompatibleBitmap(hDC, rcInHost.Width(), rcInHost.Height()) ;
	HBITMAP hOldMemBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp) ;

	HFONT hFont = (HFONT)::SendMessage(m_hHost, WM_GETFONT, 0, 0) ;
	HFONT hOldFont = (HFONT)::SelectObject(hMemDC, hFont) ;

	//TRACE(_T("[%p]Draw 1 cost %d ms \n"), m_wrapper, GetTickCount() - dwBegin) ;

	// 绘制自己
	m_wrapper->OnDraw(hMemDC, rcUpdate) ;

	//TRACE(_T("[%p]Draw 2 cost %d ms \n"), m_wrapper, GetTickCount() - dwBegin) ;

	// 绘制孩子
	m_wrapper->OnDrawChild(hMemDC, rcUpdate) ;

	//TRACE(_T("[%p]Draw 3 cost %d ms \n"), m_wrapper, GetTickCount() - dwBegin) ;

	::BitBlt(
		hDC,
		rcUpdate.left,
		rcUpdate.top,
		rcUpdate.right - rcUpdate.left,
		rcUpdate.bottom - rcUpdate.top,
		hMemDC,
		rcUpdate.left,
		rcUpdate.top,
		SRCCOPY
		) ;

	::SelectObject(hMemDC, hOldFont) ;
	::SelectObject(hMemDC, hOldMemBmp) ;
	::DeleteDC(hMemDC) ;
	::DeleteObject(hMemBmp) ;

	//TRACE(_T("[%p]Draw cost %d ms \n"), m_wrapper, GetTickCount() - dwBegin) ;
}

// 获得客户区在屏幕坐标系中的位置
void CWndLessRootImp::GetRectInScreen(LPRECT lpRect) const 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	::GetClientRect(m_hHost, lpRect) ;
	::ClientToScreen(m_hHost, (LPPOINT)lpRect) ;
	::ClientToScreen(m_hHost, ((LPPOINT)lpRect) + 1) ;

// 	::SetRect(
// 		lpRect, 
// 		rcClientInScreen.left,
// 		rcClientInScreen.top,
// 		rcClientInScreen.left + m_szClient.cx,
// 		rcClientInScreen.top + m_szClient.cy
// 		) ;
}

// 获得客户区在宿主坐标系中的位置
void CWndLessRootImp::GetRectInHost(LPRECT lpRect) const 
{
	m_wrapper->GetRectInParent(lpRect) ;
}

// Message Handler
BOOL CWndLessRootImp::OnWndMsg(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/, LRESULT * /*pResult*/)
{
	return FALSE ;
}

void CWndLessRootImp::OnMinimized(WNDSIZE enPrev) 
{
	enPrev ;
}

void CWndLessRootImp::OnMaximized(WNDSIZE enPrev) 
{
	enPrev ;
}

void CWndLessRootImp::OnRestored(WNDSIZE enPrev) 
{
	enPrev ;
}

//////////////////////////////////////////////////////////////////////////
// RootDropTarget class

RootDropTarget::RootDropTarget(CWndLessRootImp * root) 
: m_ref_count(0), m_root(root), m_curr_drop_target(NULL), m_curr_drag_data_object(NULL)
{

}

//////////////////////////////////////////////////////////////////////////
// IUnknown Interface

// virtual 
HRESULT RootDropTarget::QueryInterface( 
                                       /* [in] */ REFIID riid,
                                       /* [annotation][iid_is][out] */ 
                                       __RPC__deref_out  void **ppvObject) 
{
    *ppvObject = NULL ;
    HRESULT hr = E_NOINTERFACE ;

    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObject = this ;
    }
    else if (IsEqualIID(riid, IID_IDropTarget))
    {
        *ppvObject = (IDropTarget*)this ;
    }

    if (*ppvObject != NULL)
    {
        AddRef() ;
        hr = S_OK ;
    }

    return hr ;
}

// virtual 
ULONG RootDropTarget::AddRef( void) 
{
    ++ m_ref_count ;
    return m_ref_count ;
}

// virtual 
ULONG RootDropTarget::Release( void) 
{
    -- m_ref_count ;
    if (m_ref_count == 0)
    {
        delete this ;
    }

    return m_ref_count ;
}

//////////////////////////////////////////////////////////////////////////
// IDropTarget methods

HRESULT STDMETHODCALLTYPE RootDropTarget::DragEnter( 
    /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ __RPC__inout DWORD *pdwEffect) 
{
    HRESULT hr = E_UNEXPECTED ;
    IDropTarget * dt = NULL ;

    m_curr_drag_data_object = pDataObj ;

    POINT point = {pt.x, pt.y} ;
    m_root->ScreenToClient(&point) ;
    HWL child = m_root->_InternalHitTest(point) ;
    if (child != NULL)
        dt = m_root->GetChildDropTarget(child) ;

    if (NULL != dt)
    {
        m_curr_drop_target = dt ;
        m_curr_drop_target->DragEnter(pDataObj, grfKeyState, pt, pdwEffect) ;
    }

    return S_OK ;
}

HRESULT STDMETHODCALLTYPE RootDropTarget::DragOver( 
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ __RPC__inout DWORD *pdwEffect) 
{
    HRESULT hr = E_UNEXPECTED ;
    IDropTarget * dt = NULL ;

    POINT point = {pt.x, pt.y} ;
    m_root->ScreenToClient(&point) ;
    HWL child = m_root->_InternalHitTest(point) ;
    if (child != NULL)
        dt = m_root->GetChildDropTarget(child) ;

    if (m_curr_drop_target != dt)
    {
        if (m_curr_drop_target != NULL)
            m_curr_drop_target->DragLeave() ;

        m_curr_drop_target = dt ;
        if (m_curr_drop_target != NULL)
            return m_curr_drop_target->DragEnter(m_curr_drag_data_object, grfKeyState, pt, pdwEffect) ;
        else
        {
            *pdwEffect = DROPEFFECT_NONE ;
            return hr ;
        }
    }
    else
    {
        if (m_curr_drop_target != NULL)
            return m_curr_drop_target->DragOver(grfKeyState, pt, pdwEffect) ;
        else
        {
            *pdwEffect = DROPEFFECT_NONE ;
            return hr ;
        }
    }
}

HRESULT STDMETHODCALLTYPE RootDropTarget::DragLeave( void) 
{
    if (m_curr_drop_target != NULL)
        return m_curr_drop_target->DragLeave() ;

    return E_UNEXPECTED ;
}

HRESULT STDMETHODCALLTYPE RootDropTarget::Drop( 
    /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ __RPC__inout DWORD *pdwEffect) 
{
    if (m_curr_drop_target != NULL)
        return m_curr_drop_target->Drop(pDataObj, grfKeyState, pt, pdwEffect) ;

    return E_UNEXPECTED ;
}


template<>
UILIB_EXPORT WndLessCenterImpBase *CreateDefaultImp<WndLessCenterImpBase>(IWndLessCenter *wrapper)
{
	CWndLessRootImp* ret = new CWndLessRootImp();
	ret->SetWrapper(wrapper);
	return ret;
};

