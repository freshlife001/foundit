#include "StdAfx.h"
#include "ScrollWnd.h"
#include "common/utils/ImageEx/ImageResource.h"

WLScrollWndImp::WLScrollWndImp(void)
:
m_can_vert_scroll(true)
, m_has_vert_scroll_bar(true)
, m_can_horz_scroll(true) 
, m_has_horz_scroll_bar(true)
, m_main_scroll_mode(Vertical)
{
    m_content_position.x = 0 ;
    m_content_position.y = 0 ;

    m_content_size.cx = 0 ;
    m_content_size.cy = 0 ;

    m_viewport_size.cx = 0 ;
    m_viewport_size.cy = 0 ;
}

WLScrollWndImp::~WLScrollWndImp(void)
{
}

BOOL WLScrollWndImp::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
    m_vert_scroll_bar.SetImage(IDI_SCROLLBAR_PNG) ;
    if (!m_vert_scroll_bar.Create(
        WS_VISIBLE | WLScrollBar::STYLE_RIGHTALIGN | WLScrollBar::STYLE_VERT,
        0, 0, cx, cy,
        this, (UINT)-1))
    {
        return FALSE ;
    }
    m_scroll_width = m_vert_scroll_bar.GetScrollWidth() ;

    m_horz_scroll_bar.SetImage(IDI_SCROLLBAR_PNG) ;
    if (!m_horz_scroll_bar.Create(
        WS_VISIBLE | WLScrollBar::STYLE_BOTTOMALIGN | WLScrollBar::STYLE_HORZ,
        0, 0, cx, cy,
        this, (UINT)-1))
    {
        return FALSE ;
    }

    SCROLLINFO si ;
    si.fMask = SIF_ALL ;
    si.nMin = 0 ;
    si.nMax = 0 ;
    si.nPos = 0 ;
    si.nPage = 0 ;
	if (!HasVertScrollBar() && !CanVertScroll())
	{
		si.nMin = 1;
	}
	m_vert_scroll_bar.SetScrollInfo(&si) ;

	si.nMin = 0;
	if (!HasHorzScrollBar() && !CanHorzScroll())
	{
		si.nMin = 1;
	}
    m_horz_scroll_bar.SetScrollInfo(&si) ;

    return TRUE ;
}

BOOL WLScrollWndImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
    *pResult = 0 ;
    BOOL consumed = FALSE ;

    switch (message)
    {
    case WM_SCROLLBAR_SHOW :
        {
            OnScrollShow(wParam, (WLScrollBar*)lParam) ;
            consumed = TRUE ;
        }
        break ;

    case WM_VSCROLL :
        {
            if (!CanVertScroll())
            {
                //ASSERT(FALSE) ;
            }
            else
            {
                OnVScroll((LPSCROLLMSGINFO)wParam, (WLScrollBar*)lParam) ;
            }
            consumed = TRUE ;
        }
        break ;

    case WM_HSCROLL :
        {
            if (!CanHorzScroll())
            {
                //ASSERT(FALSE) ;
            }
            else
            {
                OnHScroll((LPSCROLLMSGINFO)wParam, (WLScrollBar*)lParam) ;
            }
            consumed = TRUE ;
        }
        break ;

    case WM_MOUSEWHEEL :
        {
            OnMouseWheel(LOWORD(wParam), HIWORD(wParam), CPoint(lParam)) ;
            consumed = TRUE ;
        }
        break ;
    }

    return consumed ;
}

