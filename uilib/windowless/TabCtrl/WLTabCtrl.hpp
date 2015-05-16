// TabCtrlST.cpp : implementation file
//
#include "WLTabItem.h"
#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/Graphics.h"
#include "logex.h"


/////////////////////////////////////////////////////////////////////////////
// WLTabCtrl
template<class IT>
WLTabCtrlImp<IT>::WLTabCtrlImp( BOOL bCanDrag )
:m_bCanDrag( bCanDrag )
{
	m_bFocusable = TRUE ;

	m_nNearSpace	= 0 ;
	m_nItemSpace	= 0 ;
	m_nFarSpace		= 0 ;

	m_pImgBkgnd		= NULL ;

	m_crText		= hdutils::crTextColor ;
	m_crBkgnd		= RGB(255, 255, 255) ;

	m_pSelItem = NULL;

	m_nMaxItemWidth = 0;

	m_bDelayCalItemSize = FALSE;
}
template<class IT>
WLTabCtrlImp<IT>::~WLTabCtrlImp()
{
	for( unsigned int i = 0; i < m_vecItems.size(); i++ )
	{
		delete m_vecItems[i];
	}
	m_vecItems.clear();
	// Destroy();
}

/////////////////////////////////////////////////////////////////////////////
// WLTabCtrl operations

// 函数功能：设置背景图片
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/23/2009]
template<class IT>
void WLTabCtrlImp<IT>::SetImage(
						  Image * pImgBkgnd,
						  BOOL bRedraw// = FALSE
						  )
{
	m_pImgBkgnd = pImgBkgnd ;
	UpdateView();
}

// 函数功能：设置子项间隔
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/24/2009]
template<class IT>
void WLTabCtrlImp<IT>::SetSpace(
						  int nNearSpace,
						  int nItemSpace,
						  int nFarSpace
						  ) 
{
	if (
		m_nNearSpace == nNearSpace &&
		m_nItemSpace == nItemSpace &&
		m_nFarSpace == nFarSpace
		)
		return ;

	m_nNearSpace = nNearSpace ;
	m_nItemSpace = nItemSpace ;
	m_nFarSpace = nFarSpace ;

	RecalcLayout() ;
}

// 函数功能：设置文字颜色
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[07/01/2009]
template<class IT>
void WLTabCtrlImp<IT>::SetTextColor(
							  COLORREF crText,
							  BOOL bRedraw		// = FALSE
							  ) 
{
	m_crText = crText ;

	m_crTextSelected = crText; // reset all text color

    for (std::vector<WLTabItem*>::size_type i = 0; i < m_vecItems.size(); i++ )
	{
		m_vecItems[i]->SetTextColor( crText );
	}

	if ( bRedraw )
		UpdateView();
}

// 函数功能：设置背景颜色
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[09/20/2010]
template<class IT>
void WLTabCtrlImp<IT>::SetBkgndColor(
							   COLORREF crBkgnd, 
							   BOOL bRedraw		// = FALSE
							   ) 
{
	m_crBkgnd = crBkgnd ;
	UpdateView();
}

// 设置是否可以获取焦点
template<class IT>
void WLTabCtrlImp<IT>::SetFocusable(BOOL bFocusable) 
{
	m_bFocusable = bFocusable ;
}

