#include "StdAfx.h"
#include "WLTabItem.h"
#include "common/utils/Graphics.h"
#include "common/utils/ImageEx/ImageEx.h"
//#include "include/ISkin.h"
#include "logex.h"

WLTabItem::WLTabItem(void)
:
m_nIndex(-1)
, m_pPrevItem(NULL)
, m_pNextItem(NULL)
, m_dwStatus(TIS_NORMAL)
,m_bCanDrag( FALSE )
{
}

WLTabItem::WLTabItem( LPCTSTR lpszText, UINT uTextStyle, CPoint ptText, LPCTSTR sTipText, Shared_ImageEx pImgIcon, Shared_ImageEx pImgBkgnd, BOOL bCanDrag, int nTabStateCount )
:m_uTextStyle( uTextStyle ),
m_ptText( ptText ),
m_sTipText( sTipText ),
m_pImgIcon( pImgIcon ),
m_pImgBkgnd( pImgBkgnd ),
m_szIcon( 0, 0 ),
m_bCanDrag( bCanDrag ),
m_ntabStateCount( nTabStateCount )
{
	m_sText = lpszText;
	if ( m_pImgIcon )
	{
		m_szIcon.cx = pImgIcon->GetWidth();
		m_szIcon.cy = pImgIcon->GetHeight();
	}
	if ( m_ntabStateCount <= 0 )
	{
		m_ntabStateCount = 3;
	}
}

WLTabItem::~WLTabItem(void)
{
	//Destroy();
}

// CWndLess Implementation
void WLTabItem::SetItemText(LPCTSTR lpszText) 
{
	if (m_sText == lpszText)
		return ;

	m_sText = lpszText ;
	//RecalcLayout(TRUE) ;
}