void WLScrollWndImp::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
    // 先简单处理
    if (szOld.cx != szNew.cx)
    {
        m_horz_scroll_bar.SetWndLessPos(0, 0, szNew.cx - szOld.cx, 0, SWLP_CX_OFFSET) ;
        m_vert_scroll_bar.SetWndLessPos(szNew.cx - m_scroll_width, 0, 0, 0, SWLP_X) ;
    }

    if (szOld.cy != szNew.cy)
    {
        m_horz_scroll_bar.SetWndLessPos(0, szNew.cy - m_scroll_width, 0, 0, SWLP_Y) ;
        m_vert_scroll_bar.SetWndLessPos(0, 0, 0, szNew.cy - szOld.cy, SWLP_CY_OFFSET) ;
    }    

    // 再根据显示区域(viewport)调整滚动条
    if (szNew.cx != szOld.cx)
    {
        int new_viewport_width = szNew.cx ;
        if (m_vert_scroll_bar.IsWndLessVisible())
            new_viewport_width -= m_scroll_width ;

        if (CanHorzScroll())
        {
            int old_pos = -m_content_position.x ;
            int new_pos = old_pos ;
            int page = new_viewport_width ;
            int max = m_content_size.cx ;

            if (m_content_size.cx <= szNew.cx)
            {
                new_pos = 0 ;
            }
            else
            {
                if (new_pos + page > max)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }
            }

            if (old_pos != new_pos)
            {
                m_content_position.x = -new_pos ;
                OnContentPositionXChanged(-old_pos, -new_pos) ;
            }

            if (HasHorzScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_PAGE | SIF_POS ;
                si.nPos = new_pos ;
                si.nPage = (UINT)page ;
                m_horz_scroll_bar.SetScrollInfo(&si) ;
            }
        }

        if (new_viewport_width != m_viewport_size.cx)
        {
            int old_width = m_viewport_size.cx ;
            m_viewport_size.cx = new_viewport_width ;
            OnViewportWidthChanged(old_width, m_viewport_size.cx) ;
        }
    }

    if (szNew.cy != szOld.cy)
    {
        int new_viewport_height = szNew.cy ;
        if (m_horz_scroll_bar.IsWndLessVisible())
            new_viewport_height -= m_scroll_width ;

        if (CanVertScroll())
        {
            int old_pos = -m_content_position.y ;
            int new_pos = old_pos ;
            int page = new_viewport_height ;
            int max = m_content_size.cy ;

            if (m_content_size.cy <= szNew.cy)
            {
                new_pos = 0 ;
            }
            else
            {
                if (new_pos + page > max)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }
            }

            if (old_pos != new_pos)
            {
                m_content_position.y = -new_pos ;
                OnContentPositionYChanged(-old_pos, -new_pos) ;
            }

            if (HasVertScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_PAGE | SIF_POS ;
                si.nPos = new_pos ;
                si.nPage = (UINT)page ;
                m_vert_scroll_bar.SetScrollInfo(&si) ;
            }
        }

        if (new_viewport_height != m_viewport_size.cy)
        {
            int old_height = m_viewport_size.cy ;
            m_viewport_size.cy = new_viewport_height ;
            OnViewportHeightChanged(old_height, m_viewport_size.cy) ;
        }
    }
}

