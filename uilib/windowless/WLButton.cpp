#include "StdAfx.h"
#include "WLButton.h"
#include "common/MemoryLeakChecker.h"
WLButtonImp::WLButtonImp()
:m_dwStatus(BS_NORMAL)
, m_bLButtonDown(FALSE)
{
	
}

WLButtonImp::~WLButtonImp(void)
{
}

int WLButtonImp::GetCheck()  
{
	return (int)SendMessage(BM_GETCHECK, 0, 0) ;
}

void WLButtonImp::SetCheck(int nCheck) 
{
	SendMessage(BM_SETCHECK, nCheck, 0) ;
}

BOOL WLButtonImp::DrawBySelf(HDC /*hDC*/, RECT const &/*rcUpdate*/, DWORD /*dwStatus*/)
{
	return FALSE ;
}

BOOL WLButtonImp::CustomDraw(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) 
{
	// 先由自己重绘
	//if (0 != SendMessage(WM_NOTIFY, 0, (LPARAM)&info))
	//	return ;
	if (m_wrapper->DrawBySelf(hDC, rcUpdate, dwStatus))
		return TRUE ;

	NMCDWLBTNINFO info ;
	info.hdr.hWlFrom = m_wrapper ;
	info.hdr.idFrom = GetID() ;
	info.hdr.code = NM_CUSTOMDRAW ;
	info.hDC = hDC ;
	GetRectInHost(&info.rc) ;
	info.dwStatus = m_dwStatus ;

	// 再由父亲重绘
	if (0 != GetParent()->SendMessage(WM_NOTIFY, GetID(), (LPARAM)&info))
		return TRUE ;

	// 默认绘制
	return FALSE ;
}

void WLButtonImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	//TRACE(_T("[%d, %p]WLButton::OnDraw\n"), GetTickCount(), this) ;
	if (CustomDraw(hDC, rcUpdate, m_dwStatus))
		return ;

	CDC dc ;
	dc.Attach(hDC) ;

	CRect rcClient ;
	GetRectInHost(rcClient) ;

	if (m_dwStatus & BS_DISABLED)
	{
		
	}
	else if (m_dwStatus & BS_SELECTED)
	{
		dc.DrawFrameControl(rcClient, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED) ;
	}
	else if (m_dwStatus & BS_HOT)
	{
		dc.FillSolidRect(rcClient, RGB(190, 190, 190)) ;
		dc.DrawEdge(rcClient, EDGE_RAISED, BF_RECT) ;
	}
	else
	{
		dc.DrawFrameControl(rcClient, DFC_BUTTON, DFCS_BUTTONPUSH) ;
	}

	if (m_dwStatus & BS_FOCUS)
	{
		CRect rcFocus(rcClient) ;
		rcFocus.DeflateRect(3, 3, 3, 3) ;
		//dc.DrawFocusRect(rcFocus) ;
		dc.MoveTo(rcFocus.TopLeft()) ;
		dc.LineTo(rcFocus.BottomRight()) ;
	}

	dc.Detach() ;
}

