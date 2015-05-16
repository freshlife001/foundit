#include "stdafx.h"

#include <algorithm>

#include "LinearListPlate.h"


LinearListPlate::LinearListPlate() 
:
m_orientation(Vertical)
, m_column_width(14)
, m_row_height(14)
, m_fixed_span(false)
{
    m_viewport_size.cx = 0 ;
    m_viewport_size.cy = 0 ;
}

LinearListPlate::~LinearListPlate() 
{
    // 清除数据
    ITEMLIST::iterator pos ;
    for (pos = m_item_list.begin() ; pos != m_item_list.end() ; ++ pos)
    {
        delete (*pos) ;
    }
    m_item_list.clear() ;
}

// 设置方向
void LinearListPlate::SetOrientation(Orientation ori) 
{
    m_orientation = ori ;
}

// 列表方向为垂直时，width为-1表示列宽跟托盘宽度保持一直
void LinearListPlate::SetColumnWidth(int width) 
{
    m_column_width = width ;
}

// 列表方向为水平时，height为-1表示列宽跟托盘高度保持一直
void LinearListPlate::SetRowHeight(int height) 
{
    m_row_height = height ;
}

int LinearListPlate::InsertItem(int index, IWLSelectableItem * item) 
{
    if (index == -1)
        index = m_item_list.size() ;

    if (index < 0 || index > (int)m_item_list.size())
        return -1 ;

    // 计算新元素的索引和位置大小
    item->SetIndex(index) ;
    m_item_list.insert(m_item_list.begin() + index, item) ;

	item->Create(0, 0, 0, 0, 0, this, -1) ;

    POINT item_pos ;
    SIZE item_size ;
    CalcItemPositionByIndex(index, item_pos) ;
    GetItemDefaultSize(item_size) ;

    if (m_fixed_span)
    {
        item->SetWndLessPos(item_pos.x, item_pos.y, item_size.cx, item_size.cy, SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY | SWLP_SHOW) ;
    }
    else
    {
        if (m_orientation == Vertical)
        {
            item->SetWndLessPos(item_pos.x, item_pos.y, item_size.cx, 0, SWLP_X | SWLP_Y | SWLP_CX | SWLP_SHOW) ;
        }
        else if (m_orientation == Horizontal)
        {
            item->SetWndLessPos(item_pos.x, item_pos.y, 0, item_size.cy, SWLP_X | SWLP_Y | SWLP_CY | SWLP_SHOW) ;
        }
	}
    //item->SetWndLessPos(item_pos.x, item_pos.y, item_size.cx, item_size.cy, SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY | SWLP_SHOW) ;

	// 不监测在这之前的item大小变化
	item->SetDisplayListener(this) ;
	item->GetSize(&item_size) ;

    // 更新index之后的元素索引和位置
    int new_index = index + 1 ;
    ITEMLIST::iterator pos = m_item_list.begin() + (index + 1) ;

    if (m_orientation == Vertical)
    {
        for ( ; pos != m_item_list.end() ; ++ pos, ++ new_index)
        {
            (*pos)->SetIndex(new_index) ;
            (*pos)->SetWndLessPos(0, item_size.cy, 0, 0, SWLP_Y_OFFSET) ;
        }

        int width = 0 ;
        UINT flags = 0 ;
        if (m_column_width != -1)
        {
            width = m_column_width ;
            flags = SWLP_CX ;
        }
        SetWndLessPos(0, 0, width, item_size.cy, SWLP_CY_OFFSET | SWLP_NOTIFYCHANGE | flags) ;
    }
    else if (m_orientation == Horizontal)
    {
        for ( ; pos != m_item_list.end() ; ++ pos, ++ new_index)
        {
            (*pos)->SetIndex(new_index) ;
            (*pos)->SetWndLessPos(item_size.cx, 0, 0, 0, SWLP_X_OFFSET) ;
        }

        int height = 0 ;
        UINT flags = 0 ;
        if (m_row_height != -1)
        {
            height = m_row_height ;
            flags = SWLP_CY ;
        }
        SetWndLessPos(0, 0, item_size.cx, height, SWLP_CX_OFFSET | SWLP_NOTIFYCHANGE | flags) ;
    }

    return index ;
}

