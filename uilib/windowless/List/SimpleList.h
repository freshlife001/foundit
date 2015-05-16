//////////////////////////////////////////////////////////////////////////
//
//	描述:   组合列表、列表项托盘、选择管理器、边框
//
//	
//	日期:   2011/08/26
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLScrollWnd.h"
#include "uilib/wndless/IWLListItem.h"
#include "uilib/wndless/IWLSimpleList.h"

#include "LinearListPlate.h"


class WLSimpleListImp :
    public WLSimpleListImpBase, public ItemSelectManager
{
public:
    WLSimpleListImp(void);
    virtual ~WLSimpleListImp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    // 对外接口

    // 设置能否垂直滚动
    void SetCanVertScroll(bool can) ;

    // 设置是否有垂直滚动条
    void SetHasVertScrollBar(bool has) ;

    // 设置能否水平滚动
    void SetCanHorzScroll(bool can) ;

    // 设置是否有水平滚动条
    void SetHasHorzScrollBar(bool has) ;

    // 设置方向
    void SetVertical(bool vert) ;

    // 列表方向为垂直时，width为-1表示列宽跟托盘宽度保持一直
    // 列表方向为水平时，表示元素默认列宽
    void SetColumnWidth(int width) ;

    // 列表方向为水平时，height为-1表示列宽跟托盘高度保持一直
    // 列表方向为垂直时，表示元素默认行高
    void SetRowHeight(int height) ;

	void SetItemFixedSpan(bool fix)
	{
		m_plate.SetItemFixedSpan(fix);
	}

    // 前插，index为-1表示插在最后，返回-1表示失败，
    // 这是一个普通的插入元素接口，不适合批量插入
    int InsertItem(int index, IWLSimpleListItem * item) ;
    bool DeleteItem(int index) ;
	bool DeleteAllItems();
    IWLSimpleListItem * GetItem(int index) ;
	int GetItemCount() const { return m_plate.GetItemCount(); }
	virtual void GetSize(LPSIZE lpSize) const { return m_plate.GetSize(lpSize); }

    // 使某元素可见
    void EnsureVisible(int index) ;

    void SetEventListener(EventListener * listener) { m_event_listener = listener ; }
	void SetScrollListener(IWLScrollWnd::ScrollListener * listener) {m_list.SetScrollListener(listener) ;};

    //////////////////////////////////////////////////////////////////////////
    // ItemSelectManager Implement
    virtual void SetMultipleSelect(bool mutiple) { m_multiple_select = mutiple ; }
    virtual bool IsMultipleSelect() const { return m_multiple_select ; }

    virtual IWLSelectableItem * GetSelectedItem() const ;
    virtual int GetMultipleSelectedItem(ITEMLIST & selected_item_list) const ;

    // 排序
    typedef bool (*SORTFUN)(IWLSelectableItem * lhs, IWLSelectableItem * rhs) ;
    void Sort(SORTFUN sort_fun) ;

    template<typename T>
    void Sort(T sort_fun)
    {
        m_plate.Sort<T>(sort_fun) ;
        m_list.UpdateView() ;
    }

	virtual ItemSelectManager* GetSelectManager();

protected:
    // uFlags标记当前的按键、鼠标状态
    virtual void SelectItem(IWLSelectableItem * item, UINT uFlags) ;
    virtual void UnselectItem(IWLSelectableItem * item, UINT uFlags) ;
    virtual void UnselectAllItem() ;

    virtual void LeftClickItem(IWLSelectableItem * item) ;
    virtual void LeftDoubleClickItem(IWLSelectableItem * item) ;
    virtual void RightClickItem(IWLSelectableItem * item) ;
    virtual void RightDoubleClickItem(IWLSelectableItem * item) ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // IWndLess Implement
    virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
    virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
    WLScrollWnd m_list ;
    LinearListPlate m_plate ;

    // 边缘空白
    RECT m_frame_margin ;
    COLORREF m_frame_color ;

    bool m_multiple_select ;
    ITEMLIST m_selected_item_list ;
    IWLSelectableItem * m_last_selectd_item ;

    EventListener * m_event_listener ;

};
