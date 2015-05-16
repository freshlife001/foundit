#include "StdAfx.h"

#include "uilib\common\utils\ImageEx\ImageEx.h"
#include "SearchResultList.h"
#include "logex.h"


//////////////////////////////////////////////////////////////////////////
// WLContactItem
const int DEFAULT_ICON_WIDTH = 20;
const int DEFAULT_ICON_HEIGHT = 20;

CSearchResultItem::CSearchResultItem()
: m_nMarginLeft(0), m_hIcon(NULL)
{
	m_bHoted = false;
	m_bActivated = true;
}

CSearchResultItem::~CSearchResultItem()
{

}

void CSearchResultItem::SetIcon(HICON icon)
{
	m_hIcon = icon; 
	UpdateView();
}

BOOL CSearchResultItem::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
	if(__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE;

	*pResult = 0;
	BOOL consumed = FALSE;
	switch(message)
	{
	case WM_MOUSEHOVER:
		m_bHoted = true;
		UpdateView();
		consumed = TRUE;
		break;
	case WM_MOUSELEAVE:
		m_bHoted = false;
		UpdateView();
		consumed = TRUE;
		break;
	}

	return consumed;
}

void CSearchResultItem::SetActivate(bool bActivate)
{
	if(m_bActivated == bActivate)
		return;

	m_bActivated = bActivate;
	UpdateView();
}

void CSearchResultItem::SetMarkImg(Shared_ImageEx pAffiMark)
{
	ASSERT(FALSE);
}

