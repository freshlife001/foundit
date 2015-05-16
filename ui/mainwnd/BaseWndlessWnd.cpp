// BaseWndlessWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "BaseWndlessWnd.h"
#include "uilib/uilib.h"

// BaseWndlessWnd

IMPLEMENT_DYNAMIC(BaseWndlessWnd, CWnd)

BaseWndlessWnd::BaseWndlessWnd()
: 
m_listener(NULL)
, m_root(NULL)
// , m_auto_delete(auto_delete)
{

}

BaseWndlessWnd::~BaseWndlessWnd()
{
	if (m_root != NULL)
		delete m_root ;
}

BOOL BaseWndlessWnd::SetWndLessRoot(CWndLessRoot * root) 
{
	if (root == NULL)
		return FALSE ;

	if (GetSafeHwnd() != NULL)
		return FALSE ;

	if (m_root != NULL)
		delete m_root ;

	m_root = root ;

	return TRUE ;
}

BOOL BaseWndlessWnd::Create(
							DWORD dwExStyle,
							DWORD dwStyle,
							LPCTSTR lpszWindowName,
							int x, int y, int cx, int cy,
							HWND hParent,
							HMENU nIDorHMenu
							) 
{
	if (m_root == NULL)
		return FALSE ;

	return CWnd::CreateEx(
		dwExStyle, 
		ST_HDBASEWINDOW,
		lpszWindowName,
		dwStyle,
		x, y, cx, cy,
		hParent,
		nIDorHMenu,
		0
		) ;
}

LRESULT BaseWndlessWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//	if (message == WM_NCCREATE)
	if (message == WM_CREATE)
	{
		if (!m_root->CreateEx(GetSafeHwnd()))
			return -1 ;
	}
	else
	{
		LRESULT lResult = 0 ;
		if (m_root->DispatchMessage(message, wParam, lParam, &lResult))
		{
			return lResult ;
		}
	}

	return __super::WindowProc(message, wParam, lParam) ;
}

void BaseWndlessWnd::PostNcDestroy() 
{
	if (m_listener != NULL)
		m_listener->OnSomeWndDestroy(this) ;
}

BEGIN_MESSAGE_MAP(BaseWndlessWnd, CWnd)
END_MESSAGE_MAP()



// BaseWndlessWnd 消息处理程序


