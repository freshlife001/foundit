#include "StdAfx.h"
#include "WLWindowFace.h"

WLWindowFace::WLWindowFace(void)
: 
m_dragging(FALSE)
, m_listener(NULL)
{
}

WLWindowFace::~WLWindowFace(void)
{
}

CCanvasDC & WLWindowFace::GetFace() 
{
	return m_face ;
}

BOOL WLWindowFace::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent )
{
	if (!m_face.IsAvailable())
		return FALSE ;

	SetWndLessPos(0, 0, m_face.GetWidth(), m_face.GetHeight(), SWLP_CX | SWLP_CY) ;

	return TRUE ;
}

void WLWindowFace::OnMoved(POINT const &ptOld, POINT const &ptNew) 
{
	if (m_listener != NULL)
	{
		m_listener->OnFaceMove() ;
	}
}

void WLWindowFace::OnDraw(HDC hDC, RECT const &rcUpdate)
{
	RECT rcInHost ;
	GetRectInHost(&rcInHost) ;

	if (m_face.IsAvailable())
	{
		BLENDFUNCTION bfn ;
		bfn.BlendOp = AC_SRC_OVER ;
		bfn.BlendFlags = 0 ;
		bfn.SourceConstantAlpha = 255 ;
		bfn.AlphaFormat = AC_SRC_ALPHA ;

		::AlphaBlend(
			hDC,
			rcInHost.left, rcInHost.top, m_face.GetWidth(), m_face.GetHeight(),
			m_face,
			0, 0, m_face.GetWidth(), m_face.GetHeight(),
			bfn
			) ;
	}
}

BOOL WLWindowFace::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consume = FALSE ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
			BeginDrag(point) ;
			consume = TRUE ;
		}
		break ;

	case WM_MOUSEMOVE :
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
			DoDrag(point) ;
			consume = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			EndDrag() ;
			consume = TRUE ;
		}
		break ;
	}

	return consume ;
}

void WLWindowFace::BeginDrag(POINT const & point)
{
	if (m_dragging)
		return ;

	m_dragging = TRUE ;
	SetCapture() ;

	m_drag_point.x = point.x ;
	m_drag_point.y = point.y ;
}

void WLWindowFace::DoDrag(POINT const & point)
{
	if (!m_dragging)
		return ;

	SetWndLessPos(point.x - m_drag_point.x, point.y - m_drag_point.y, 0, 0, SWLP_X | SWLP_X_OFFSET | SWLP_Y | SWLP_Y_OFFSET) ;

	m_drag_point.x = point.x ;
	m_drag_point.y = point.y ;
}

void WLWindowFace::EndDrag()
{
	if (m_dragging)
	{
		m_dragging = FALSE ;
		ReleaseCapture() ;
	}
}