BOOL WLButtonImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;
	switch (message)
	{
	case WM_MOUSEHOVER :
		{
			if (GetStyle() & WS_DISABLED)
				;
			else
			{
				if (!(m_dwStatus & BS_HOT))
				{
					m_dwStatus |= BS_HOT ;
					UpdateView() ;
				}
			}
		}
		break ;

	case WM_MOUSEMOVE :
		{
			if (GetStyle() & WS_DISABLED)
			{
			}
			else
			{
				if (m_bLButtonDown)
				{
					CPoint pt(lParam) ;
					if (PtInWndLess(pt))
					{
						if (!(m_dwStatus & BS_SELECTED))
						{
							m_dwStatus |= BS_SELECTED ;
							UpdateView() ;
						}
					}
					else
					{
						if (m_dwStatus & BS_SELECTED)
						{
							m_dwStatus &= ~BS_SELECTED ;
							UpdateView() ;
						}
					}
				}

				bRes = TRUE ;
			}
		}
		break ;

	case WM_MOUSELEAVE :
		{
			if (GetStyle() & WS_DISABLED)
			{
			}
			else
			{
				if (m_dwStatus & BS_HOT)
				{
					m_dwStatus &= ~BS_HOT ;
					UpdateView() ;
				}

				// 将鼠标移出事件传上去 
			}
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			if (GetStyle() & WS_DISABLED)
			{
			}
			else
			{
				//TRACE(_T("WLButton::WM_LBUTTONDOWN\n")) ;
				SetFocus() ;
				SetCapture() ;
				if (!(m_dwStatus & BS_SELECTED))
				{
					m_dwStatus |= BS_SELECTED ;
					UpdateView() ;
				}
				m_bLButtonDown = TRUE ;
				bRes = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONUP :
		{
			if (GetStyle() & WS_DISABLED)
			{
			}
			else
			{
				//TRACE(_T("WLButton::WM_LBUTTONUP\n")) ;
				// 记录鼠标是否按下，因为释放鼠标控制权后会清除该标记
				BOOL bLButtonDown = m_bLButtonDown ;
				ReleaseCapture() ;

				if (bLButtonDown && PtInWndLess(CPoint(lParam)))
				{
					if (!SendMessage(WM_COMMAND + WM_REFLECT_BASE, MAKEWPARAM(GetID(), BN_CLICKED), 0))
					{
						GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), BN_CLICKED), (LPARAM)m_wrapper) ;
					}
				}

				// 在释放鼠标所有权时清除BS_PUSHED状态并刷新视图
				// if (m_dwStatus & BS_PUSHED)
				// {
				// 		m_dwStatus &= ~BS_PUSHED ;
				// 		UpdateView() ;
				// }
				bRes = TRUE ;
			}
		}
		break ;

	case WM_SETFOCUS :
		{
			//::SendMessage(GetCenter()->GetHost(), WM_INPUTLANGCHANGE, 134, 0x08040804) ;
			//::PostMessage(GetCenter()->GetHost(), WM_INPUTLANGCHANGEREQUEST, 1, 0x08040804) ;
			//TRACE(_T("WLButton SetFocus\n")) ;
			//ImmDisableIME(0) ;
			if (!(m_dwStatus & BS_FOCUS))
			{
				m_dwStatus |= BS_FOCUS ;
				UpdateView() ;
			}
			bRes = TRUE ;
		}
		break ;

	case WM_KILLFOCUS :
		{
			//TRACE(_T("WLButton KillFocus: %x\n"), wParam) ;
			if (m_dwStatus & BS_FOCUS)
			{
				m_dwStatus &= ~BS_FOCUS ;
				UpdateView() ;
			}

			bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			if (m_dwStatus & BS_SELECTED)
			{
				m_dwStatus &= ~BS_SELECTED ;
				UpdateView() ;
			}

			if (m_bLButtonDown)
				m_bLButtonDown = FALSE ;

			bRes = TRUE ;
		}
		break ;

	case WM_GETDLGCODE :
		{
			*pResult = DLGC_UNDEFPUSHBUTTON | DLGC_BUTTON ;
			bRes = TRUE ;
		}
		break ;

	case WM_ENABLE :
		{
			if (wParam)
			{
				if (m_dwStatus & BS_DISABLED)
				{
					m_dwStatus &= ~BS_DISABLED ;
					UpdateView() ;
				}
			}
			else
			{
				if (!(m_dwStatus & BS_DISABLED))
				{
					m_dwStatus |= BS_DISABLED ;
					UpdateView() ;
				}
			}
			bRes = TRUE ;
		}
		break ;

	case BM_SETCHECK :
		{
			if (wParam != 0)
			{
				if (!(m_dwStatus & BS_CHECK))
				{
					m_dwStatus |= BS_CHECK ;
					UpdateView() ;
				}
			}
			else
			{
				if (m_dwStatus & BS_CHECK)
				{
					m_dwStatus &= ~BS_CHECK ;
					UpdateView() ;
				}
			}
			bRes = TRUE ;
		}
		break ;

	case BM_GETCHECK :
		{
			*pResult = (m_dwStatus & BS_CHECK) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}


template<>
UILIB_EXPORT WLButtonImpBase *CreateDefaultImp< WLButtonImpBase >(IWLButton * wrapper)

{
	WLButtonImp *ret = new WLButtonImp();
	ret->SetWrapper(wrapper);
	return ret;
};
