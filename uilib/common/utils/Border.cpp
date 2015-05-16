#include "stdafx.h"
#include "Border.h"


//////////////////////////////////////////////////////////////////////////
// IBorder Interface

RectangleBorder::RectangleBorder() 
: m_ref_count(0), m_color(RGB(0, 0, 0))
{
    ::SetRect(&m_inset, 1, 1, 1, 1) ;
}

//////////////////////////////////////////////////////////////////////////
// IUnknown Interface

HRESULT RectangleBorder::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) 
        || IsEqualIID(riid, IID_IBorder)) 
    {
        AddRef();
        *ppvObject = (IBorder *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG RectangleBorder::AddRef(void)
{
    return ++ m_ref_count ;
}

ULONG RectangleBorder::Release(void)
{
    ULONG result = -- m_ref_count ;

    if (m_ref_count == 0)
    {
        delete this ;
    }

    return result ;
}

// void RectangleBorder::SetBound(int x, int y, int width, int height) 
// {
//     ::SetRect(&m_bound, x, y, x + width, y + height) ;
// }

void RectangleBorder::SetPos(int x, int y) 
{
    m_position.x = x ;
    m_position.y = y ;
}

void RectangleBorder::SetSize(int width, int height) 
{
    m_size.cx = width ;
    m_size.cy = height ;
}

// void RectangleBorder::GetBound(RECT &bound) 
// {
//     bound = m_bound ;
// }

void RectangleBorder::SetInset(int left, int top, int right, int bottom) 
{
    if (left < 0 || top < 0 || right < 0 || bottom < 0)
    {
        ASSERT(FALSE) ;
        return ;
    }
    ::SetRect(&m_inset, left, top, right, bottom) ;
}

// void RectangleBorder::GetInset(RECT &inset) 
// {
//     inset = m_inset ;
// }

void RectangleBorder::Draw(HDC hdc, RECT const &update) 
{
    COLORREF old_color = ::SetBkColor(hdc, m_color) ;

    // 四个矩形和update的交集
    RECT left_side = {m_position.x, m_position.y + m_inset.top, m_position.x + m_inset.left, m_position.y + m_size.cy} ;
    if (IntersectRect(&left_side, &left_side, &update))
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &left_side, NULL, 0, NULL) ;

    RECT top_side = {m_position.x, m_position.y, m_position.x + m_size.cx - m_inset.right, m_position.y + m_inset.top} ;
    if (IntersectRect(&top_side, &top_side, &update))
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &top_side, NULL, 0, NULL) ;

    RECT right_side = {m_position.x + m_size.cx - m_inset.right, m_position.y, m_position.x + m_size.cx, m_position.y + m_size.cy - m_inset.bottom} ;
    if (IntersectRect(&right_side, &right_side, &update))
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &right_side, NULL, 0, NULL) ;

    RECT bottom_side = {m_position.x + m_inset.left, m_position.y + m_size.cy - m_inset.bottom, m_position.x + m_size.cx, m_position.y + m_size.cy} ;
    if (IntersectRect(&bottom_side, &bottom_side, &update))
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &bottom_side, NULL, 0, NULL) ;

    ::SetBkColor(hdc, old_color) ;
}

//////////////////////////////////////////////////////////////////////////
// RectangleBorder Interface

COLORREF RectangleBorder::SetColor(COLORREF color) 
{
    COLORREF old_color = m_color ;
    m_color = color ;
    return old_color ;
}

COLORREF RectangleBorder::GetColor() const 
{
    return m_color ;
}

HRESULT CreateBorder(IID const &iid, IUnknown ** ppUnknown) 
{
    HRESULT hr = E_FAIL ;
    *ppUnknown = NULL ;

    if (iid == IID_IRectangleBorder)
    {
        RectangleBorder * border = new RectangleBorder ;
        border->AddRef() ;
        *ppUnknown = border ;

        hr = S_OK ;
    }

    return hr ;
}