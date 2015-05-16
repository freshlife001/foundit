// MyDropTarget.cpp : 实现文件
//

#include "stdafx.h"
#include "WLTabDropTarget.h"
#include "WLDragTabCtrl.h"
#include "logex.h"
#include "common/MemoryLeakChecker.h"
// WLTabDropTarget
WLTabDropTarget::WLTabDropTarget( )
:m_bEntered( FALSE )
{
}

WLTabDropTarget::~WLTabDropTarget()
{
}
DROPEFFECT WLTabDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	if ( !IsPointInControl(pWnd, point) )
	{
		return DROPEFFECT_NONE;
	}
	DROPEFFECT eff = m_pTabCtrl->OnDragEnter( pWnd, pDataObject, dwKeyState, point );
	if ( eff != DROPEFFECT_NONE )
	{
		m_bEntered = TRUE;
	}
	return eff;
}

DROPEFFECT WLTabDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT eff = DROPEFFECT_NONE;
	if ( !IsPointInControl(pWnd, point) )
	{
		LOG_DEBUG( L"OnDragOver: out control range");
		if ( m_bEntered )
		{
			LOG_DEBUG( L"OnDragOver: DragLeave");
			m_pTabCtrl->OnDragLeave( pWnd );
			m_bEntered = FALSE;
		}
	}
	else
	{
		if ( m_bEntered )
		{
			eff = m_pTabCtrl->OnDragOver( pWnd, pDataObject, dwKeyState, point );
		}
		else
		{
			LOG_DEBUG( L"OnDragOver: DragEnter");
			eff = m_pTabCtrl->OnDragEnter( pWnd, pDataObject, dwKeyState, point );
			if ( eff != DROPEFFECT_NONE )
			{
				m_bEntered = TRUE;
			}
		}
	}
	return eff;
}

BOOL WLTabDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	if ( !IsPointInControl(pWnd, point) && m_bEntered )
	{
		return FALSE;
	}
	m_bEntered = FALSE;
	return m_pTabCtrl->OnDrop( pWnd, pDataObject, dropEffect, point );
}

void WLTabDropTarget::OnDragLeave( CWnd* pWnd )
{
	if ( m_bEntered )
	{
		m_pTabCtrl->OnDragLeave( pWnd );
		m_bEntered = FALSE;
	}
}

BOOL WLTabDropTarget::Register( WLDragTabCtrlImp* pWLWnd )
{
	if ( pWLWnd == NULL )
	{
		return FALSE;
	}
	m_pTabCtrl = pWLWnd;
	HWND hHost = pWLWnd->GetCenter()->GetHost();
	return COleDropTarget::Register( CWnd::FromHandle( hHost ));
}

BOOL WLTabDropTarget::IsPointInControl( CWnd* pWnd, CPoint point )
{
	BOOL bPtInWndless = FALSE;
	ASSERT( pWnd->GetSafeHwnd() == m_pTabCtrl->GetCenter()->GetHost() );
	CRect rcWlCtrl;
	m_pTabCtrl->GetRectInHost( &rcWlCtrl );
	if ( rcWlCtrl.PtInRect( point ))
	{
		bPtInWndless = TRUE;
	}
	return bPtInWndless;
}



// WLTabDropTarget 消息处理程序
