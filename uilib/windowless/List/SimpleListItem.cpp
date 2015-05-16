#include "StdAfx.h"
#include "SimpleListItem.h"

WLSimpleListItemImp::WLSimpleListItemImp(LPCTSTR lpszText)
:
m_text(lpszText)
, m_select_manager(NULL) 
, m_left_button_down(false)
, m_right_button_down(false)
, m_index(-1)
, m_selected(false) 
{
}

WLSimpleListItemImp::~WLSimpleListItemImp(void)
{
}

void WLSimpleListItemImp::Select() 
{
	//this->SetFocus();
    if (IsSelected())
        return ;

    //SetWndLessPos(0, 0, 0, 10, SWLP_CY_OFFSET | SWLP_NOTIFYCHANGE) ;

    m_selected = true ;
    UpdateView() ;
}

void WLSimpleListItemImp::Unselect() 
{
    if (!IsSelected())
        return ;

    //SetWndLessPos(0, 0, 0, -10, SWLP_CY_OFFSET | SWLP_NOTIFYCHANGE) ;

    m_selected = false ;
    UpdateView() ;
}

BOOL WLSimpleListItemImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
    *pResult = 0 ;
    BOOL consumed = FALSE ;

    switch (message)
    {
    case WM_LBUTTONDOWN :
        {
            if (m_select_manager != NULL)
            {
                m_select_manager->SelectItem(this, (UINT)wParam) ;
            }
            SetCapture() ;
            m_left_button_down = true ;
            consumed = TRUE ;
        }
        break ;

    case WM_LBUTTONUP :
        {
            if (m_left_button_down)
            {
                ReleaseCapture() ;

                // 判断当前鼠标是否在item上
                POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
                if (PtInWndLess(point))
                {
                    // 发送leftclick消息
                    if (m_select_manager != NULL)
                        m_select_manager->LeftClickItem(this) ;
                }
            }
            consumed = TRUE ;
        }
        break ;

    case WM_LBUTTONDBLCLK :
        {
            // 发送leftdoubleclick消息
            if (m_select_manager != NULL)
                m_select_manager->LeftDoubleClickItem(this) ;
        }
        break ;

    case WM_RBUTTONDOWN :
        {
            // 右键点击时，已选择则不需再选
            if (m_select_manager != NULL && !IsSelected())
            {
                m_select_manager->SelectItem(this, (UINT)wParam) ;
            }
            SetCapture() ;
            m_right_button_down = true ;
            consumed = TRUE ;
        }
        break ;

    case WM_RBUTTONUP :
        {
            if (m_right_button_down)
            {
                ReleaseCapture() ;

                // 判断当前鼠标是否在item上
                POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
                if (PtInWndLess(point))
                {
                    // 发送rightclick消息
                    if (m_select_manager != NULL)
                        m_select_manager->RightClickItem(this) ;
                }
            }
            consumed = TRUE ;
        }
        break ;

    case WM_RBUTTONDBLCLK :
        {
            // 发送rightdoubleclick消息
            if (m_select_manager != NULL)
                m_select_manager->RightDoubleClickItem(this) ;
        }
        break ;

    case WM_CAPTURECHANGED :
        {
            m_left_button_down = false ;
            m_right_button_down = false ;
        }
        break ;
    }

    return consumed ;
}

void WLSimpleListItemImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
    RECT bound_in_host ;
    GetRectInHost(&bound_in_host) ;

    HPEN pen = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0)) ;
    HGDIOBJ old_pen = ::SelectObject(hDC, pen) ;

    ::MoveToEx(hDC, bound_in_host.left, bound_in_host.top, NULL) ;
    ::LineTo(hDC, bound_in_host.right - 1, bound_in_host.top) ;
    ::LineTo(hDC, bound_in_host.right - 1, bound_in_host.bottom - 1) ;
    ::LineTo(hDC, bound_in_host.left, bound_in_host.bottom - 1) ;
    ::LineTo(hDC, bound_in_host.left, bound_in_host.top) ;

    ::SelectObject(hDC, old_pen) ;
    ::DeleteObject(pen) ;

    int old_bkmode = ::SetBkMode(hDC, TRANSPARENT) ;
    COLORREF old_text_color ;
    if (!IsSelected())
    {
        old_text_color = ::SetTextColor(hDC, RGB(0, 0, 0)) ;
    }
    else
    {
        old_text_color = ::SetTextColor(hDC, RGB(255, 0, 0)) ;
    }

    ::DrawText(hDC, m_text, m_text.GetLength(), &bound_in_host, DT_CENTER | DT_VCENTER | DT_SINGLELINE) ;

    ::SetTextColor(hDC, old_text_color) ;

    ::SetBkMode(hDC, old_bkmode) ;
}


template<>
UILIB_EXPORT WLSimpleListItemImpBase *CreateDefaultImp< WLSimpleListItemImpBase >(IWLSimpleListItem * wrapper)

{
	WLSimpleListItemImp *ret = new WLSimpleListItemImp(L"");
	ret->SetWrapper(wrapper);
	return ret;
};