#include "StdAfx.h"
#include "SingleWindow.h"

SingleWindow::SingleWindow(CREATEWINDOWOBJECT fun)
:
m_create_window_fun(fun)
, m_single_window(NULL)
{
}

SingleWindow::~SingleWindow(void)
{
	Close() ;
}

void SingleWindow::Open() 
{
	if (m_single_window == NULL)
	{
		if (m_create_window_fun == NULL)
			return ;

		m_single_window = m_create_window_fun(this) ;
	}

	Activate() ;
}

BOOL SingleWindow::IsOpen() 
{
	return (m_single_window != NULL) ;
}

void SingleWindow::Close() 
{
	if (m_single_window != NULL)
	{
		m_single_window->DestroyWindow() ;
		delete m_single_window ;
		m_single_window = NULL ;
	}
}

BaseWndlessWnd * SingleWindow::GetWindow() const 
{
	return m_single_window ;
}

CWndLessRoot * SingleWindow::GetWindowRoot() const 
{
	if (m_single_window == NULL)
		return NULL ;

	return m_single_window->GetWndLessRoot() ;
}

void SingleWindow::OnSomeWndDestroy(BaseWndlessWnd * wnd) 
{
	if (m_single_window == wnd)
	{
		delete m_single_window ;
		m_single_window = NULL ;
	}
}

void SingleWindow::Activate() 
{
	if (m_single_window != NULL)
	{
		m_single_window->ShowWindow(SW_SHOWNORMAL) ;
		m_single_window->SetActiveWindow() ;
	}
}