// 重新计算布局
template<class IT>
void WLTabCtrlImp<IT>::RecalcLayout() 
{
	if (m_vecItems.empty())
	{
		UpdateView();
		return ;
	}

	DWORD dwStyle = GetStyle();

	CRect rcClient ;
	GetRectInClient(rcClient) ;

	int nItemCount = m_vecItems.size() ;

	int nBound = 0 ;
	if (dwStyle & TS_AUTOADJUSTBOUND)
	{
		if (!(dwStyle & TS_VERTICAL))
		{
			nBound = (rcClient.Width() - m_nNearSpace - m_nFarSpace - (nItemCount - 1) * m_nItemSpace) / nItemCount ;
		}
		else
		{
			nBound = (rcClient.Height() - m_nNearSpace - m_nFarSpace - (nItemCount - 1) * m_nItemSpace) / nItemCount ;
		}
	}

	int i ;
	int nPos = m_nNearSpace ;
	if (!(dwStyle & TS_VERTICAL))		// 水平
	{
		CPoint ptItemTmp;
		CSize szItemTmp;
		for (i = 0 ; i < nItemCount ; ++ i)
		{
			WLTabItem*  pItem = m_vecItems[i] ;
			pItem->GetSize( &szItemTmp );
			pItem->GetPos( &ptItemTmp );
			ptItemTmp.x = nPos ;
			int nNextPos = 0 ;
			if (dwStyle & TS_AUTOADJUSTBOUND )
			{
				nNextPos = (rcClient.Width() + m_nItemSpace - m_nNearSpace - m_nFarSpace)  / nItemCount * (i + 1) + m_nNearSpace ;
				szItemTmp.cx = nNextPos - m_nItemSpace - nPos  ;
				if ( szItemTmp.cx > m_nMaxItemWidth && m_nMaxItemWidth > 0 )
				{
					szItemTmp.cx = m_nMaxItemWidth;
					nNextPos = m_nMaxItemWidth + m_nItemSpace + nPos;
				}
			}
			else
			{
				nNextPos = nPos +szItemTmp.cx + m_nItemSpace ;
			}
			nPos = nNextPos ;

			if (!(dwStyle & TS_BOTTOM))
			{
				ptItemTmp.y = rcClient.bottom - szItemTmp.cy ;
			}
			else
			{
				ptItemTmp.y = 0 ;
			}
			pItem->SetWndLessPos( ptItemTmp.x, ptItemTmp.y, szItemTmp.cx, szItemTmp.cy, SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY | SWLP_NOREDRAW  );
		}
	}
	else		// 垂直
	{
		CPoint ptItemTmp;
		CSize szItemTmp;
		for (i = 0 ; i < nItemCount ; ++ i)
		{
			WLTabItem*  pItem = m_vecItems[i] ;
			pItem->GetSize( &szItemTmp );
			pItem->GetPos( &ptItemTmp );
			ptItemTmp.y = nPos ;
			if (dwStyle & TS_AUTOADJUSTBOUND)
				szItemTmp.cy = nBound ;
			nPos += szItemTmp.cy + m_nItemSpace ;
			if (!(dwStyle & TS_RIGHT))
			{
				ptItemTmp.x = rcClient.right - szItemTmp.cx ;
			}
			else
			{
				ptItemTmp.x = 0 ;
			}
			pItem->SetWndLessPos( ptItemTmp.x, ptItemTmp.y, szItemTmp.cx, szItemTmp.cy, SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY | SWLP_NOREDRAW );
		}
	}
	UpdateView();
}

// 函数功能：插入子项
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/23/2009]
template<class IT>
int WLTabCtrlImp<IT>::InsertItem(
						   int nIndex,
						   Shared_ImageEx pImgIcon,
						   Shared_ImageEx pImgBkgnd,
						   int nTabStateCount,
						   LPCTSTR lpszText, // = NULL
						   UINT uTextStyle, // = 0
						   int nTextX, // = 0
						   int nTextY, // = 0
						   LPCTSTR lpszTip, // = NULL
						   int nMaxWidth // = 0
						   ) 
{
	if (nIndex == -1)
		nIndex = (int)m_vecItems.size() ;

	if (nIndex < 0 || nIndex > (int)m_vecItems.size())
		return -1 ;

	ASSERT(pImgBkgnd != NULL && pImgBkgnd->GetLastStatus() == Ok) ;
	if (!(pImgBkgnd != NULL && pImgBkgnd->GetLastStatus() == Ok))
		return -1 ;

	m_nMaxItemWidth = nMaxWidth;

	WLTabItem* pTabItem = CreateTabItem( lpszText, uTextStyle, CPoint( nTextX, nTextY ), lpszTip, pImgIcon, pImgBkgnd, m_bCanDrag, nTabStateCount );

	pTabItem->SetTextColor( m_crText );

	int nWidth = m_nMaxItemWidth>0 ? m_nMaxItemWidth : pImgBkgnd->GetWidth() / nTabStateCount ;
	int nHeight = pImgBkgnd->GetHeight() ;

	BOOL bRes = pTabItem->Create( WS_VISIBLE, 0, 0, nWidth, nHeight, m_wrapper, -1 );
	if ( !bRes )
	{
		delete pTabItem;
		return -1;
	}

	//pTabItem->SetIndex( nIndex );

	if ( m_vecItems.size() > 0 )
	{
		ASSERT( pTabItem != m_vecItems[0] );
	}

	m_vecItems.insert(m_vecItems.begin() + nIndex, pTabItem ) ;
	ResetIndexAndZorder();

	RecalcLayout() ;

	return nIndex ;
}

// 设置子项文本
template<class IT>
BOOL WLTabCtrlImp<IT>::SetItemText(int nIndex, LPCTSTR lpszText) 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;

	m_vecItems[nIndex]->SetWindowText( lpszText );

	m_vecItems[nIndex]->UpdateTooltip( lpszText );

	RecalcLayout() ;

	return TRUE ;
}