void CSearchResultItem::OnDraw(HDC hDC, RECT const &rcUpdate)
{
	CRect rcInHost;
	GetRectInHost(rcInHost);
	int nYOffset = ( rcInHost.Height() - DEFAULT_ICON_HEIGHT ) / 2;
	int nXOffset = nYOffset + m_nMarginLeft;
	//if(m_bGroup)	nXOffset += (6 + nYOffset);

	{
		Graphics g(hDC);
		Image* pImgBkgnd = NULL;
		if (IsSelected())
		{
			pImgBkgnd = m_pImgSel;
		}
		else if (m_bHoted)
		{
			pImgBkgnd = m_pImgHot;
		}
		if (pImgBkgnd != NULL && pImgBkgnd->GetLastStatus() == Ok)
		{
			int nWidth = pImgBkgnd->GetWidth() / 3;
			int nHeight = pImgBkgnd->GetHeight();
			g.DrawImage(pImgBkgnd, Rect(rcInHost.left, rcInHost.top, nWidth, rcInHost.Height()), 0, 0, nWidth, nHeight, UnitPixel);
			g.DrawImage(
				pImgBkgnd,
				Rect(rcInHost.left + nWidth, rcInHost.top, rcInHost.Width() - 2 * nWidth, rcInHost.Height()),
				nWidth, 0, nWidth, nHeight,
				UnitPixel
				);
			g.DrawImage(
				pImgBkgnd,
				Rect(rcInHost.right - nWidth, rcInHost.top, nWidth, rcInHost.Height()),
				nWidth * 2, 0, nWidth, nHeight,
				UnitPixel
				);
		}

		ImageAttributes ia;
		if (false)
		{
			ColorMatrix cm =
			{
				(Gdiplus::REAL)0.30, (Gdiplus::REAL)0.30, (Gdiplus::REAL)0.30, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00,
				(Gdiplus::REAL)0.59, (Gdiplus::REAL)0.59, (Gdiplus::REAL)0.59, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00,
				(Gdiplus::REAL)0.11, (Gdiplus::REAL)0.11, (Gdiplus::REAL)0.11, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00,
				(Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00, (Gdiplus::REAL)1.00, (Gdiplus::REAL)0.00,
				(Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00, (Gdiplus::REAL)0.00, (Gdiplus::REAL)1.00,
			} ;
			ia.SetColorMatrix(&cm);
		}
		if (m_hIcon)
		{
			::DrawIconEx(hDC, rcInHost.left + nXOffset,  rcInHost.top + nYOffset, m_hIcon, 20, 20, 0, 0, DI_NORMAL) ;
		}

	}

	// 绘制文字
	CDC* pDC = CDC::FromHandle(hDC);
	CRect rcLabel = rcInHost;
	const CString& name = GetText();
	rcLabel.left += nXOffset + DEFAULT_ICON_WIDTH + nYOffset;
	if(m_bActivated == false)	rcLabel.right -= 18;
	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(name, rcLabel, DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->SetBkMode(nOldBkMode);
}

void CSearchResultItem::SetBkgndImages( Shared_ImageEx pImgHot, Shared_ImageEx pImgSel )
{
	m_pImgHot = pImgHot;
	m_pImgSel = pImgSel;
}



#define ROW_HEIGHT 24
#define UPDATE_VISIBLE_RANGE_TIMER 10

CSearchResultList::CSearchResultList(void) : m_cpx(0),m_cpy(0), m_vpw(0),m_vph(0)
{
}

CSearchResultList::~CSearchResultList(void)
{
}

void CSearchResultList::Init()
{
	SetCanVertScroll(true);
	SetHasVertScrollBar(true);
	SetCanHorzScroll(true);
	SetHasHorzScrollBar(true);
	SetVertical(true);
	SetColumnWidth(-1);
	SetRowHeight(ROW_HEIGHT);

	m_pImgHot = Shared_ImageEx::FromId( IDI_SEARCH_ITEM_HOVER_PNG ) ;
	m_pImgSel = Shared_ImageEx::FromId( IDI_SEARCH_ITEM_SEL_PNG ) ;

	SetScrollListener(this);

}



int CSearchResultList::Insert(LPCTSTR text)
{
	CSearchResultItem * item = new CSearchResultItem();
	item->SetText(text);
	item->SetBkgndImages(m_pImgHot, m_pImgSel);
	int index = InsertItem(-1, item);
	return index;
}
bool CSearchResultList::Delete(unsigned int index)
{
	return DeleteItem(index);
}
void CSearchResultList::DeleteAll()
{
	DeleteAllItems();
	return;
}

LPCTSTR CSearchResultList::GetItemText(unsigned int index)
{
	CSearchResultItem * item = dynamic_cast<CSearchResultItem * >(GetItem(index));
	if (item)
	{
		return item->GetText();
	}
	return _T("");
}

void CSearchResultList::OnDraw(HDC hDC, RECT const &rcUpdate)
{
	Graphics gph(hDC) ;

	//绘制半透明的背景
	CRect rcClient ;
	GetRectInHost(rcClient) ;
	SolidBrush sb(Color(150, 255, 255, 255)) ;
	gph.FillRectangle(&sb, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height() ) ;

	__super::OnDraw(hDC, rcUpdate);

}


void CSearchResultList::OnContentPositionXChanged(int /*old_x*/, int new_x)
{
	m_cpx = new_x;

}
void CSearchResultList::OnContentPositionYChanged(int old_y, int new_y)
{
	m_cpy = new_y;

	if (old_y <= 0 && new_y <= 0)
	{
		if (old_y/ROW_HEIGHT != new_y/ROW_HEIGHT)
		{
			SetTimer(UPDATE_VISIBLE_RANGE_TIMER, 50, NULL) ;
		}
	}
}
void CSearchResultList::OnViewportWidthChanged(int /*old_width*/, int new_width)
{
	m_vpw = new_width;
}
void CSearchResultList::OnViewportHeightChanged(int old_height, int new_height)
{
	m_vph = new_height;
	if (new_height > 0)
	{
		if (old_height/ROW_HEIGHT != new_height/ROW_HEIGHT)
		{
			SetTimer(UPDATE_VISIBLE_RANGE_TIMER, 50, NULL) ;
		}
	}
}

void CSearchResultList::OnVisibleItemChanges()
{
	unsigned int total = (unsigned int)GetItemCount();

	if (m_cpy <=0 && m_vph >0 && IsWndLessVisible() && total>0)
	{
		NMVISIBLERANGE vr ;
		vr.hdr.hWlFrom = this ;
		vr.hdr.idFrom = GetID() ;
		vr.hdr.code = SRLN_VISIBLE_ITEM_CHANGE ;
		vr.fromIndex = -m_cpy/ROW_HEIGHT;
		vr.toIndex = (-m_cpy + m_vph)/ROW_HEIGHT;

		if (vr.toIndex  >= total && total > 0)
		{
			vr.toIndex  = total - 1;
		}

		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&vr) ;

	}

	KillTimer(UPDATE_VISIBLE_RANGE_TIMER);


}

CSearchResultItem *CSearchResultList::SearchVisibleItem(LPCTSTR fullpath)
{
	CSearchResultItem *ret = NULL;

	unsigned int total = (unsigned int)GetItemCount();

	if (m_cpy <=0 && m_vph >0 && IsWndLessVisible())
	{
		unsigned int visible_from =  -m_cpy/ROW_HEIGHT;
		unsigned int visible_to = (-m_cpy + m_vph)/ROW_HEIGHT + 1;
		
		if (visible_to >= total && total > 0)
		{
			visible_to = total - 1;
		}

		CString key = fullpath;
		for (unsigned int index = visible_from ; index <=  visible_to; index ++)
		{
			IWLSimpleListItem * item = GetItem(index);
			
			if (key.CompareNoCase(item->GetText() ) == 0 )
			{
				ret = dynamic_cast<CSearchResultItem*>(item);
				break;
			}
		}

	}

	if (ret == NULL)
	{
			LOG_DEBUG_C("not found " << fullpath);
	}

	return ret;

}

BOOL CSearchResultList::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{

	case WM_TIMER :
		{
			if (UPDATE_VISIBLE_RANGE_TIMER == wParam)
			{
				OnVisibleItemChanges();
			}
		}
		break ;
	}

	return bRes ;
}

