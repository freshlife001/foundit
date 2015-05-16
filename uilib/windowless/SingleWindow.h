#pragma once

#include "BaseWndLessWnd.h"

class SingleWindow : public BaseWndlessWndDestroyListener
{
public:

	typedef BaseWndlessWnd * (*CREATEWINDOWOBJECT)(SingleWindow *) ;

	SingleWindow(CREATEWINDOWOBJECT fun);
	~SingleWindow(void);

	// 由外界调用
	// 打开单一窗口，
	// 如果未创建，则创建并激活
	// 如果已创建，则激活
	void Open() ;

	// 窗口是否已打开
	BOOL IsOpen() ;

	// 由外界调用
	// 关闭单一窗口，这类窗口对象一般支持在销毁时delete自己
	void Close() ;

	BaseWndlessWnd * GetWindow() const ;
	CWndLessRoot * GetWindowRoot() const ;

	//////////////////////////////////////////////////////////////////////////
	// BaseWndlessWndDestroyListener

	virtual void OnSomeWndDestroy(BaseWndlessWnd * wnd) ;

protected:
	/*virtual*/ void Activate() ;
	//virtual HWND CreateSingleWindow() = 0 ;

private:
	//HWND m_single_window ;
	CREATEWINDOWOBJECT m_create_window_fun ;
	BaseWndlessWnd * m_single_window ;
};
