//////////////////////////////////////////////////////////////////////////
//
//	描述:   一个简单的无句柄按钮实现
//
//	
//	日期:   2010/12/24
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "wndless.h"
#include "IWLButton.h"

class WLButtonImp : public WLButtonImpBase
{
public:
	WLButtonImp();
	virtual ~WLButtonImp(void);

public:

	int GetCheck() ;
	void SetCheck(int nCheck) ;
	BOOL DrawBySelf(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) ;

protected:
	BOOL CustomDraw(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) ;

public:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

protected:
	DWORD m_dwStatus ;
	BOOL m_bLButtonDown ;
};


