#include "stdafx.h"

#include "ScrollWndEx.h"


WLScrollWndExImp::WLScrollWndExImp() 
: m_plate(NULL)
, m_lock_plate_size_change(false)
, m_scroll_listener(NULL)
{

}

WLScrollWndExImp::~WLScrollWndExImp() 
{
}

void WLScrollWndExImp::SetPlate(IWLScrollWndPlate * plate) 
{
    if (m_plate == plate)
        return ;

    m_plate = plate ;
}

void WLScrollWndExImp::LockPlateSizeChange() 
{
    m_lock_plate_size_change = true ;
}

void WLScrollWndExImp::UnlockPlateSizeChange() 
{
    if (m_lock_plate_size_change)
    {
        m_lock_plate_size_change = false ;

        SIZE plate_size ;
        m_plate->GetSize(&plate_size) ;
        SetContentSize(plate_size.cx, plate_size.cy) ;
    }
}

BOOL WLScrollWndExImp::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
    if (!m_plate)
        return FALSE ;

    if (!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
        return FALSE ;

    if (!m_plate->Create(WS_VISIBLE, 0, 0, 0, 0, this, -1))
        return FALSE ;

    m_plate->SetDisplayListener(this) ;
    m_plate->SetZorder(HWL_TOP) ;

    return TRUE ;
}

void WLScrollWndExImp::_InternalOnDraw(HDC hDC, RECT const &rcUpdate) 
{
    // 限制绘制范围
    //SIZE view_port_size = GetViewPortSize() ;
    HRGN hOldClipRgn = CreateRectRgn(0, 0, 0, 0) ;
    BOOL bNoClipNow = (0 == ::GetClipRgn(hDC, hOldClipRgn)) ;

    RECT view_port_bound ;
    GetRectInHost(&view_port_bound) ;
    //view_port_bound.right = view_port_bound.left + view_port_size.cx ;
    //view_port_bound.bottom = view_port_bound.top + view_port_size.cy ;

    HRGN hClipRgn = ::CreateRectRgnIndirect(&view_port_bound) ;
    ::ExtSelectClipRgn(hDC, hClipRgn, RGN_AND) ;
    ::DeleteObject(hClipRgn) ;

    __super::_InternalOnDraw(hDC, rcUpdate) ;

    // 取消限制范围
    if (bNoClipNow)
        ::SelectClipRgn(hDC, NULL) ;
    else
        ::SelectClipRgn(hDC, hOldClipRgn) ;
    ::DeleteObject(hOldClipRgn) ;
}

//////////////////////////////////////////////////////////////////////////
// DisplayListener Implement

void WLScrollWndExImp::OnWndLessSizeChanged(IWndLess * wnd, SIZE const &old_size, SIZE const &new_size) 
{
    if (m_lock_plate_size_change)
        return ;

    if (wnd == m_plate)
    {
        SetContentSize(new_size.cx, new_size.cy) ;
    }
}

void WLScrollWndExImp::OnWndLessPositionChanged(IWndLess * wnd, POINT const &old_pos, POINT const &new_pos) 
{
    if (wnd == m_plate)
    {
        SetContentPosition(new_pos.x, new_pos.y) ;
    }
}

//////////////////////////////////////////////////////////////////////////
// ScrollWnd Implement

void WLScrollWndExImp::OnContentPositionXChanged(int old_x, int new_x) 
{
    if (m_plate != NULL)
    {
        m_plate->SetWndLessPos(new_x, 0, 0, 0, SWLP_X | SWLP_NOREDRAW) ;
        UpdateView() ;
    }

	if (m_scroll_listener)
	{
		m_scroll_listener->OnContentPositionXChanged(old_x, new_x) ;
	}
}

void WLScrollWndExImp::OnContentPositionYChanged(int old_y, int new_y) 
{
    if (m_plate != NULL)
    {
        m_plate->SetWndLessPos(0, new_y, 0, 0, SWLP_Y | SWLP_NOREDRAW) ;
        UpdateView() ;
    }

	if (m_scroll_listener)
	{
		m_scroll_listener->OnContentPositionYChanged(old_y, new_y) ;
	}
}

void WLScrollWndExImp::OnViewportWidthChanged(int old_width, int new_width) 
{
    if (m_plate != NULL)
    {
        m_plate->OnViewportWidthChanged(old_width, new_width) ;
    }

	if (m_scroll_listener)
	{
		m_scroll_listener->OnViewportWidthChanged(old_width, new_width) ;
	}
}

void WLScrollWndExImp::OnViewportHeightChanged(int old_height, int new_height) 
{
    if (m_plate != NULL)
    {
        m_plate->OnViewportHeightChanged(old_height, new_height) ;
    }
	if (m_scroll_listener)
	{
		m_scroll_listener->OnViewportHeightChanged(old_height, new_height) ;
	}
}


template<>
UILIB_EXPORT WLScrollWndImpBase *CreateDefaultImp< WLScrollWndImpBase >(IWLScrollWnd * wrapper)

{
	WLScrollWndExImp *ret = new WLScrollWndExImp();
	ret->SetWrapper(wrapper);
	return ret;
};