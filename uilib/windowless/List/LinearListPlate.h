//////////////////////////////////////////////////////////////////////////
//
//	描述:   列表托盘，负责放置列表项，并根据列表项大小变化调整自身大小，
//          最终在列表上显示
//          一维列表托盘只有单行或者单列
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

#include <vector>
#include "uilib/wndless/IWLScrollWndPlate.h"
#include "uilib/wndless/IWLListItem.h"

class LinearListPlate: public CWndLessShell<IWLScrollWndPlate>, public IWndLess::DisplayListener
{
public:
    LinearListPlate() ;
    virtual ~LinearListPlate() ;

    enum Orientation
    {
        Horizontal = 0, Vertical,
    };

public:
    // 设置方向
    void SetOrientation(Orientation ori) ;

    // 列表方向为垂直时，width为-1表示列宽跟托盘宽度保持一直
    // 列表方向为水平时，表示元素默认列宽
    void SetColumnWidth(int width) ;

    // 列表方向为水平时，height为-1表示列宽跟托盘高度保持一直
    // 列表方向为垂直时，表示元素默认行高
    void SetRowHeight(int height) ;

    // 设置元素固定跨幅
    void SetItemFixedSpan(bool fixed) { m_fixed_span = fixed ; }

    // 前插，index为-1表示插在最后，返回-1表示失败，
    // 这是一个普通的插入元素接口，不适合批量插入
    int InsertItem(int index, IWLSelectableItem * item) ;
    bool DeleteItem(int index) ;
	BOOL DeleteAllItems();
    IWLSelectableItem * GetItem(int index) ;

	int GetItemCount()const;

    // 使某个元素可见
    void EnsureVisible(int index) ;

    typedef bool (*SORTFUN)(IWLSelectableItem * lhs, IWLSelectableItem * rhs) ;
    void Sort(SORTFUN sort_fun) ;

    template<typename T>
    void Sort(T sort_fun) 
	{
		std::sort(m_item_list.begin(), m_item_list.end(), sort_fun) ;

		RecalcLayout() ;
	}

protected:
    //virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    //virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

    //////////////////////////////////////////////////////////////////////////
    // IWndLess Implement
    virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

    //////////////////////////////////////////////////////////////////////////
    // ListPlate Implement
    virtual void OnViewportWidthChanged(int old_width, int new_width) ;
    virtual void OnViewportHeightChanged(int old_height, int new_height) ;

    //////////////////////////////////////////////////////////////////////////
    // IWndLess::DisplayListener Implement
    virtual void OnWndLessSizeChanged(IWndLess * wnd, SIZE const &old_size, SIZE const &new_size) ;
    virtual void OnWndLessShowHideChanged(IWndLess * wnd, bool old_status, bool new_status) ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // Internal 
    bool CalcItemPositionByIndex(int index, POINT &pos) ;
    void GetItemDefaultSize(SIZE &size) ;
    void RecalcLayout() ;

private:

    typedef std::vector<IWLSelectableItem*> ITEMLIST ;
    ITEMLIST m_item_list ;

    Orientation m_orientation ;
    int m_column_width ;    // 列表方向为水平时，列宽为每个元素的初始默认宽度；为垂直时，列宽为每个元素的统一宽度
    int m_row_height ;      // 列表方向为水平时，行高为每个元素的统一宽度；为垂直时，列宽为每个元素的初始默认宽度

    bool m_fixed_span ;     // 固定跨幅

    SIZE m_viewport_size ;  // 视口大小

};
