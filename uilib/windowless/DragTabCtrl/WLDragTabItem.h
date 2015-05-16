#pragma once
#include "windowless/TabCtrl/WLTabItem.h"
#include "common/utils/ImageEx/Shared_ImageEx.h"


class WLDragTabItem : public WLTabItem
{
	enum
	{
		CTRL_ID_CLOSE = 100,
	};
public:
	WLDragTabItem(  LPCTSTR lpszText, 
		UINT uTextStyle,
		CPoint	ptText,
		LPCTSTR sTipText,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		BOOL bDrag, int nTabState  );
	~WLDragTabItem();

	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent );

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);

	virtual void OnDestroy();

	virtual void OnSized(SIZE const &szOld, SIZE const &szNew);

	virtual void GetIconTextDrawRect( CRect& rcInHost );

	virtual void OnDraw( HDC hDC, RECT const &rcUpdate);

private:
	void DrawCloseButton( HDC hDC, RECT const& rcUpdate );
	BOOL HitTestClostButton( POINT const& point );

private:
	enum 
	{
		BS_NORMAL	= 0x00,
		BS_HOT		= 0x02,
		BS_SELECTED	= 0x08,
	};
	DWORD m_dwCloseBtnStatus;
	Shared_ImageEx m_imgClose;
	CRect m_rectCloseBtn;
	BOOL m_bLBtnDownInClose;
	BOOL m_bMbtnDown;
};
