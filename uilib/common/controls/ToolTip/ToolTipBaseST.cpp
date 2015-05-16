// ToolTipBaseST.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTipBaseST.h"

#include "logex.h"

/////////////////////////////////////////////////////////////////////////////
// CToolTipBaseST

CToolTipBaseST::CToolTipBaseST()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, ST_TOOLTIP_BASE, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_OWNDC ;
		wndcls.lpfnWndProc		= ::DefWindowProc ;					// 关联消息处理函数
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0 ;
		wndcls.hInstance		= hInst ;							// 实例句柄
		wndcls.hIcon			= NULL;								// 图标
		wndcls.hCursor			= ::LoadCursor( NULL, IDC_ARROW ) ;	// 光标
		//wndcls.hbrBackground	= HBRUSH(COLOR_INFOBK + 1) ;		// 画刷(提示条背景)
		wndcls.hbrBackground	= HBRUSH(0) ;						// 画刷(空画刷，不删除背景)
		wndcls.lpszMenuName		= NULL ;
		wndcls.lpszClassName	= ST_TOOLTIP_BASE ;				// 类名称
		
		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}
}

CToolTipBaseST::~CToolTipBaseST()
{
}


BEGIN_MESSAGE_MAP(CToolTipBaseST, CWnd)
	//{{AFX_MSG_MAP(CToolTipBaseST)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Operations

BOOL CToolTipBaseST::Pop(CWnd * pParent)
{
	// 先压入之前的气泡
	Push() ;
	
	// 创建窗口
#ifndef UNICODE
	TCHAR szWN[7] = {-63, -42, -84, 69, -43, -16, 0} ;
#else
	TCHAR szWN[4] = {26519, 29693, 38663, 0} ;
#endif
	
	BOOL bRes = CreateEx(
		WS_EX_TOOLWINDOW,
		ST_TOOLTIP_BASE,
		szWN,
		WS_POPUP,
		CRect(CW_USEDEFAULT, CW_USEDEFAULT, 0, 0),
		pParent,
		0) ;

	if (bRes)
	{
		// 记录字体
		LOGFONT lf ;
		pParent->GetFont()->GetLogFont(&lf) ;
		m_font.CreateFontIndirect(&lf) ;
	}

	return bRes ;
}

// 函数功能：压入气泡提示
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/12/2009]
void CToolTipBaseST::Push() 
{
	// 如果当前是一个窗口，则销毁窗口
	if (::IsWindow(GetSafeHwnd()))
		DestroyWindow() ;
	m_font.DeleteObject() ;
}

/////////////////////////////////////////////////////////////////////////////
// CToolTipBaseST message handlers

int CToolTipBaseST::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	UNUSED_ALWAYS(pDesktopWnd) ;
	UNUSED_ALWAYS(nHitTest) ;
	UNUSED_ALWAYS(message) ;
	return MA_NOACTIVATE ;
//	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

LRESULT CToolTipBaseST::OnGetFont(WPARAM, LPARAM) 
{
	return (LRESULT)m_font.GetSafeHandle() ;
}