void WLTabItem::OnDraw(HDC hDC, RECT const &/*rcUpdate*/) 
{
	CRect rcInHost ;
	GetDrawRect(rcInHost);

	// 根据状态绘制图片
	int nUnitWidth = m_pImgBkgnd->GetWidth() / m_ntabStateCount ;
	int nUnitX = 0 ;
	if( m_dwStatus & TIS_SELECTED)
	{
		nUnitX = nUnitWidth * 2 ;
	}
    else if ( m_dwStatus & TIS_HIGHTLIGHT )
    {
        nUnitX = nUnitWidth * 4;
    }
	else if (m_dwStatus & TIS_HOT)
	{
        nUnitX = nUnitWidth * 1 ;
	}
	else
	{
		nUnitX = 0 ;
	}

	

/*	pDC->FillSolidRect(
		item.m_ptPos.x, item.m_ptPos.y,
		item.m_szSize.cx, item.m_szSize.cy,
		m_crBkgnd
		) ;*/

	//ISkin const &skin = GetFrame()->GetSkin() ;
	//if ( skin.GetMode() != ISkin::Default )
	//{
	//	COLORREF color = skin.GetRoughColor();
	//	SchemeHelper::SetCustomColor( color );
	//	SchemeHelper::ApplySchemeColor( SchemeHelper::CUSTOM_COLOR );
	//}

	//const ImageAttributes& imgAttr = SchemeHelper::GetSchemeImageAttributes();
	//Graphics * pGraphics = new Graphics(*pDC) ;
	//pGraphics->DrawImage(
	//	item.m_pImgBkgnd,
	//	Rect( rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height() ),
	//	nUnitX, 0, nUnitWidth, item.m_pImgBkgnd->GetHeight(), UnitPixel, &imgAttr
	//	) ;
	//const ImageAttributes* pAttr = (m_dwStatus & TIS_HIGHTLIGHT) ? NULL : &imgAttr; // 高亮不变色
	hdutils::LineStretch( hDC, rcInHost.left, rcInHost.top, rcInHost.Width(), rcInHost.Height(),
		m_pImgBkgnd, nUnitX, 0, nUnitWidth, m_pImgBkgnd->GetHeight(), NULL );
	//delete pGraphics ;
	//pGraphics = NULL ;

	//SchemeHelper::ApplySchemeColor( SchemeHelper::SKY_BLUE );

	GetIconTextDrawRect( rcInHost ); // 获取文本和Icon绘制区域

	// 绘制icon和text
	if (!( m_uTextStyle & TITS_VERICAL ))	// 水平绘制
	{
		LOGFONT lf ;
		//GetParent()->GetFont()->GetLogFont(&lf) ;
		CFont * pFont = CFont::FromHandle((HFONT)::SendMessage(GetCenter()->GetHost(), WM_GETFONT, 0, 0)) ;
		pFont->GetLogFont(&lf) ;
		int nOldMode = ::SetBkMode( hDC, TRANSPARENT) ;
		//COLORREF crOldText = ::SetTextColor( hDC, m_crText) ;

		// 根据状态设置字体
		CFont ftText ;
// 		if (m_dwStatus & TISTATE_SELECTED)
// 		{
// 			lf.lfWeight = FW_BLACK ;
// 		}
		ftText.CreateFontIndirect(&lf) ;
		HFONT hOldFont = (HFONT)::SelectObject( hDC, ftText.GetSafeHandle() ) ;

		// 获得它们所占的区域
		CPoint ptIconText(0, 0) ;
		CSize szIconText(0, 0) ;
		CPoint ptIcon(0, 0) ;
		CSize szIcon(0, 0) ;
		BOOL bDrawIcon = FALSE ;
		if ( m_pImgIcon != NULL)
		{
			bDrawIcon = TRUE ;
			szIcon.cx =  m_szIcon.cx ;
			szIcon.cy =  m_szIcon.cy ;
			if ( m_szClient.cx < szIcon.cx + (int)GetMinimizeSize() ) // 不显示icon
			{
				szIcon.cx = 0;
			}

			szIconText = szIcon ;
		}

		CPoint ptText(0, 0) ;
		CSize szText(0, 0) ;
		if (!m_sText.IsEmpty())
		{
			if (bDrawIcon)
				szIconText.cx += 4 ;		// 这是图标和文字之间的间隔

			ptText.x = szIconText.cx ;

			::GetTextExtentPoint( hDC, m_sText, m_sText.GetLength(),  &szText );
			//szText = pDC->GetTextExtent(item.m_sText) ;
			szIconText.cx += szText.cx ;
			if (szIconText.cy < szText.cy)
				szIconText.cy = szText.cy ;
		}

		// 图标、文字垂直居中
		ptIcon.y = (szIconText.cy - szIcon.cy) / 2 ;
		ptText.y = (szIconText.cy - szText.cy) / 2 ;

		int nOffsetY = (rcInHost.Height() - szIconText.cy) / 2 ;

		if (m_uTextStyle & TITS_X_SPECIFY)		// 用户指定文字位置(子项坐标系)
		{
			ptIconText.x = m_ptText.x ;
		}
		if (m_uTextStyle & TITS_Y_SPECIFY)
		{
			ptIconText.y = m_ptText.y ;
		}
		if (m_uTextStyle & TITS_CENTER)		// 居中
		{
			if ( !( m_uTextStyle & TITS_X_SPECIFY ))
			{
				ptIconText.x = ( rcInHost.Width() - szIconText.cx) / 2 ;
			}
			if ( !( m_uTextStyle & TITS_Y_SPECIFY ) )
			{
				ptIconText.y = nOffsetY ;
			}
		}
		else if (m_uTextStyle & TITS_RIGHT)	// 靠右
		{
			if ( !( m_uTextStyle & TITS_X_SPECIFY ))
			{
				ptIconText.x = rcInHost.Width() - nOffsetY - szIconText.cx ;
			}
			if ( !( m_uTextStyle & TITS_Y_SPECIFY ))
			{
				ptIconText.y = nOffsetY ;
			}
		}
		else										// 靠左
		{
			if ( !( m_uTextStyle & TITS_X_SPECIFY ))
			{
				ptIconText.x = nOffsetY ;
			}
			if ( !( m_uTextStyle & TITS_Y_SPECIFY ))
			{
				ptIconText.y = nOffsetY ;
			}

			// 靠左时，文字太长显示省略号
			if (ptText.x + ptIconText.x + szText.cx > rcInHost.Width() - ptText.x - ptIconText.x )
			{
				szText.cx = rcInHost.Width() - ( ptText.x + ptIconText.x ) ;
			}
		}

		// 绘制图标
		if ( m_pImgIcon && szIcon.cx > 0 )
		{
			Graphics * pGraphics = new Graphics(hDC) ;
			pGraphics->DrawImage(
				m_pImgIcon,
				Rect(
				rcInHost.left + ptIconText.x + ptIcon.x, 
				rcInHost.top + ptIconText.y + ptIcon.y,
				szIcon.cx,
				szIcon.cy ),
				0, 0,
				m_pImgIcon->GetWidth(),
				m_pImgIcon->GetHeight(),
				UnitPixel
				) ;
			delete pGraphics ;
			pGraphics = NULL ;
		}
		// 绘制文字
		if (!(m_sText.IsEmpty()))
		{
			/*pDC->TextOut(
				item.m_ptPos.x + ptIconText.x + ptText.x,
				item.m_ptPos.y + ptIconText.y + ptText.y,
				item.m_sText
				) ;*/

			CRect rcText ;
			rcText.left = rcInHost.left + ptIconText.x + ptText.x ;
			rcText.top = rcInHost.top + ptIconText.y + ptText.y ;
			rcText.right = rcText.left + szText.cx ;
			rcText.bottom = rcText.top + szText.cy ;
			//::DrawText( hDC,  m_sText, m_sText.GetLength(), rcText, DT_END_ELLIPSIS) ;
			hdutils::DrawGlowText( hDC, m_sText, m_sText.GetLength(), rcText, DT_END_ELLIPSIS );
		}

		// 恢复
		::SelectObject( hDC, hOldFont ) ;
		//::SetTextColor( hDC, crOldText ) ;
		::SetBkMode( hDC, nOldMode ) ;
	}
	else
	{
		// 目前不提供垂直绘制
	}
}