void WLScrollWndImp::OnScrollShow(BOOL show, WLScrollBar* pScrollBar) 
{
    SIZE wnd_size ;
    GetSize(&wnd_size) ;
    if (pScrollBar == &m_horz_scroll_bar)
    {
        if (show)
        {
            m_viewport_size.cy = wnd_size.cy - m_scroll_width ;

            // 如果垂直滚动条是显示的
            if (m_vert_scroll_bar.IsWndLessVisible())
            {
                m_vert_scroll_bar.SetWndLessPos(0, 0, 0, wnd_size.cy - m_scroll_width, SWLP_CY) ;
                m_horz_scroll_bar.SetWndLessPos(0, 0, wnd_size.cx - m_scroll_width, 0, SWLP_CX) ;
            }
            else
            {
                m_horz_scroll_bar.SetWndLessPos(0, 0, wnd_size.cx, 0, SWLP_CX) ;
            }

            // 设置滚动条信息
            if (CanVertScroll() && HasVertScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_PAGE ;
                si.nPage = wnd_size.cy - m_scroll_width ;
                m_vert_scroll_bar.SetScrollInfo(&si) ;
            }

            // 通知显示区域变化
            OnViewportHeightChanged(wnd_size.cy, wnd_size.cy - m_scroll_width) ;
        }
        else
        {
            m_viewport_size.cy = wnd_size.cy ;

            // 如果水平滚动条是显示的
            if (m_vert_scroll_bar.IsWndLessVisible())
            {
                m_vert_scroll_bar.SetWndLessPos(0, 0, 0, wnd_size.cy, SWLP_CY) ;
            }

            // 设置滚动条信息
            if (CanVertScroll())
            {
                int old_pos = -m_content_position.y ;
                int new_pos = old_pos ;
                int page = m_viewport_size.cy ;
                int max = m_content_size.cy ;

                if (new_pos + page > max)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }

                if (old_pos != new_pos)
                {
                    m_content_position.y = - new_pos ;
                    OnContentPositionYChanged(-old_pos, -new_pos) ;
                }

                if (HasVertScrollBar())
                {
                    SCROLLINFO si ;
                    si.fMask = SIF_PAGE | SIF_POS ;
                    si.nPage = page ;
                    si.nPos = new_pos ;
                    m_vert_scroll_bar.SetScrollInfo(&si) ;
                }
            }

            // 通知显示区域变化
            OnViewportHeightChanged(wnd_size.cy - m_scroll_width, wnd_size.cy) ;
        }
    }
    else if (pScrollBar == &m_vert_scroll_bar)
    {
        if (show)
        {
            m_viewport_size.cx = wnd_size.cx - m_scroll_width ;

            // 修改滚动条大小
            if (m_horz_scroll_bar.IsWndLessVisible())
            {
                m_horz_scroll_bar.SetWndLessPos(0, 0, wnd_size.cx - m_scroll_width, 0, SWLP_CX) ;
                m_vert_scroll_bar.SetWndLessPos(0, 0, 0, wnd_size.cy - m_scroll_width, SWLP_CY) ;
            }
            else
            {
                m_vert_scroll_bar.SetWndLessPos(0, 0, 0, wnd_size.cy, SWLP_CY) ;
            }

            // 设置滚动条信息
            if (CanHorzScroll() && HasHorzScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_PAGE ;
                si.nPage = wnd_size.cx - m_scroll_width ;
                m_horz_scroll_bar.SetScrollInfo(&si) ;
            }

            // 通知显示区域变化
            OnViewportWidthChanged(wnd_size.cx, wnd_size.cx - m_scroll_width) ;
        }
        else
        {
            m_viewport_size.cx = wnd_size.cx ;

            // 如果水平滚动条是显示的
            if (m_horz_scroll_bar.IsWndLessVisible())
            {
                m_horz_scroll_bar.SetWndLessPos(0, 0, wnd_size.cx, 0, SWLP_CX) ;
            }

            // 设置滚动条信息
            if (CanHorzScroll())
            {
                int old_pos = -m_content_position.x ;
                int new_pos = old_pos ;
                int page = m_viewport_size.cx ;
                int max = m_content_size.cx ;

                if (new_pos + page > max)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }

                if (old_pos != new_pos)
                {
                    m_content_position.x = - new_pos ;
                    OnContentPositionXChanged(-old_pos, -new_pos) ;
                }

                if (HasHorzScrollBar())
                {
                    SCROLLINFO si ;
                    si.fMask = SIF_PAGE | SIF_POS ;
                    si.nPage = page ;
                    si.nPos = new_pos ;
                    m_horz_scroll_bar.SetScrollInfo(&si) ;
                }
            }

            // 通知显示区域变化
            OnViewportWidthChanged(wnd_size.cx - m_scroll_width, wnd_size.cx) ;
        }
    }
}