// 设置子项数据
template<class IT>
BOOL WLTabCtrlImp<IT>::SetItemData(int nIndex, DWORD_PTR dwData) 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;

	m_vecItems[nIndex]->SetItemData( dwData );
	return TRUE ;
}

// 获取子项数据
template<class IT>
DWORD_PTR WLTabCtrlImp<IT>::GetItemData(int nIndex) const 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return 0 ;

	return m_vecItems[nIndex]->GetItemData() ;
}

// 函数功能：删除子项
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/23/2009]
template<class IT>
BOOL WLTabCtrlImp<IT>::DeleteItem( int nIndex, BOOL bResize ) 
{
	int size = (int)m_vecItems.size();
	if (nIndex < 0 || nIndex >= size)
		return FALSE ;
	if ( m_pSelItem == m_vecItems[nIndex] )
	{
		if ( m_vecItems.size() <= 1 )
		{
			m_pSelItem = NULL;
		}
		else
		{
			int nNextSelItem = ( nIndex == ( size - 1 ) ) ? ( nIndex -1 ) : ( nIndex + 1 );
			SelectItem( nNextSelItem );
		}
	}

	LOG_DEBUG( L"DeleteIem: " << m_vecItems[nIndex] << L" nIndex: " << nIndex );

	CSize delItemSize;
	m_vecItems[nIndex]->GetSize( &delItemSize );

	m_vecItems[nIndex]->Destroy();

	delete m_vecItems[nIndex];
	m_vecItems[nIndex] = NULL;
	m_vecItems.erase(m_vecItems.begin() + nIndex) ;

	ResetIndexAndZorder();

	BOOL bNeedResize = bResize || ( nIndex == m_vecItems.size() );

	RepositionAfterDeleteItem( bNeedResize, nIndex, delItemSize);


	return TRUE ;
}

// 函数功能：选择子项
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/23/2009]
template<class IT>
BOOL WLTabCtrlImp<IT>::SelectItem(int nIndex) 
{
	if ( nIndex < 0 || nIndex > (int)m_vecItems.size() )
	{
		return FALSE;
	}

	if ( m_pSelItem && m_pSelItem->GetIndex() == nIndex )
	{
		NMWLTABITEM item ;
		item.hdr.hWlFrom =  m_wrapper;
		item.hdr.idFrom = GetID() ;
		item.hdr.code = TCN_FOCUSCHANGE ;
		item.nOldItem = nIndex ;
		item.dwOldItemData =  GetItemData( nIndex ) ;
		item.nNewItem = nIndex ;
		item.dwNewItemData =  GetItemData( nIndex ) ;

		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item) ;

		return TRUE ;
	}

	m_vecItems[nIndex]->SendMessage( WLTabItem::TIM_SELECTED, 0, 0 );
	int nOldItem  = -1;
	if ( m_pSelItem )
	{
		m_pSelItem->SendMessage( WLTabItem::TIM_UNSELECTED, 0, 0) ;
		m_pSelItem->UpdateView() ;
		nOldItem = m_pSelItem->GetIndex() ;

		m_pSelItem->SetTextColor( m_crText ); // 恢复为选择item文本颜色
	}
	m_vecItems[nIndex]->SetTextColor( m_crTextSelected ); // 设置选中item文本颜色
	m_vecItems[nIndex]->UpdateView() ;	
	m_pSelItem = m_vecItems[nIndex];
	ResetIndexAndZorder();

	NMWLTABITEM item ;
	item.hdr.hWlFrom = m_wrapper ;
	item.hdr.idFrom = GetID();
	item.hdr.code = TCN_SELCHANGE ;
	item.nOldItem = nOldItem ;
	item.dwOldItemData = GetItemData( nOldItem ) ;
	item.nNewItem = nIndex ;
	item.dwNewItemData = GetItemData( nIndex ) ;

	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item) ;

	return TRUE ;
}


// 函数功能：获得子项区域
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/23/2009]
template<class IT>
BOOL WLTabCtrlImp<IT>::GetItemRect(int nIndex, CRect &rcItem) 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;
	m_vecItems[nIndex]->GetRectInParent( &rcItem );
	return TRUE ;
}

template<class IT>
BOOL WLTabCtrlImp<IT>::GetDrawItemRect( int nIndex, CRect& rcItem )
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;
	m_vecItems[nIndex]->GetDrawRect( rcItem );
	//m_vecItems[nIndex]->HostToParent( &rcItem );
	return TRUE;
}


