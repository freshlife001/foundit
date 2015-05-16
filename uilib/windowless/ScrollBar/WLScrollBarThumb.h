//////////////////////////////////////////////////////////////////////////
//
//	描述:   无句柄滚动条滑块
//
//	
//	日期:   2011/02/10
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\wlbutton.h"

class WLScrollBarThumbImp :
	public WLButtonImp
{
public:
	WLScrollBarThumbImp(void);
	virtual ~WLScrollBarThumbImp(void);

	enum 
	{
		NM_BEGINDRAG = (NM_WLFIRST - 300U),
		NM_DRAGGING = (NM_WLFIRST - 301U),
		NM_ENDDRAG = (NM_WLFIRST - 302U),
	};

	typedef struct _tagNMDRAGINFO
	{
		NMWLHDR hdr ;
		POINT point ;
	}NMDRAGINFO, * LPNMDRAGINFO ;

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// Message Handler
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;
};


typedef CWLButtonShell<IWLButton, WLScrollBarThumbImp> WLScrollBarThumb;