#include "StdAfx.h"

#include "uilib\common\utils\ImageEx\ImageEx.h"
#include "SearchHistoryList.h"
#include "logex.h"


//////////////////////////////////////////////////////////////////////////
// WLContactItem
const int DEFAULT_ICON_WIDTH = 20;
const int DEFAULT_ICON_HEIGHT = 20;

CSearchHistoryItem::CSearchHistoryItem()
: m_nMarginLeft(0)
{
	m_bHoted = false;
	m_bActivated = true;
}

CSearchHistoryItem::~CSearchHistoryItem()
{

}

BOOL CSearchHistoryItem::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
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

void CSearchHistoryItem::SetActivate(bool bActivate)
{
	if(m_bActivated == bActivate)
		return;

	m_bActivated = bActivate;
	UpdateView();
}

void CSearchHistoryItem::SetMarkImg(Shared_ImageEx pAffiMark)
{
	ASSERT(FALSE);
}

void CSearchHistoryItem::OnDraw(HDC hDC, RECT const &rcUpdate)
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
		if (!m_imgIcon.IsNull())
		{
			g.DrawImage(
				m_imgIcon,
				Rect(rcInHost.left + nXOffset, rcInHost.top + nYOffset, 20, 20),
				0, 0, m_imgIcon->GetWidth(), m_imgIcon->GetHeight(),
				UnitPixel, &ia
				);
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

void CSearchHistoryItem::SetBkgndImages( Shared_ImageEx pImgHot, Shared_ImageEx pImgSel )
{
	m_pImgHot = pImgHot;
	m_pImgSel = pImgSel;
}





CSearchHistoryList::CSearchHistoryList(void)
{
}

CSearchHistoryList::~CSearchHistoryList(void)
{
}

void CSearchHistoryList::Init()
{
	SetCanVertScroll(false);
	SetHasVertScrollBar(false);
	SetCanHorzScroll(false);
	SetHasHorzScrollBar(false);
	SetVertical(true);
	SetColumnWidth(-1);
	SetRowHeight(24);

	m_pImgHot = Shared_ImageEx::FromId( IDI_SEARCH_ITEM_HOVER_PNG ) ;
	m_pImgSel = Shared_ImageEx::FromId( IDI_SEARCH_ITEM_SEL_PNG ) ;

}

int CSearchHistoryList::Insert(LPCTSTR text)
{
	CSearchHistoryItem * item = new CSearchHistoryItem();
	item->SetText(text);
	item->SetBkgndImages(m_pImgHot, m_pImgSel);
	int index = InsertItem(-1, item);
	return index;
}
bool CSearchHistoryList::Delete(unsigned int index)
{
	return DeleteItem(index);
}
void CSearchHistoryList::DeleteAll()
{
	DeleteAllItems();
	return;
}
int CSearchHistoryList::SearchItem(LPCTSTR text)
{
	int ret = -1;
	int count = GetItemCount();
	if (count > 0)
	{
		CString key = text;
		for( int i = 0; i < count; i++)
		{
			CSearchHistoryItem * item = dynamic_cast<CSearchHistoryItem * >(GetItem(i));
			if (item)
			{
				if (0 == key.CompareNoCase(item->GetText() ) )
				{
					ret = i;
					break;
				}
			}
		}

		if (ret == -1)
		{
			GetSelectManager()->SelectItem(GetItem(0), 0);
			EnsureVisible(0);
		}
		else
		{
			GetSelectManager()->SelectItem(GetItem(ret), 0);
			EnsureVisible(ret);
		}
		
	}



	return ret;

}

LPCTSTR CSearchHistoryList::GetItemText(unsigned int index)
{
	CSearchHistoryItem * item = dynamic_cast<CSearchHistoryItem * >(GetItem(index));
	if (item)
	{
		return item->GetText();
	}
	return _T("");
}

void CSearchHistoryList::OnDraw(HDC hDC, RECT const &rcUpdate)
{
	Graphics gph(hDC) ;

	//绘制半透明的背景
	CRect rcClient ;
	GetRectInHost(rcClient) ;
	SolidBrush sb(Color(200, 255, 255, 255)) ;
	gph.FillRectangle(&sb, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height() ) ;

	__super::OnDraw(hDC, rcUpdate);

}
