//////////////////////////////////////////////////////////////////////////
//
//	描述:   列表控件
//
//	
//	日期:   2011/08/18
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "ScrollWnd.h"

class ScrollWndPlate ;

class WLScrollWndExImp : public WLScrollWndImp, IWndLess::DisplayListener
{
public:
    WLScrollWndExImp() ;
    virtual ~WLScrollWndExImp() ;

public:
    virtual void SetPlate(IWLScrollWndPlate * plate) ;

    virtual void LockPlateSizeChange() ;
    virtual void UnlockPlateSizeChange() ;

	virtual void SetScrollListener(ScrollListener * listener) { m_scroll_listener = listener;};

protected:
    //////////////////////////////////////////////////////////////////////////
    // IWndLess Implement
    virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // DisplayListener Implement
    virtual void OnWndLessSizeChanged(IWndLess * wnd, SIZE const &old_size, SIZE const &new_size) ;
    virtual void OnWndLessPositionChanged(IWndLess * wnd, POINT const &old_pos, POINT const &new_pos) ;

    //////////////////////////////////////////////////////////////////////////
    // ScrollWnd Implement
    virtual void OnContentPositionXChanged(int old_x, int new_x) ;
    virtual void OnContentPositionYChanged(int old_y, int new_y) ;
    virtual void OnViewportWidthChanged(int old_width, int new_width) ;
    virtual void OnViewportHeightChanged(int old_height, int new_height) ;

private:
    // 列表盘子，装列表项
    IWLScrollWndPlate * m_plate ;

    bool m_lock_plate_size_change ;
	ScrollListener * m_scroll_listener;

};
