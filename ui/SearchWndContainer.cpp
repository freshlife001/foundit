#include "StdAfx.h"
#include "uilib/uilib.h"
#include "SearchWndContainer.h"
#include "SearchWndContainerRoot.h"
CSearchWndContainer::CSearchWndContainer(void)
{
	m_pRoot = new CSearchWndContainerRoot( );
}

CSearchWndContainer::~CSearchWndContainer(void)
{
	delete m_pRoot;
	m_pRoot = NULL;
}


BEGIN_MESSAGE_MAP( CSearchWndContainer, CWnd )
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



LRESULT CSearchWndContainer::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (m_pRoot->GetHost() != NULL)
	{
		LRESULT lResult = 0 ;
		if (m_pRoot->DispatchMessage(message, wParam, lParam, &lResult))
			return lResult ;
	}
	return CWnd::WindowProc( message, wParam, lParam ) ;
}

int CSearchWndContainer::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CSearchWndContainer::Create( CWnd* pParent )
{
	SIZE defSize = m_pRoot->GetDefaultSize();
	BOOL bRes = CWnd::CreateEx( 0, ST_HDBASEWINDOW, NULL,
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN, 
		CRect( 0, 0, defSize.cx, defSize.cy ),  pParent, 0, NULL );
	if ( bRes )
	{
		m_pRoot->CreateEx(GetSafeHwnd()) ;
	}
	return bRes;
}

BOOL CSearchWndContainer::PreTranslateMessage( MSG* pMsg )
{
	return __super::PreTranslateMessage(pMsg);
}


void CSearchWndContainer::OnDestroy()
{
	if ( m_pRoot )
	{
		m_pRoot->Destroy();
	}
}