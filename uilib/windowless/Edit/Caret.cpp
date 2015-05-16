#include "stdafx.h"
#include "wndless\IWndLess.h"

#include "Caret.h"
#include "common/MemoryLeakChecker.h"
Caret::Caret() 
:
m_initialized(false)
, m_wnd(NULL)
, m_x(0)
, m_y(0)
, m_show(false) 
, m_appeared(false) 
, m_hide_count(1)
, m_blink_timer_id(0x07c2)
{
    m_blink_time = ::GetCaretBlinkTime() ;
}

BOOL Caret::CreateCaret(IWndLess * wnd, int width, int height) 
{
    if (wnd == NULL || width <= 0 || height <= 0)
        return FALSE ;

    m_initialized = true ;
    m_wnd = wnd ;
    m_hide_count = 1 ;
    m_width = width ;
    m_height = height ;

    return TRUE ;
}

BOOL Caret::DestroyCaret() 
{
    if (m_initialized)
    {
        m_wnd->KillTimer(m_blink_timer_id) ;
        m_show = false ;
        Update() ;

        m_wnd = NULL ;
        m_hide_count = 1 ;
        m_initialized = false ;

        return TRUE ;
    }

    return FALSE ;
}

BOOL Caret::GetCaretPos(POINT &point) const 
{
    if (m_initialized)
    {
        point.x = m_x ;
        point.y = m_y ;
        return TRUE ;
    }

    return FALSE ;
}

BOOL Caret::SetCaretPos(int x, int y) 
{
    if (m_initialized)
    {
        if (m_x != x || m_y != y)
        {
            int old_x = m_x, old_y = m_y ;
            m_x = x ;
            m_y = y ;

            if (m_show)
            {
                // ¸üÐÂ
                RECT old_caret = {old_x, old_y, old_x + m_width, old_y + m_height} ;
                m_wnd->HostToClient(&old_caret) ;
                m_wnd->UpdateView(&old_caret) ;

                Update() ;
            }
        }

        return TRUE ;
    }

    return FALSE ;
}

BOOL Caret::ShowCaret() 
{
    if (m_initialized)
    {
        if (!m_show)
        {
//             -- m_hide_count ;
//             if (m_hide_count > 0)
//                 return FALSE ;

            m_show = true ;
            m_appeared = true ;

            if (0 == m_wnd->SetTimer(m_blink_timer_id, m_blink_time, NULL))
            {
                m_show = false ;
                return FALSE ;
            }

            Update() ;
        }

        return TRUE ;
    }

    return FALSE ;
}

BOOL Caret::HideCaret() 
{
    if (m_initialized)
    {
//         ++ m_hide_count ;
        if (m_show)
        {
            m_show = false ;
            m_wnd->KillTimer(m_blink_timer_id) ;
            Update() ;
        }

        return TRUE ;
    }

    return FALSE ;
}

UINT Caret::GetCaretBlinkTime() const 
{
    return m_blink_time ;
}

BOOL Caret::SetCaretBlinkTime(UINT milliseconds) 
{
    if (m_initialized)
    {
        m_blink_time = milliseconds ;
        return TRUE ;
    }

    return FALSE ;
}

void Caret::Draw(HDC hdc, RECT const &update) 
{
    RECT caret = {m_x, m_y, m_x + m_width, m_y + m_height} ;
    if (m_show && m_appeared && ::IntersectRect(&caret, &caret, &update))
    {
        ::InvertRect(hdc, &caret) ;
    }
}

BOOL Caret::OnTimer(UINT_PTR timer_id) 
{
    BOOL consumed = FALSE ;
    if (timer_id == m_blink_timer_id)
    {
        if (m_show)
        {
            m_appeared = !m_appeared ;
            Update() ;
        }

        consumed = TRUE ;
    }

    return consumed ;
}

void Caret::Update() 
{
    if (!m_initialized)
        return ;

    RECT caret = {m_x, m_y, m_x + m_width, m_y + m_height} ;
    m_wnd->HostToClient(&caret) ;

    m_wnd->UpdateView(&caret) ;
}
