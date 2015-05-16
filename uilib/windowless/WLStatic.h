#pragma once
#include "wndless.h"
#include "IWLStatic.h"
class WLStaticImp :
	public WLStaticImpBase
{
public:
	WLStaticImp(void);
	virtual ~WLStaticImp(void);
public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	COLORREF SetTextColor(COLORREF crTextColor) ;
	void SetFont(HFONT hFont) ;

	// 必须在创建后使用
	void SizeToContent() ;

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

public:
	virtual BOOL SetWindowText(LPCTSTR lpszText) ;

protected:
	COLORREF m_crTextColor ;
	HFONT m_hFont ;

};
