//////////////////////////////////////////////////////////////////////////
//
//	描述:   下拉按钮，点下该按钮，会下拉一个菜单
//			目前下拉操作为父窗口提供，不合理，以后改
//
//	
//	日期:   2011/02/20
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\windowless\wndless.h"

#include "common/wnds/Menu/hdmenu.h"

class WLDropDownBtn :
	public CWndLess
{
public:
	WLDropDownBtn(void);
	virtual ~WLDropDownBtn(void);

public:
	typedef struct _tagDDBCUSTOMDRAW
	{
		NMWLHDR hdr ;
		HDC hdc ;
		RECT rc ;
		DWORD dwStatus ;
	}DDBCUSTOMDRAW, *LPDDBCUSTOMDRAW ;

	typedef struct _tagDDBDROPDOWN
	{
		NMWLHDR hdr ;
		RECT rc ;
	}DDBDROPDOWN, * PDDBDROPDOWN ;

	typedef struct _tagDDBASKFORMENU
	{
		NMWLHDR hdr ;
		HHDMENU * pMenu ;
	}DDBASKFORMENU, *LPDDBASKFORMENU ;

	enum
	{
		DDBN_PREDROPDOWN	= NM_WLFIRST - 200U,
		DDBN_DROPDOWN		= NM_WLFIRST - 201U,
		DDBN_ASKFORMENU		= NM_WLFIRST - 202U,
		DDBN_POSTDROPDOWN	= NM_WLFIRST - 203U,
	};
	enum
	{
		BS_NORMAL	= 0x00,
		BS_HOT		= 0x01,
		BS_SELECTED	= 0x02,
	} ;

public:

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler

	void OnMouseMove(UINT nFlags, CPoint point) ;
	void OnMouseLeave() ;
	void OnLButtonDown(UINT nFlags, CPoint point) ;
// 	void OnLButtonUp(UINT nFlags, CPoint point) ;
// 	void OnCaptureChanged() ;

// Data
protected:
// 	BOOL m_bLDown ;
	DWORD m_dwStatus ;
// 	BOOL m_bPopMenu ;

};
