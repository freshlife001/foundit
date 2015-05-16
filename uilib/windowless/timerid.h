#pragma once

#include <set>

class TimerIdAlloter
{
public:
    // 左闭右开区间
    // lower_limit/upper_limit/error三个值必须满足以下三个条件:
    // 1. lower_limit < upper_limit 
    // 2. error < lower_limit
    // 3. error >= upper_limit
    TimerIdAlloter(UINT_PTR lower_limit, UINT_PTR upper_limit) 
        : m_lower_limit(lower_limit), m_upper_limit(upper_limit), m_next_free_id(lower_limit)
    {}

    bool AllocateId(UINT_PTR & id)
    {
        if (!m_free_id_container.empty())
        {
            std::set<UINT_PTR>::iterator iter = m_free_id_container.begin() ;
            id = *iter ;
            m_free_id_container.erase(iter) ;
            return true ;
        }
        else
        {
            if (m_next_free_id >= m_upper_limit)
                return false ;

            id = m_next_free_id ++ ;
            return true ;
        }
    }

    void FreeId(UINT_PTR id)
    {
        // error
        if (id < m_lower_limit || id >= m_upper_limit)
            return ;

        if (m_next_free_id > m_lower_limit && id == m_next_free_id - 1)
        {
            -- m_next_free_id ;
        }
        else
        {
            m_free_id_container.erase(id) ;
        }
    }

private:
    UINT_PTR m_lower_limit ;
    UINT_PTR m_upper_limit ;

    UINT_PTR m_next_free_id ;
    std::set<UINT_PTR> m_free_id_container ;

};


class IWndLess ;

class WndLessTimerId
{
public:
    WndLessTimerId(IWndLess * wndless, UINT_PTR id) 
        : m_wndless(wndless), m_id(id)
    {}

    bool operator < (WndLessTimerId const &rhs) const
    {
        if (m_wndless < rhs.m_wndless)
            return true ;

        if (m_wndless == rhs.m_wndless)
            return m_id < rhs.m_id ;

        return false ;
    }

    IWndLess * m_wndless ;
    UINT_PTR m_id ;

};
