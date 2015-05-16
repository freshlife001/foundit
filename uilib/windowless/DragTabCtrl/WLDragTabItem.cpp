#include "stdafx.h"
#include "WLDragTabItem.h"
#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"
#include "common/utils/Graphics.h"
#include "common/MemoryLeakChecker.h"
#include "IWLDragTabCtrl.h"

WLDragTabItem::WLDragTabItem(  LPCTSTR lpszText, 
							 UINT uTextStyle,
							 CPoint	ptText,
							 LPCTSTR sTipText,
							 Shared_ImageEx pImgIcon,
							 Shared_ImageEx pImgBkgnd,
							 BOOL bDrag,
							 int nTabState )
							 : WLTabItem( lpszText, uTextStyle, ptText, sTipText, pImgIcon, pImgBkgnd, bDrag, nTabState )
{
}

WLDragTabItem::~WLDragTabItem(void)
{
}

BOOL WLDragTabItem::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent )
{
	BOOL bCreate = __super::OnCreate( dwStyle, x, y, cx, cy, pParent );

	//ÉèÖÃÌí¼Ó°´Å¥
	m_imgClose = Shared_ImageEx::FromId( IDI_DRAG_TAB_ITEM_CLOSE_PNG );
	CSize szBtnClose( m_imgClose->GetWidth() / 3, m_imgClose->GetHeight() );

	m_rectCloseBtn.SetRect(0, 0, szBtnClose.cx, szBtnClose.cy);
	m_dwCloseBtnStatus = BS_NORMAL;

	return bCreate;
}

void WLDragTabItem::OnSized( SIZE const &szOld, SIZE const &szNew )
{
	__super::OnSized( szOld, szNew );

    CSize szBtnClose( m_rectCloseBtn.Width(), m_rectCloseBtn.Height() );

    int bnX = szNew.cx - szBtnClose.cx - 10;
    int bnY = (szNew.cy - szBtnClose.cy ) / 2 + 2;

    m_rectCloseBtn.SetRect(bnX, bnY, bnX + szBtnClose.cx, bnY + szBtnClose.cy);
}

void WLDragTabItem::OnDestroy()
{
	__super::OnDestroy();
}

void WLDragTabItem::GetIconTextDrawRect( CRect& rcInHost )
{
	GetDrawRect( rcInHost );
	if ( m_dwStatus & TIS_SELECTED )
	{
		rcInHost.right = rcInHost.left + m_rectCloseBtn.left - 2;
	}
	else
	{
		rcInHost.right -= m_ptText.x;
	}
}