void WLScrollWndImp::OnVScroll(LPSCROLLMSGINFO pSmi, WLScrollBar* pScrollBar) 
{
    if (!CanVertScroll())
        return ;

    SIZE wnd_size ;
    GetSize(&wnd_size) ;

    int old_pos = -m_content_position.y ;
    int new_pos = old_pos ;
    int page = m_viewport_size.cy ;
    int max = m_content_size.cy ;

    UINT nSBCode = pSmi->nSBCode ;
    UINT nPos = pSmi->nPos ;
    switch (nSBCode)
    {
    case SB_LINEUP :
        {
            new_pos -= 25 ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_LINEDOWN :
        {
            new_pos += 25 ;
            if (new_pos > max - page)
                new_pos = max - page ;
        }
        break ;

    case SB_PAGEUP :
        {
            new_pos -= page ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_PAGEDOWN :
        {
            new_pos += page ;
            if (new_pos > max - page)
                new_pos = max - page ;
        }
        break ;

    case SB_WHEELUP :
        {
            new_pos -= (page * 2 / 5) ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_WHEELDOWN :
        {
            new_pos += (page * 2 / 5) ;
            if (new_pos > max - page)
                new_pos = max - page ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_THUMBPOSITION :
        {
            new_pos = (int)nPos ;
        }
        break ;

    case SB_THUMBTRACK :
        {
            new_pos = (int)nPos ;
        }
        break ;
    }

    if (HasVertScrollBar())
    {
        SCROLLINFO si ;
        si.fMask = SIF_POS ;
        si.nPos = new_pos ;
        m_vert_scroll_bar.SetScrollInfo(&si) ;
    }

    if (new_pos != old_pos)
    {
        m_content_position.y = -new_pos ;
        OnContentPositionYChanged(-old_pos, -new_pos) ;
    }
}

void WLScrollWndImp::OnHScroll(LPSCROLLMSGINFO pSmi, WLScrollBar* pScrollBar) 
{
    if (!CanHorzScroll())
        return ;

    SIZE wnd_size ;
    GetSize(&wnd_size) ;

    int old_pos = -m_content_position.x ;
    int new_pos = old_pos ;
    int page = m_viewport_size.cx ;
    int max = m_content_size.cx ;

    UINT nSBCode = pSmi->nSBCode ;
    UINT nPos = pSmi->nPos ;
    switch (nSBCode)
    {
    case SB_LINEUP :
        {
            new_pos -= 25 ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_LINEDOWN :
        {
            new_pos += 25 ;
            if (new_pos > max - page)
                new_pos = max - page ;
        }
        break ;

    case SB_PAGEUP :
        {
            new_pos -= page ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_PAGEDOWN :
        {
            new_pos += page ;
            if (new_pos > max - page)
                new_pos = max - page ;
        }
        break ;

    case SB_WHEELUP :
        {
            new_pos -= (page * 2 / 5) ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_WHEELDOWN :
        {
            new_pos += (page * 2 / 5) ;
            if (new_pos > max - page)
                new_pos = max - page ;
            if (new_pos < 0)
                new_pos = 0 ;
        }
        break ;

    case SB_THUMBPOSITION :
        {
            new_pos = (int)nPos ;
        }
        break ;

    case SB_THUMBTRACK :
        {
            new_pos = (int)nPos ;
        }
        break ;
    }

    if (HasHorzScrollBar())
    {
        SCROLLINFO si ;
        si.fMask = SIF_POS ;
        si.nPos = new_pos ;
        m_horz_scroll_bar.SetScrollInfo(&si) ;
    }

    if (new_pos != old_pos)
    {
        m_content_position.x = -new_pos ;
        OnContentPositionXChanged(-old_pos, -new_pos) ;
    }
}

BOOL WLScrollWndImp::OnMouseWheel(UINT nFlags, short zDelta, CPoint /*point*/) 
{
    SCROLLMSGINFO smi ;
    if (zDelta > 0)
        smi.nSBCode = SB_WHEELUP ;
    else
        smi.nSBCode = SB_WHEELDOWN ;

    // 按住shift键时滚动方向和主滚动模式相反
    bool press_shift = !!(nFlags & MK_SHIFT) ;
    bool vert_mode = (m_main_scroll_mode == Vertical) ;

    UINT message = !!(press_shift ^ vert_mode) ? WM_VSCROLL : WM_HSCROLL ;
    SendMessage(message, (WPARAM)&smi, NULL) ;

    return TRUE ;
}

void WLScrollWndImp::SetContentSize(int width, int height) 
{
    SIZE wnd_size ;
    GetSize(&wnd_size) ;

    if (m_content_size.cx != width)
    {
        m_content_size.cx = width ;

        if (CanHorzScroll())
        {
            int old_pos = -m_content_position.x ;
            int new_pos = old_pos ;
            int max = width ;
            int page = wnd_size.cx ;

            if (HasVertScrollBar() && m_vert_scroll_bar.IsWndLessVisible())
                page -= m_scroll_width ;

            if (max <= page)
            {
                new_pos = 0 ;
            }
            else
            {
                if (new_pos > max - page)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }
            }

            if (page != m_viewport_size.cx)
            {
                int old_width = m_viewport_size.cx ;
                m_viewport_size.cx = page ;
                OnViewportWidthChanged(old_width, m_viewport_size.cx) ;
            }

            if (new_pos != old_pos)
            {
                m_content_position.x = -new_pos ;
                OnContentPositionXChanged(-old_pos, -new_pos) ;
            }

            if (HasHorzScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE ;
                si.nMin = 0 ;
                si.nMax = max - 1 ;
                si.nPos = new_pos ;
                si.nPage = page ;
                m_horz_scroll_bar.SetScrollInfo(&si) ;
            }
        }
    }

    if (m_content_size.cy != height)
    {
        m_content_size.cy = height ;

        if (CanVertScroll())
        {
            int old_pos = -m_content_position.y ;
            int new_pos = old_pos ;
            int max = height ;
            int page = wnd_size.cy ;

            if (HasHorzScrollBar() && m_horz_scroll_bar.IsWndLessVisible())
                page -= m_scroll_width ;

            if (max <= page)
            {
                new_pos = 0 ;
            }
            else
            {
                if (new_pos > max - page)
                {
                    new_pos = max - page ;
                    if (new_pos < 0)
                        new_pos = 0 ;
                }
            }

            if (page != m_viewport_size.cy)
            {
                int old_height = m_viewport_size.cy ;
                m_viewport_size.cy = page ;
                OnViewportHeightChanged(old_height, m_viewport_size.cy) ;
            }

            if (new_pos != old_pos)
            {
                m_content_position.y = -new_pos ;
                OnContentPositionYChanged(-old_pos, -new_pos) ;
            }

            if (HasVertScrollBar())
            {
                SCROLLINFO si ;
                si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE ;
                si.nMin = 0 ;
                si.nMax = max - 1 ;
                si.nPos = new_pos ;
                si.nPage = page ;
                m_vert_scroll_bar.SetScrollInfo(&si) ;
            }
        }
    }
}

void WLScrollWndImp::SetContentPosition(int x, int y) 
{
    // 水平
    if (CanHorzScroll() && m_content_position.x != x)
    {
        m_content_position.x = x ;

        if (HasHorzScrollBar())
        {
            SCROLLINFO si ;
            si.fMask = SIF_POS ;
            si.nPos = -x ;
            m_horz_scroll_bar.SetScrollInfo(&si) ;
        }
    }

    // 垂直
    if (CanVertScroll() && m_content_position.y != y)
    {
        m_content_position.y = y ;

        if (HasVertScrollBar())
        {
            SCROLLINFO si ;
            si.fMask = SIF_POS ;
            si.nPos = -y ;
            m_vert_scroll_bar.SetScrollInfo(&si) ;
        }
    }
}
