#pragma once

#include "common\windowless\WndLess.h"
#include "common\utils\CanvasDC.h"

class IFaceListener
{
public:
	virtual void OnFaceMove() = 0 ;
};

class WLWindowFace :
	public CWndLess
{
public:
	WLWindowFace(void);
	virtual ~WLWindowFace(void);

	CCanvasDC & GetFace() ;
	void SetListener(IFaceListener * listener) { m_listener = listener ; }

	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual void OnMoved(POINT const &ptOld, POINT const &ptNew) ;

protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

protected:
	void BeginDrag(POINT const & point) ;
	void DoDrag(POINT const & point) ;
	void EndDrag() ;

private:
	CCanvasDC m_face ;

	BOOL m_dragging ;
	POINT m_drag_point ;

	IFaceListener * m_listener ;

};