bool LinearListPlate::DeleteItem(int index) 
{
    if (index < 0 || index >= (int)m_item_list.size())
        return false ;

    SIZE item_size ;
    IWndLess * item = m_item_list[index] ;
    item->GetSize(&item_size) ;
    item->Destroy() ;
    delete item ;
    m_item_list.erase(m_item_list.begin() + index) ;

    // 更新index之后的元素索引和位置
    int new_index = index ;
    ITEMLIST::iterator pos = m_item_list.begin() + index ;

    if (m_orientation == Vertical)
    {
        for ( ; pos != m_item_list.end() ; ++ pos, ++ new_index)
        {
            (*pos)->SetIndex(new_index) ;
            (*pos)->SetWndLessPos(0, -item_size.cy, 0, 0, SWLP_Y_OFFSET) ;
        }

        UINT flags = 0 ;
        if (m_column_width != -1 && m_item_list.empty())
             flags = SWLP_CX ;
		SetWndLessPos(0, 0, 0, -item_size.cy, SWLP_CY_OFFSET | SWLP_NOTIFYCHANGE | flags) ;
    }
    else if (m_orientation == Horizontal)
    {
        for ( ; pos != m_item_list.end() ; ++ pos, ++ new_index)
        {
            (*pos)->SetIndex(new_index) ;
            (*pos)->SetWndLessPos(-item_size.cx, 0, 0, 0, SWLP_X_OFFSET) ;
        }

        UINT flags = 0 ;
        if (m_row_height != -1 && m_item_list.empty())
            flags = SWLP_CY ;
        SetWndLessPos(0, 0, -item_size.cx, 0, SWLP_CX_OFFSET | SWLP_NOTIFYCHANGE | flags) ;
    }

    return true ;
}

BOOL LinearListPlate::DeleteAllItems()
{
	ITEMLIST::iterator it;
	for(it = m_item_list.begin(); it != m_item_list.end(); ++it)
	{
		IWndLess* item = *it;
		item->Destroy();
		delete item;
	}
	m_item_list.clear();

	if (m_orientation == Vertical)
	{
		UINT flags = 0 ;
		if (m_column_width != -1)
			flags = SWLP_CX ;
		SetWndLessPos(0, 0, 0, 0, SWLP_CY | SWLP_NOTIFYCHANGE | flags) ;
	}
	else if (m_orientation == Horizontal)
	{
		UINT flags = 0 ;
		if (m_row_height != -1)
			flags = SWLP_CY ;
		SetWndLessPos(0, 0, 0, 0, SWLP_CX | SWLP_NOTIFYCHANGE | flags) ;
	}

	return TRUE;
}

IWLSelectableItem * LinearListPlate::GetItem(int index) 
{
    if (index < 0 || index >= (int)m_item_list.size())
        return NULL ;

    return m_item_list[index] ;
}

// 使某个元素可见
void LinearListPlate::EnsureVisible(int index) 
{
    if (index < 0 || index >= (int)m_item_list.size())
        return ;

    CRect plate_bound ;
    GetRectInParent(&plate_bound) ;

    CRect item_bound_in_list ;
    m_item_list[index]->GetRectInParent(&item_bound_in_list) ;

    item_bound_in_list.OffsetRect(plate_bound.TopLeft()) ;

    if (m_orientation == Vertical)
    {
        if (item_bound_in_list.top < 0)
        {
            SetWndLessPos(0, -item_bound_in_list.top, 0, 0, SWLP_Y_OFFSET | SWLP_NOTIFYCHANGE) ;
        }
        else
        {
            if (item_bound_in_list.bottom > m_viewport_size.cy)
            {
                SetWndLessPos(0, m_viewport_size.cy - item_bound_in_list.bottom, 0, 0, SWLP_Y_OFFSET | SWLP_NOTIFYCHANGE) ;
            }
        }
    }
    else if (m_orientation == Horizontal)
    {
        if (item_bound_in_list.left < 0)
        {
            SetWndLessPos(-item_bound_in_list.left, 0, 0, 0, SWLP_X_OFFSET | SWLP_NOTIFYCHANGE) ;
        }
        else
        {
            if (item_bound_in_list.right > m_viewport_size.cx)
            {
                SetWndLessPos(m_viewport_size.cx - item_bound_in_list.right, 0, 0, 0, SWLP_X_OFFSET | SWLP_NOTIFYCHANGE) ;
            }
        }
    }
}