BOOL WLTabItem::OnWndMsg(UINT message, WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;
	switch (message)
	{
	case WM_MOUSEMOVE :
		{
			bRes = TRUE ;

			if (!(m_dwStatus & TIS_HOT))
			{
				m_dwStatus |= TIS_HOT ;
				UpdateView() ;
			}

			if (m_dwStatus & TIS_LDOWN)
			{
				CPoint pt(lParam) ;
				UpdateMovedPosition(pt);
			}
		}
		break ;

	case WM_MOUSELEAVE :
		{
			if (m_dwStatus & TIS_HOT)
			{
				m_dwStatus &= ~TIS_HOT ;
				UpdateView() ;
			}
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			SetFocus() ;
			SetCapture() ;

			// 开始拖拽
			m_dwStatus |= TIS_LDOWN ;
			if ( m_bCanDrag )
			{
				CPoint pt(lParam) ;	
				m_ptCurrent =pt;
				CRect rcInHost ;
				GetRectInHost(rcInHost) ;
				m_ptDragOffset.cx = pt.x - rcInHost.left ;
				m_ptDragOffset.cy = pt.y - rcInHost.top ;
			}

			// 发送选择命令
			//m_dwStatus |= TIS_SELECTED ;
			NMWLHDR hdr ;
			hdr.hWlFrom = this ;
			hdr.idFrom = m_uID ;
			hdr.code = NM_LDOWN ;
			m_pParent->SendMessage(WM_NOTIFY, 0, (WPARAM)&hdr) ;

			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			//TRACE(_T("WLButton::WM_LBUTTONUP\n")) ;
			// 记录鼠标是否按下，因为释放鼠标控制权后会清除该标记
			ReleaseCapture() ;		

			NMWLHDR hdr ;
			hdr.hWlFrom = this ;
			hdr.idFrom = m_uID ;
			hdr.code = TCN_TAB_ITEM_LUP ;
			m_pParent->SendMessage(WM_NOTIFY, 0, (WPARAM)&hdr) ;

			// 在释放鼠标所有权时清楚TCIS_HOT状态并刷新视图
			bRes = TRUE ;
		}
		break ;
	case WM_RBUTTONUP:
		{
			NMWLHDR hdr ;
			hdr.hWlFrom = this ;
			hdr.idFrom = m_uID ;
			hdr.code = NM_RCLICK;
			m_pParent->SendMessage(WM_NOTIFY, 0, (WPARAM)&hdr) ;
			bRes = TRUE;
		}
		break;

	case WM_SETFOCUS :
		{
			//TRACE(_T("WLButton SetFocus\n")) ;
			// 			if (!(m_dwStatus & BS_FOCUS))
			// 			{
			// 				m_dwStatus |= BS_FOCUS ;
			// 				UpdateView() ;
			// 			}
			bRes = TRUE ;
		}
		break ;

	case WM_KILLFOCUS :
		{
			//TRACE(_T("WLButton KillFocus: %x\n"), wParam) ;
			// 			if (m_dwStatus & BS_FOCUS)
			// 			{
			// 				m_dwStatus &= ~BS_FOCUS ;
			// 				UpdateView() ;
			// 			}

			bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			if (m_dwStatus & TIS_HOT)
			{
				m_dwStatus &= ~TIS_HOT ;
				UpdateView() ;
			}

			if (m_dwStatus & TIS_LDOWN)
				m_dwStatus &= ~TIS_LDOWN ;

			bRes = TRUE ;
		}
		break ;

	case TIM_SELECTED :
		{
			if (!(m_dwStatus & TIS_SELECTED))
			{
				m_dwStatus |= TIS_SELECTED ;
				m_dwStatus &= ~TIS_HIGHTLIGHT;
			}
		}
		break ;

	case TIM_UNSELECTED :
		{
			if (m_dwStatus & TIS_SELECTED)
			{
				m_dwStatus &= ~TIS_SELECTED ;
			}
		}
		break ;
	case TIM_LDOWN :
		{
			if (!(m_dwStatus & TIS_LDOWN))
			{
				m_dwStatus |= TIS_LDOWN ;
			}
		}
		break ;

	case TIM_LUP :
		{
			if (m_dwStatus & TIS_LDOWN)
			{
				m_dwStatus &= ~TIS_LDOWN;
			}
		}
	}

	return bRes ;
}

