#include "StdAfx.h"
#include "WLTempEdit.h"

WLTempEdit::WLTempEdit(void)
:
m_hOwner(NULL)
, m_editing(FALSE)
{
}

WLTempEdit::~WLTempEdit(void)
{
}

void WLTempEdit::BeginEdit() 
{
	if (m_editing)
		return ;

	m_editing = TRUE ;
	SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
	SetFocus() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLTempEdit::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = __super::OnWndMsg(message, wParam, lParam, pResult) ;

	if (message == WM_KILLFOCUS || message == WM_LDOWNHOST)
	{
		if (m_editing && m_hOwner != NULL)
		{
			m_hOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), EN_ENDEDIT), (LPARAM)this) ;
			m_editing = FALSE ;
		}
		bRes = TRUE ;
	}
	else if (message == WM_KEYDOWN)		// 拦住，不往上传
	{
		if (m_hOwner != NULL)
		{
			if (m_editing)
			{
				if (wParam == VK_RETURN)
				{
					m_hOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), EN_ENDEDIT), (LPARAM)this) ;
					m_editing = FALSE ;
				}
				else if (wParam == VK_ESCAPE)
				{
					m_hOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), EN_CANCELEDIT), (LPARAM)this) ;
					m_editing = FALSE ;
				}
			}
		}
		bRes = TRUE ;
	}

	return bRes ;
}