BOOL WLDragTabItem::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult )
{
	BOOL bRes = FALSE;
	switch( message )
	{
	case WM_MOUSEHOVER:
		{
			BOOL bDraging = m_bCanDrag && ( m_dwStatus& TIS_LDOWN ) && ( m_dwStatus & TIS_SELECTED );
			if ( !bDraging && HitTestClostButton( CPoint(lParam) ) )
			{
				if (!(m_dwCloseBtnStatus & BS_HOT))
				{
					m_dwCloseBtnStatus |= BS_HOT ;
					UpdateView( &m_rectCloseBtn );
				}
				return TRUE;
			}
		}
		break;
	case WM_MOUSEMOVE :
		{
			BOOL bDraging = m_bCanDrag && ( m_dwStatus& TIS_LDOWN ) && ( m_dwStatus & TIS_SELECTED );
			if ( !bDraging && ( m_dwStatus & TIS_SELECTED ) && HitTestClostButton( CPoint(lParam) ))
			{
				if ( m_bLBtnDownInClose )
				{	
					if (!(m_dwCloseBtnStatus & BS_SELECTED))
					{
						m_dwCloseBtnStatus |= BS_SELECTED ;
						UpdateView( &m_rectCloseBtn );
					}		
				}
				else
				{
					if (!(m_dwCloseBtnStatus & BS_HOT))
					{
						m_dwCloseBtnStatus |= BS_HOT ;
						UpdateView( &m_rectCloseBtn );
					}
				}
				return TRUE;
			}
			else
			{
				m_dwCloseBtnStatus = BS_NORMAL;
				UpdateView( &m_rectCloseBtn );
			}
		}
		break ;
	case WM_MOUSELEAVE :
		{
			m_dwCloseBtnStatus = BS_NORMAL;
			m_bLBtnDownInClose = FALSE;
			m_bMbtnDown = FALSE;
			UpdateView( &m_rectCloseBtn );
		}
		break ;
	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			BOOL bDraging = m_bCanDrag && ( m_dwStatus& TIS_LDOWN ) && ( m_dwStatus & TIS_SELECTED );
			if ( !bDraging && ( m_dwStatus & TIS_SELECTED ) && HitTestClostButton( CPoint(lParam) ))
			{
				if (!(m_dwCloseBtnStatus & BS_SELECTED))
				{
					m_dwCloseBtnStatus |= BS_SELECTED ;
					UpdateView( &m_rectCloseBtn );
				}
				m_bLBtnDownInClose = TRUE ;
				return TRUE;
			}
		}
		break ;
	case WM_MBUTTONDOWN:
		{
			m_bMbtnDown = TRUE;
		}
		break;
	case WM_MBUTTONUP:
		{
			if ( m_bMbtnDown )
			{
				NMWLHDR item ;
				item.hWlFrom = this ;
				item.idFrom = GetID() ;
				item.code = TCN_CLOSE_BN_CLICKED;
				GetParent()->SendMessage( WM_NOTIFY, GetID(), LPARAM(&item));
				return TRUE;
			}
			m_bMbtnDown = FALSE;
		}
		break;
	case WM_LBUTTONUP :
		{
			BOOL bDraging = m_bCanDrag && ( m_dwStatus& TIS_LDOWN ) && ( m_dwStatus & TIS_SELECTED );
			if ( !bDraging && ( m_dwStatus & TIS_SELECTED ) && HitTestClostButton( CPoint(lParam) ))
			{
				if ( m_bLBtnDownInClose )
				{
					NMWLHDR item ;
					item.hWlFrom = this ;
					item.idFrom = GetID() ;
					item.code = TCN_CLOSE_BN_CLICKED;
					GetParent()->SendMessage( WM_NOTIFY, GetID(), LPARAM(&item));
					return TRUE;
				}
				bRes = TRUE;
			}
			m_bLBtnDownInClose = FALSE;
		}
		break ;
	}
	if ( !bRes )
	{
		bRes = __super::OnWndMsg( message, wParam, lParam, pResult );
	}
	return bRes;

}

void WLDragTabItem::OnDraw( HDC hDC, RECT const &rcUpdate )
{
	__super::OnDraw( hDC, rcUpdate );

	if ( m_dwStatus & TIS_SELECTED )
	{
		DrawCloseButton( hDC, rcUpdate );
	}
}

void WLDragTabItem::DrawCloseButton( HDC hDC, RECT const& /*rcUpdate*/ )
{
	CRect rcDraw;
	GetDrawRect( rcDraw );

	CRect rcDrawInHost = m_rectCloseBtn;
	rcDrawInHost.OffsetRect( rcDraw.left, rcDraw.top );

	int nWidth = m_imgClose->GetWidth() / 3;
	if ( m_dwCloseBtnStatus & BS_SELECTED )
	{
		Graphics gs( hDC );
		gs.DrawImage( m_imgClose, Rect( rcDrawInHost.left, rcDrawInHost.top,
			rcDrawInHost.Width(), rcDrawInHost.Height() ),
			2 * nWidth, 0, nWidth, m_imgClose->GetHeight(), UnitPixel );
	}
	else if ( m_dwCloseBtnStatus & BS_HOT )
	{
		Graphics gs( hDC );
		gs.DrawImage( m_imgClose, Rect( rcDrawInHost.left, rcDrawInHost.top,
			rcDrawInHost.Width(), rcDrawInHost.Height()),
			nWidth, 0, nWidth, m_imgClose->GetHeight(), UnitPixel  );
	}
	else
	{
		Graphics gs( hDC );
		gs.DrawImage( m_imgClose, Rect( rcDrawInHost.left, rcDrawInHost.top,
			rcDrawInHost.Width(), rcDrawInHost.Height()),
			0, 0, nWidth, m_imgClose->GetHeight(), UnitPixel  );
	}
}

BOOL WLDragTabItem::HitTestClostButton( POINT const& point )
{
	CPoint pt( point );
	HostToClient( &pt );
	return m_rectCloseBtn.PtInRect( pt );
}
