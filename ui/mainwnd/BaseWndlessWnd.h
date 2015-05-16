#pragma once
#include "uilib/wndless/IWndLess.h"

// BaseWndlessWnd
class BaseWndlessWnd ;

class BaseWndlessWndDestroyListener
{
public:
	virtual void OnSomeWndDestroy(BaseWndlessWnd * wnd) = 0 ;
};

class BaseWndlessWnd : public CWnd
{
	DECLARE_DYNAMIC(BaseWndlessWnd)

public:
	BaseWndlessWnd();
	virtual ~BaseWndlessWnd();

	void SetDestroyListener(BaseWndlessWndDestroyListener * listener) { m_listener = listener ; }
	BOOL SetWndLessRoot(CWndLessRoot * root) ;
	CWndLessRoot * GetWndLessRoot() const { return m_root ; }

	// 可以创建TopLevel窗口或者子窗口
	BOOL Create(
		DWORD dwExStyle,
		DWORD dwStyle,
		LPCTSTR lpszWindowName,
		int x, int y, int cx, int cy,
		HWND hParent,
		HMENU nIDorHMenu
		) ;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	virtual void PostNcDestroy() ;

protected:
	BaseWndlessWndDestroyListener * m_listener ;
	CWndLessRoot * m_root ;
// 	BOOL m_auto_delete ;

protected:
	DECLARE_MESSAGE_MAP()
};