// WLTabItem Implementation

void WLTabItem::SetIndex(int nIndex) 
{
	m_nIndex = nIndex ;
}

int WLTabItem::GetIndex() const 
{
	return m_nIndex ;
}

void WLTabItem::SetPrevItem(WLTabItem * pPrevItem) 
{
	m_pPrevItem = pPrevItem ;
}

WLTabItem * WLTabItem::GetPrevItem() const 
{
	return m_pPrevItem ;
}

void WLTabItem::SetNextItem(WLTabItem * pNextItem) 
{
	m_pNextItem = pNextItem ;
}

WLTabItem * WLTabItem::GetNextItem() const 
{
	return m_pNextItem ;
}

void WLTabItem::RecalcLayout(BOOL bNotifyParent) 
{
	WLDC dc(this) ;
	CSize sz = dc.GetTextExtent(m_sText) ;
	sz.cx += 20 ;
	sz.cy += 10 ;

	UINT uFlag = SWLP_CX | SWLP_CY ;
	if (bNotifyParent)
		uFlag |= SWLP_NOTIFYCHANGE ;

	SetWndLessPos(0, 0, sz.cx, sz.cy, uFlag) ;
}

void WLTabItem::SetItemData( DWORD_PTR dwData )
{
	m_dwData = dwData;
}

