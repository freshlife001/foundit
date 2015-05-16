//////////////////////////////////////////////////////////////////////////
//
//	描述:   滚动窗口，可以仅通过几个接口控制滚动条属性和响应滚动条事件
//
//  术语:   Content表示ScrollWnd中要展现的内容，Viewport表示展现的视口
//
//	
//	日期:   2011/08/15
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "uilib/wndless/IWLScrollWnd.h"
#include "windowless/ScrollBar/WLScrollBar.h"

class WLScrollWndImp :
    public WLScrollWndImpBase
{
public:
    WLScrollWndImp(void);
    virtual ~WLScrollWndImp(void);

public:
    // 在创建前设置
    // 设置是否能垂直滚动
    virtual void SetCanVertScroll(bool can) 
    {
        m_can_vert_scroll = can ;
        if (!can)
        {
            m_has_vert_scroll_bar = false ;
        }
    }

    // 设置是否有垂直滚动条
    virtual void SetHasVertScrollBar(bool has) { m_has_vert_scroll_bar = has ; }

    // 设置是否能水平滚动
    virtual void SetCanHorzScroll(bool can) 
    {
        m_can_horz_scroll = can ;
        if (!can)
        {
            m_has_horz_scroll_bar = false ;
        }
    }

    // 设置是否有水平滚动条
    virtual void SetHasHorzScrollBar(bool has) { m_has_horz_scroll_bar = has ; }

    // 设置主滚动模式：
    // 主滚动模式为垂直时，滚轮事件转换成垂直滚动条翻页事件，shift+滚轮事件转换成水平滚动条翻页事件；
    // 水平时相反。
    virtual void SetMainScrollMode(MainScrollMode msm) { m_main_scroll_mode = msm ; }

protected:
    virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
    virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

    void OnScrollShow(BOOL show, WLScrollBar* pScrollBar) ;
    void OnVScroll(LPSCROLLMSGINFO pSmi, WLScrollBar* pScrollBar) ;
    void OnHScroll(LPSCROLLMSGINFO pSmi, WLScrollBar* pScrollBar) ;
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point) ;

public:
//protected:
    void SetContentSize(int width, int height) ;
    SIZE GetContentSize() const { return m_content_size ; }

    SIZE GetViewportSize() const { return m_viewport_size ; }

    // x,y是以viewport左上角点为原点的坐标
    // 通过这个接口控制滚动条的位置
    void SetContentPosition(int x, int y) ;
    POINT GetContentPosition() const { return m_content_position ; }

protected:
    ///////////////////////////////////////////////////////////////////////////
    // 这些接口用于被重载
    virtual void OnContentPositionXChanged(int /*old_x*/, int /*new_x*/) {}
    virtual void OnContentPositionYChanged(int /*old_y*/, int /*new_y*/) {}
    virtual void OnViewportWidthChanged(int /*old_width*/, int /*new_width*/) {}
    virtual void OnViewportHeightChanged(int /*old_height*/, int /*new_height*/) {}

    virtual bool CanVertScroll() const { return m_can_vert_scroll ; }
    virtual bool HasVertScrollBar() const { return m_has_vert_scroll_bar ; }
    virtual bool CanHorzScroll() const { return m_can_horz_scroll ; }
    virtual bool HasHorzScrollBar() const { return m_has_horz_scroll_bar ; }

protected:
    // 子类中一般不要直接使用以下成员变量
    // 垂直滚动条
    WLScrollBar m_vert_scroll_bar ;

    // 水平滚动条
    WLScrollBar m_horz_scroll_bar ;

    int m_scroll_width ;

    POINT m_content_position ;
    SIZE m_content_size ;
    SIZE m_viewport_size ;

    bool m_can_vert_scroll ;
    bool m_has_vert_scroll_bar ;
    bool m_can_horz_scroll ;
    bool m_has_horz_scroll_bar ;

    MainScrollMode m_main_scroll_mode ;

};
