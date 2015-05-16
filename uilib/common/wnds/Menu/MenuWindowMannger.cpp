#include "StdAfx.h"
#include "MenuWindowMannger.h"

#include "common/utils/ImageEx/ImageResource.h"
#include "common/utils/ImageEx/Shared_ImageEx.h"
#include "common/utils/ImageEx/ImageEx.h"

#include "MenuWindow.h"
#include <algorithm>

// Image * CMenuWindowMannger::s_pImgMenuBkgnd = NULL ;

CMenuWindowMannger::CMenuWindowMannger(void)
:
m_hHotMenuWindow(NULL)
, m_nResult(0)
{
	HDC hDesktopDC = ::GetWindowDC(NULL) ;

	Shared_ImageEx imgMenuBkgnd = Shared_ImageEx::FromId(IDI_MENU_BKGND_PNG) ;
	//Shared_ImageEx imgMenuBkgnd = Shared_ImageEx::FromId(IDI_WND_FRAME_PNG) ;
	m_cvMenuBkgnd.CreateFromImage(hDesktopDC, imgMenuBkgnd, TRUE) ;

	Shared_ImageEx imgMenuItemHotBkgnd = Shared_ImageEx::FromId(IDI_MENU_ITEM_HOT_BKGND_PNG) ;
	m_cvMenuItemHotBkgnd.CreateFromImage(hDesktopDC, imgMenuItemHotBkgnd, TRUE) ;

	::ReleaseDC(NULL, hDesktopDC) ;

	LOGFONT lf = {0} ;
	lf.lfHeight = -12 ;
	lf.lfWeight = FW_NORMAL ;
	lf.lfCharSet = GB2312_CHARSET ;
	_tcsncpy_s(lf.lfFaceName, _T("宋体"), _countof(lf.lfFaceName) - 1) ;
	m_hFont = ::CreateFontIndirect(&lf) ;
}

CMenuWindowMannger::~CMenuWindowMannger(void)
{
	::DeleteObject(m_hFont) ;
}

int CMenuWindowMannger::Run(HHDMENU hMenu, unsigned int uFlags, int x, int y, HWND hOwner, LPRECT lprcExclude)
{
	hdui::PMENUCORE pMenuCore = (hdui::PMENUCORE)hMenu ;

	CMenuWindow * pMenuWindow = new CMenuWindow(this, TRUE) ;
	pMenuWindow->SetImage(&m_cvMenuBkgnd, &m_cvMenuItemHotBkgnd) ;
	pMenuWindow->SetFont(m_hFont) ;
	pMenuWindow->SetFlags(uFlags) ;
	//pMenuWindow->SetFlags(uFlags & (TPM_LEFTBUTTON | TPM_RIGHTBUTTON)) ;
	pMenuWindow->SetExcludeRect(lprcExclude) ;

	if (!pMenuWindow->Create(pMenuCore, x, y, hOwner))
		return 0 ;
	AppendMenuWindow(pMenuWindow->GetSafeHwnd()) ;

	::SetCursor(::LoadCursor(NULL, IDC_ARROW)) ;
	
	DWORD activate_thread_id = ::GetWindowThreadProcessId(::GetForegroundWindow(), NULL) ;
	DWORD current_thread_id = ::GetCurrentThreadId() ;
	::AttachThreadInput(activate_thread_id, current_thread_id, TRUE) ;

	pMenuWindow->SetCapture() ;

	::AttachThreadInput(activate_thread_id, current_thread_id, FALSE) ;

// 	int nRes = 0 ;

	MSG msg ;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// 退出
		if (m_listMenuWindows.empty() || !::IsWindow(m_listMenuWindows.front()))
			break ;
		if (msg.message == WM_NULL)
		{
			// 把选择菜单项ID放在WM_NULL的WPARAM参数中不稳定
// 			nRes = msg.wParam ;
			break ;
		}

		PreTranslateMessage(&msg) ;
		TranslateMessage(&msg) ;
		DispatchMessage(&msg) ;
	}

	pMenuWindow->DestroyWindow() ;

	// 根菜单窗口需主动销毁
	delete pMenuWindow ;
	pMenuWindow = NULL ;

	if ((m_nResult != 0) && !(uFlags & TPM_RETURNCMD))
		::PostMessage(hOwner, WM_COMMAND, MAKELPARAM(m_nResult, 0), 0) ;

	return m_nResult ;
}

void CMenuWindowMannger::AppendMenuWindow(HWND hMenuWnd) 
{
	//m_setMenuWindows.insert(hMenuWnd) ;
	m_listMenuWindows.push_back(hMenuWnd) ;
}

void CMenuWindowMannger::RemoveMenuWindow(HWND hMenuWnd) 
{
	std::list<HWND>::iterator pos = std::find(m_listMenuWindows.begin(), m_listMenuWindows.end(), hMenuWnd) ;
	if (pos != m_listMenuWindows.end())
	{
		m_listMenuWindows.erase(pos, m_listMenuWindows.end()) ;
	}
}