// 函数功能：获得当前选择的子项索引
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/24/2009]
template<class IT>
int WLTabCtrlImp<IT>::GetCurSel() const 
{
	int nSelItem = -1;
	if ( m_pSelItem )
	{
		nSelItem = m_pSelItem->GetIndex();
	}
	return nSelItem ;
}

// 函数功能：获得子项数量
// 输入参数：
// 返回值  ：
// 编写人及日期 ：林珽震[06/24/2009]
template<class IT>
int WLTabCtrlImp<IT>::GetItemCount() const 
{
	return m_vecItems.size() ;
}


template<class IT>
void WLTabCtrlImp<IT>::OnDraw( HDC hDC, RECT const &rcUpdate ) 
{
	CRect rcClient ;
	GetRectInHost(rcClient) ;
	if (m_pImgBkgnd != NULL && m_pImgBkgnd->GetLastStatus() == Ok)
	{
		hdutils::LineStretch( hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), m_pImgBkgnd, 0, 0, m_pImgBkgnd->GetWidth(), m_pImgBkgnd->GetHeight() );
	}

}

template<class IT>
void WLTabCtrlImp<IT>::OnSized( SIZE const &szOld, SIZE const &szNew )
{
	__super::OnSized(szOld, szNew);

//	DWORD dwStyle = GetWindowStyle(GetSafeHwnd()) ;
	RecalcLayout() ;

// 更新提示条响应区域
}

template<class IT>
void WLTabCtrlImp<IT>::OnDestroy()
{
    for( std::vector<WLTabItem*>::size_type i = 0; i < m_vecItems.size(); i++ )
	{
		m_vecItems[i]->Destroy();
		delete m_vecItems[i];
	}
	m_vecItems.clear() ;
	m_pSelItem = NULL;
}

template<class IT>
BOOL WLTabCtrlImp<IT>::SetItemIconSize( int nIndex, CSize& size )
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;

	m_vecItems[nIndex]->SetIconSize( size );
	return TRUE ;
}
template<class IT>
BOOL WLTabCtrlImp<IT>::SetItemIcon( int nIndex, Shared_ImageEx icon )
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return FALSE ;

	m_vecItems[nIndex]->SetIcon( icon );
	return TRUE ;
}

template<class IT>
void WLTabCtrlImp<IT>::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WLTabNotifyData notifyData;
	notifyData.hdr.hWlFrom = m_wrapper;
	notifyData.hdr.idFrom = GetID();
	notifyData.hdr.code = NM_RCLICK;
	notifyData.nIdx = -1;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)notifyData.hdr.idFrom, (LPARAM)&notifyData);
}

template<class IT>
BOOL WLTabCtrlImp<IT>::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult )
{
	BOOL bRes = FALSE;
	*pResult = 0 ;
	switch( message )
	{
	case WM_NOTIFY:
		{
			LPNMWLHDR lpNMWLHDR = (LPNMWLHDR)lParam ;
			if (lpNMWLHDR->code == NM_LDOWN)
			{
				OnLDownItem(lpNMWLHDR) ;
				return TRUE ;
			}
            else if ( lpNMWLHDR->code == WLTabItem::TCN_TAB_ITEM_DRAG_MOVE )
			{
				OnDragItemMove( lpNMWLHDR );
				return TRUE;
			}
			else if ( lpNMWLHDR->code == WLTabItem::TCN_TAB_ITEM_LUP )
			{
				UpdateView();
			}
			else if ( lpNMWLHDR->code == NM_RCLICK )
			{
				OnItemRBtnClicked( lpNMWLHDR );
			}
		}
		break;
	case WM_MOUSELEAVE:
		{
			HWL hEnter = HWL(lParam) ;
			if ( m_bDelayCalItemSize && ( hEnter != m_wrapper ) &&  !IsChild( hEnter, TRUE )  )
			{
				RecalcLayout();
				m_bDelayCalItemSize = FALSE;
			}
		}
		break;
	default:
		break;
	}
	return bRes;
}

template<class IT>
void WLTabCtrlImp<IT>::OnLDownItem( LPNMWLHDR lpNMWLHDR )
{
	ASSERT(  lpNMWLHDR->hWlFrom );
	WLTabItem* pTabItem = (WLTabItem*)lpNMWLHDR->hWlFrom;
	SelectItem( pTabItem->GetIndex() );	
}