void LinearListPlate::Sort(SORTFUN sort_fun) 
{
    std::sort(m_item_list.begin(), m_item_list.end(), sort_fun) ;

    // 重新排列节点
    RecalcLayout() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

void LinearListPlate::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
    if (m_orientation == Vertical)
    {
        if (szNew.cx != szOld.cx && m_column_width == -1)
        {
            ITEMLIST::iterator pos ;
            for (pos = m_item_list.begin() ; pos != m_item_list.end() ; ++ pos)
            {
                (*pos)->SetWndLessPos(0, 0, szNew.cx, 0, SWLP_CX) ;
            }
        }

		if ( !m_fixed_span )
		{
			RecalcLayout();
		}
    }
    else if (m_orientation == Horizontal)
    {
        if (szNew.cy != szOld.cy && m_row_height == -1)
        {
            ITEMLIST::iterator pos ;
            for (pos = m_item_list.begin() ; pos != m_item_list.end() ; ++ pos)
            {
                (*pos)->SetWndLessPos(0, 0, 0, szNew.cy, SWLP_CY) ;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// ListPlate Implement

void LinearListPlate::OnViewportWidthChanged(int old_width, int new_width) 
{
    m_viewport_size.cx = new_width ;
    if (m_orientation == Vertical)
    {
        if (m_column_width == -1)
        {
            // 告诉ScrollWndEx自己大小变了
            SetWndLessPos(0, 0, new_width, 0, SWLP_CX | SWLP_NOTIFYCHANGE) ;
        }
    }
}

void LinearListPlate::OnViewportHeightChanged(int old_height, int new_height) 
{
    m_viewport_size.cy = new_height ;
    if (m_orientation == Horizontal)
    {
        if (m_row_height == -1)
        {
            // 告诉ScrollWndEx自己大小变了
            SetWndLessPos(0, 0, 0, new_height, SWLP_CY | SWLP_NOTIFYCHANGE) ;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// IWndLess::DisplayListener Implement

void LinearListPlate::OnWndLessSizeChanged(IWndLess * wnd, SIZE const &old_size, SIZE const &new_size) 
{
    IWLSelectableItem * item = dynamic_cast<IWLSelectableItem*>(wnd) ;
    if (item == NULL)
        return ;

    if (m_orientation == Vertical)
    {
        // 只响应高度变化
        int height_offset = new_size.cy - old_size.cy ;
        if (height_offset != 0)
        {
            int index = item->GetIndex() ;
            int i ;
            for (i = m_item_list.size() - 1 ; i > index ; -- i)
            {
                m_item_list[i]->SetWndLessPos(0, height_offset, 0, 0, SWLP_Y_OFFSET) ;
            }
        }

        SetWndLessPos(0, 0, 0, height_offset, SWLP_CY_OFFSET | SWLP_NOTIFYCHANGE) ;
    }
    else if (m_orientation == Horizontal)
    {
        // 只响应宽度变化
        int width_offset = new_size.cx - old_size.cx ;
        if (width_offset != 0)
        {
            int index = item->GetIndex() ;
            int i ;
            for (i = m_item_list.size() - 1 ; i > index ; -- i)
            {
                m_item_list[i]->SetWndLessPos(width_offset, 0, 0, 0, SWLP_X_OFFSET) ;
            }
        }

        SetWndLessPos(0, 0, width_offset, 0, SWLP_CX_OFFSET | SWLP_NOTIFYCHANGE) ;
    }
}

void LinearListPlate::OnWndLessShowHideChanged(IWndLess * wnd, bool old_status, bool new_status) 
{
    IWLSelectableItem * item = dynamic_cast<IWLSelectableItem*>(wnd) ;
    if (item == NULL)
        return ;

    SIZE item_size ;
    item->GetSize(&item_size) ;

    if (m_orientation == Vertical)
    {
        int height_offset = (new_status ? 1 : -1) * item_size.cy ;
        if (height_offset != 0)
        {
            int index = item->GetIndex() ;
            int i ;
            for (i = m_item_list.size() - 1 ; i > index ; -- i)
            {
                m_item_list[i]->SetWndLessPos(0, height_offset, 0, 0, SWLP_Y_OFFSET) ;
            }
        }
    }
    else if (m_orientation == Horizontal)
    {
        int width_offset = (new_status ? 1 : -1) * item_size.cx ;
        if (width_offset != 0)
        {
            int index = item->GetIndex() ;
            int i ;
            for (i = m_item_list.size() - 1 ; i > index ; -- i)
            {
                m_item_list[i]->SetWndLessPos(width_offset, 0, 0, 0, SWLP_X_OFFSET) ;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Internal 

bool LinearListPlate::CalcItemPositionByIndex(int index, POINT &pos) 
{
    if (index == -1)
        index = m_item_list.size() ;

    if (index < 0 || index > (int)m_item_list.size())
        return false ;

    SIZE item_size ;
    GetItemDefaultSize(item_size) ;
    if (m_orientation == Vertical)
    {
        pos.x = 0 ;
        pos.y = 0 ;

        if (m_fixed_span)
            pos.y = item_size.cy * index ;
        else
        {
            int i ;
            for (i = 0 ; i < index ; ++ i)
            {
                SIZE item_size ;
                m_item_list[i]->GetSize(&item_size) ;
                pos.y += item_size.cy ;
            }
        }
    }
    else if (m_orientation == Horizontal)
    {
        pos.x = 0 ;
        pos.y = 0 ;

        if (m_fixed_span)
            pos.x = item_size.cx * index ;
        else
        {
            int i ;
            for (i = 0 ; i < index ; ++ i)
            {
                SIZE item_size ;
                m_item_list[i]->GetSize(&item_size) ;
                pos.x += item_size.cx ;
            }
        }
    }
    else
    {
        ASSERT(FALSE) ;
        return false ;
    }

    return true ;
}

void LinearListPlate::GetItemDefaultSize(SIZE &size) 
{
    SIZE plate_size ;
    GetSize(&plate_size) ;

    if (m_orientation == Vertical)
    {
        size.cx = (m_column_width != -1) ? m_column_width : plate_size.cx ;
        size.cy = m_row_height ;
    }
    else if (m_orientation == Horizontal)
    {
        size.cx = m_column_width ;
        size.cy = (m_row_height != -1) ? m_row_height : plate_size.cy ;
    }
    else
    {
        ASSERT(FALSE) ;
    } 
}

void LinearListPlate::RecalcLayout() 
{
    // 重新排列节点
    ITEMLIST::iterator pos ;
    int i = 0 ;

    int x = 0, y = 0 ;
    SIZE item_size ;
    if (m_orientation == Vertical)
    {
        for (pos = m_item_list.begin() ; pos != m_item_list.end() ; ++ pos, ++ i)
        {
            IWLSelectableItem * item = (*pos) ;
            item->SetIndex(i) ;
            item->SetWndLessPos(0, y, 0, 0, SWLP_Y | SWLP_NOREDRAW) ;

			if ( item->IsWndLessVisible() )
			{ 
				item->GetSize(&item_size) ;
				y += item_size.cy ;
			}
        }
		SetWndLessPos( 0, 0, 0, y, SWLP_CY | SWLP_NOTIFYCHANGE );
    }
    else
    {
        for (pos = m_item_list.begin() ; pos != m_item_list.end() ; ++ pos, ++ i)
        {
            IWLSelectableItem * item = (*pos) ;
            item->SetIndex(i) ;

            item->SetWndLessPos(x, 0, 0, 0, SWLP_X | SWLP_NOREDRAW) ;
            item->GetSize(&item_size) ;
			if ( item->IsWndLessVisible() )
			{ 
				x += item_size.cx ;
			}
        }
		SetWndLessPos( 0, 0, x, 0, SWLP_CX | SWLP_NOTIFYCHANGE );
    }
}

int LinearListPlate::GetItemCount() const
{
	return m_item_list.size();
}


void LinearListPlate::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	return __super::OnDraw(hDC, rcUpdate);
}