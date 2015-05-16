#include "StdAfx.h"
#include "WLScrollBarThumb.h"
#include "common/MemoryLeakChecker.h"
WLScrollBarThumbImp::WLScrollBarThumbImp(void)
{
}

WLScrollBarThumbImp::~WLScrollBarThumbImp(void)
{
}

BOOL WLScrollBarThumbImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = __super::OnWndMsg(message, wParam, lParam, pResult) ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
		OnLButtonDown(wParam, CPoint(lParam)) ;
		break ;

	case WM_MOUSEMOVE :
		OnMouseMove(wParam, CPoint(lParam)) ;
		break ;

	case WM_LBUTTONUP :
		OnLButtonUp(wParam, CPoint(lParam)) ;
		break ;
	}

	return bRes ;
}

void WLScrollBarThumbImp::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	//TRACE(_T("WLScrollBarThumb::OnLButtonDown\n")) ;
	NMDRAGINFO info ;
	info.hdr.hWlFrom = this ;
	info.hdr.idFrom = GetID() ;
	info.hdr.code = (UINT)NM_BEGINDRAG ;
	info.point = point ;
	GetParent()->SendMessage(WM_NOTIFY, GetID(), (LPARAM)&info) ;
}

void WLScrollBarThumbImp::OnLButtonUp(UINT /*nFlags*/, CPoint point) 
{
	//TRACE(_T("WLScrollBarThumb::OnLButtonUp\n")) ;
	NMDRAGINFO info ;
	info.hdr.hWlFrom = this ;
	info.hdr.idFrom = GetID() ;
	info.hdr.code = (UINT)NM_ENDDRAG ;
	info.point = point ;
	GetParent()->SendMessage(WM_NOTIFY, GetID(), (LPARAM)&info) ;
}

void WLScrollBarThumbImp::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	if (m_bLButtonDown)
	{
		//TRACE(_T("WLScrollBarThumb::OnMouseMove\n")) ;

		m_dwStatus |= BS_SELECTED ;

		NMDRAGINFO info ;
		info.hdr.hWlFrom = this ;
		info.hdr.idFrom = GetID() ;
		info.hdr.code = (UINT)NM_DRAGGING ;
		info.point = point ;
		GetParent()->SendMessage(WM_NOTIFY, GetID(), (LPARAM)&info) ;
	}
}


template<>
WLScrollBarThumbImp *CreateDefaultImp< WLScrollBarThumbImp >(IWLButton * wrapper)

{
	WLScrollBarThumbImp *ret = new WLScrollBarThumbImp();
	ret->SetWrapper(wrapper);
	return ret;
};