template<class IT>
void WLTabCtrlImp<IT>::OnDragItemMove( LPNMWLHDR lpNMWLHDR )
{
	LPNMWLMOVEDINFO pDragMoveInfo = (LPNMWLMOVEDINFO)lpNMWLHDR;
	ASSERT( pDragMoveInfo->hdr.hWlFrom == m_pSelItem );
	CRect rcTabCtrl;
	GetRectInClient( rcTabCtrl );

	CSize szItem;
	pDragMoveInfo->hdr.hWlFrom->GetSize( &szItem );
	int nItemWidth = szItem.cx + m_nItemSpace;
	int nCurrentBlockPos = pDragMoveInfo->ptNew.x + nItemWidth / 2;
	int nTargetIndex = nCurrentBlockPos / nItemWidth;

	if ( nTargetIndex < 0 )
	{
		nTargetIndex = 0;
	}
	else if ( nTargetIndex >= (int)m_vecItems.size() )
	{
		nTargetIndex = m_vecItems.size() - 1;
	}
	if ( nTargetIndex != m_pSelItem->GetIndex() )
	{
		ASSERT( m_vecItems[m_pSelItem->GetIndex()] == m_pSelItem );
		m_vecItems.erase( m_vecItems.begin() + m_pSelItem->GetIndex() );
		m_vecItems.insert( m_vecItems.begin() + nTargetIndex, m_pSelItem );
		ResetIndexAndZorder();
		for ( unsigned int i = 0 ; i < m_vecItems.size(); ++i )
		{
			m_vecItems[i]->SetWndLessPos( m_nNearSpace + i * nItemWidth, 0, 0, 0, SWLP_X | SWLP_NOREDRAW );
		}
	}
	UpdateView();
}

template<class IT>
void WLTabCtrlImp<IT>::ResetIndexAndZorder()
{
	HWL hWndAfter = NULL;
	for ( int i = m_vecItems.size() -1 ; i >= 0; --i )
	{
		m_vecItems[i]->SetIndex( i );
		if ( m_vecItems[i] != m_pSelItem )
		{
			m_vecItems[i]->SetZorder( hWndAfter );
			hWndAfter = m_vecItems[i];
		}
	}
	if ( m_pSelItem )
	{
		m_pSelItem->SetZorder( hWndAfter );
	}
}

template<class IT>
WLTabItem* WLTabCtrlImp<IT>::CreateTabItem( LPCTSTR lpszText, 
										   UINT uTextStyle,
										   CPoint	ptText,
										   LPCTSTR sTipText,
										   Shared_ImageEx pImgIcon,
										   Shared_ImageEx pImgBkgnd,
										   BOOL bDrag,
										   int nTabStateCount )
{
	WLTabItem* pItem = new WLTabItem( lpszText, uTextStyle, ptText, sTipText, pImgIcon, pImgBkgnd, m_bCanDrag, nTabStateCount  );
	pItem->SetWrapper(pItem);//no wrapper
	return pItem;
}

template<class IT>
BOOL WLTabCtrlImp<IT>::HightLightItem( int nIndex )
{
	if ( nIndex < 0 || nIndex > (int)m_vecItems.size() )
	{
		return FALSE;
	}
	return m_vecItems[nIndex]->HightLight();
}

template<class IT>
void WLTabCtrlImp<IT>::OnItemRBtnClicked( LPNMWLHDR lpNMWLHDR )
{	
	WLTabItem* pTabItem = (WLTabItem*)lpNMWLHDR->hWlFrom;
	NMWLTABITEM item ;
	item.hdr.hWlFrom =  m_wrapper;
	item.hdr.idFrom = GetID() ;
	item.hdr.code = NM_RCLICK ;
	item.nOldItem = pTabItem->GetIndex() ;
	item.nNewItem = pTabItem->GetIndex();
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item) ;
}

template<class IT>
void WLTabCtrlImp<IT>::RepositionAfterDeleteItem( BOOL bResize, int nIndex, CSize &delItemSize )
{
	if ( bResize )
	{
		RecalcLayout() ;
	}
	else
	{
		if (nIndex >= 0)
		{
			for ( unsigned int i = nIndex; i < m_vecItems.size(); i++ )
			{
				m_vecItems[i]->SetWndLessPos( -( delItemSize.cx + m_nItemSpace ), 0, 0, 0, SWLP_X | SWLP_X_OFFSET );
			}
		}

		m_bDelayCalItemSize = TRUE;
	}
}

template<class IT>
void WLTabCtrlImp<IT>::SetSelectTextColor( COLORREF crText, BOOL bRedraw /*= FALSE */ )
{
	m_crTextSelected = crText;
	if ( m_pSelItem )
	{
		m_pSelItem->SetTextColor( m_crTextSelected );
	}
	if ( bRedraw )
	{
		UpdateView();
	}
}