DWORD_PTR WLTabItem::GetItemData()  const
{
	return m_dwData;
}

void WLTabItem::SetIconSize( const CSize& size )
{
	m_szIcon = size;
}

void WLTabItem::SetTextColor( COLORREF crText )
{
	m_crText = crText;
}

void WLTabItem::GetDragOffset( CSize& offDrag )
{
	offDrag = m_ptDragOffset;
}

BOOL WLTabItem::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent )
{
	m_dwStatus = 0;
	BOOL bRes = __super::OnCreate( dwStyle, x, y, cx, cy, pParent );
	AddTooltip( m_sTipText );
	return bRes;
}

void WLTabItem::SetDragOffset( const CSize& szOff )
{
	m_ptDragOffset = szOff;
}

void WLTabItem::UpdateMovedPosition( CPoint &pt )
{
	m_ptCurrent = pt;
	LOG_DEBUG( L"WLTabItem::OnWndMsg m_ptCurrent x " << m_ptCurrent.x << L", y " << m_ptCurrent.y );
	CRect rcInHost ;
	m_pParent->GetRectInHost( rcInHost ) ;
	pt.Offset( -rcInHost.TopLeft() );

	//pt.x -= rcInScreen.left ;
	//pt.y -= rcInScreen.top;
	if ( m_bCanDrag )
	{
		// SetWndLessPos(pt.x - m_ptDragOffset.x, 0, 0, 0, SWLP_X) ;
		NMWLMOVEDINFO hdrMove ;
		hdrMove.hdr.hWlFrom = this ;
		hdrMove.hdr.idFrom = m_uID ;
		hdrMove.hdr.code = TCN_TAB_ITEM_DRAG_MOVE ;
		CPoint ptOld;
		GetPos( &ptOld );
		ClientToParent( &ptOld );
		hdrMove.ptOld = ptOld;
		CPoint ptNew( pt );
		ptNew.Offset( -m_ptDragOffset );
		hdrMove.ptNew = ptNew;
		m_pParent->SendMessage(WM_NOTIFY, 0, (WPARAM)&hdrMove) ;
	}
}

void WLTabItem::GetDrawRect( CRect &rcInHost )
{
	GetRectInHost(rcInHost) ;
	if ( m_bCanDrag && ( m_dwStatus& TIS_LDOWN ) && ( m_dwStatus & TIS_SELECTED ))
	{
		CPoint ptClient( m_ptCurrent );
		HostToClient( &ptClient );
		rcInHost.OffsetRect( ptClient.x - m_ptDragOffset.cx, 0 );

		CRect rcParent;
		GetParent()->GetRectInHost( rcParent );
		if ( rcInHost.left < rcParent.left )
		{
			rcInHost.OffsetRect(  rcParent.left - rcInHost.left, 0 );
		}
		else if ( rcInHost.right > rcParent.right )
		{
			rcInHost.OffsetRect( rcParent.right - rcInHost.right, 0 );
		}
	}
}

UINT WLTabItem::GetMinimizeSize()
{
	if ( m_pImgBkgnd )
	{
		return m_pImgBkgnd->GetWidth() / m_ntabStateCount * 2 / 3 ;
	}
    return 0;
}

void WLTabItem::GetIconTextDrawRect( CRect& rcInHost )
{
	GetRectInHost( rcInHost );
}

BOOL WLTabItem::HightLight()
{
	if ( m_dwStatus & TIS_SELECTED )
	{
		return FALSE;
	}
	m_dwStatus |= TIS_HIGHTLIGHT;
	UpdateView();
	return TRUE;
}

void WLTabItem::SetIcon( Shared_ImageEx icon )
{
	if ( !m_pImgIcon && icon )
	{
		m_szIcon.cx = icon->GetWidth();
		m_szIcon.cy = icon->GetHeight();
	}
	m_pImgIcon = icon;
	UpdateView();
}