// void CMenuWindowMannger::RemoveMenuWindow(HWND hMenuWnd) 
// {
// 	std::set<HWND>::iterator pos = m_setMenuWindows.find(hMenuWnd) ;
// 	if (pos != m_setMenuWindows.end())
// 	{
// 		m_setMenuWindows.erase(pos) ;
// 	}
// }

void CMenuWindowMannger::Exit(int nResult) 
{
	::PostMessage(NULL, WM_NULL, 0, 0) ;
	m_nResult = nResult ;
}

void CMenuWindowMannger::PreTranslateMessage(LPMSG pMsg) 
{
	UINT message = pMsg->message ;
	
	if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
	{
		pMsg->hwnd = m_listMenuWindows.back() ;
	}
	else if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST)
	{
		HWND hWndUnderCursor = ::WindowFromPoint(pMsg->pt) ;
		if (message == WM_MOUSEMOVE)
		{
			if (m_hHotMenuWindow != hWndUnderCursor)
			{
				if (m_hHotMenuWindow != NULL)
				{
					// 只把鼠标消息告诉菜单窗口
					if (std::find(m_listMenuWindows.begin(), m_listMenuWindows.end(), m_hHotMenuWindow) != m_listMenuWindows.end())
						::SendMessage(m_hHotMenuWindow, WM_MOUSELEAVE, 0, 0) ;
				}

				m_hHotMenuWindow = hWndUnderCursor ;
			}
		}

		std::list<HWND>::iterator pos = 
			std::find(m_listMenuWindows.begin(), m_listMenuWindows.end(), hWndUnderCursor) ;
		if (pos != m_listMenuWindows.end())
		{
			// 鼠标移动至菜单窗口
			if (pMsg->hwnd != hWndUnderCursor)
			{
				// 鼠标消息分发给鼠标下的菜单窗口
				POINT pt ;
				pt.x = LOWORD(pMsg->lParam) ;
				pt.y = HIWORD(pMsg->lParam) ;
				::ClientToScreen(pMsg->hwnd, &pt) ;
				::ScreenToClient(hWndUnderCursor, &pt) ;

				pMsg->hwnd = hWndUnderCursor ;
				pMsg->lParam = MAKELPARAM(pt.x, pt.y) ;
				//TRACE(_T("Mouse Hover MenuWindow %d\n"), hWndUnderCursor) ;
			}
		}
		else
		{
			// 鼠标按在非菜单窗口上
			if (message == WM_LBUTTONDOWN)
			{
				//TRACE(_T("%#08x Post WM_NULL : %u\n"), pMsg->hwnd, 0) ;
				//::PostMessage(NULL, WM_NULL, 0, 0) ;
				Exit(0) ;

				POINT point = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)} ;
				POINT ptCursor = point ;
				::ClientToScreen(pMsg->hwnd, &ptCursor) ;

				// 判断是否同线程的窗口
				if (GetCurrentThreadId() == GetWindowThreadProcessId(hWndUnderCursor, NULL))
				{
					LRESULT lres = ::SendMessage(
						hWndUnderCursor,
						WM_NCHITTEST, 0,
						//MAKELPARAM(pMsg->pt.x, pMsg->pt.y)
						MAKELPARAM(ptCursor.x, ptCursor.y)
						) ;
					if (lres == HTCLIENT)
					{
// 						POINT ptClient = {ptCursor.x, ptCursor.y} ;
// 						::ScreenToClient(hWndUnderCursor, &ptClient) ;
// 						::PostMessage(hWndUnderCursor, WM_LBUTTONDOWN, pMsg->wParam, MAKELPARAM(ptClient.x, ptClient.y)) ;
					}
					else
					{
						::PostMessage(hWndUnderCursor, WM_NCLBUTTONDOWN, lres, MAKELPARAM(ptCursor.x, ptCursor.y)) ;
					}
				}
			}
			else if (message == WM_RBUTTONDOWN)
			{
				//TRACE(_T("%#08x Post WM_NULL : %u\n"), pMsg->hwnd, 0) ;
				//::PostMessage(NULL, WM_NULL, 0, 0) ;
				Exit(0) ;

				POINT point = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)} ;
				POINT ptCursor = point ;
				::ClientToScreen(pMsg->hwnd, &ptCursor) ;

				// 判断是否同线程的窗口
				if (GetCurrentThreadId() == GetWindowThreadProcessId(hWndUnderCursor, NULL))
				{
					LRESULT lres = ::SendMessage(
						hWndUnderCursor,
						WM_NCHITTEST, 0,
						MAKELPARAM(ptCursor.x, ptCursor.y)
						) ;
					if (lres == HTCLIENT)
					{
						POINT ptClient = {ptCursor.x, ptCursor.y} ;
						::ScreenToClient(hWndUnderCursor, &ptClient) ;
						::PostMessage(hWndUnderCursor, WM_RBUTTONDOWN, pMsg->wParam, MAKELPARAM(ptClient.x, ptClient.y)) ;
					}
					else
					{
						::PostMessage(hWndUnderCursor, WM_NCRBUTTONDOWN, lres, MAKELPARAM(ptCursor.x, ptCursor.y)) ;
					}
				}
			}
		}
	}
}
