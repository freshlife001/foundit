#include "StdAfx.h"
#include "WLDragTabCtrl.h"
#include "DragTabImage.h"
#include "logex.h"
#include "windowless/TabCtrl/WLTabItem.h"
#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"
#include "windowless/ButtonAL/WLButtonALHelper.h"
#include "WLDragTabItem.h"

WORD WLDragTabCtrlImp::s_uDragTabClipboardFormat = 0;

WLDragTabCtrlImp::WLDragTabCtrlImp( BOOL bCanDrag )
:  WLTabCtrlImp<IWLDragTabCtrl>( bCanDrag ),
m_bBeginDrag( FALSE ),
m_bDragging( FALSE )
{
	// Register clipformat according the process id, the drag drop operation just supported in the same process
	if ( s_uDragTabClipboardFormat == 0 )
	{
		CString csClipFormat;
		DWORD dwProcessId = GetCurrentProcessId();
		csClipFormat.Format( L"DragTabCtrl_%d", dwProcessId );
		s_uDragTabClipboardFormat = (WORD)RegisterClipboardFormat( csClipFormat );
	}
	m_btnAdd = NULL;
	m_nAddBtnSpace = 0;
	m_nTabItemMinSize = -1;
	// DEFINE_DEBUG_FRAME_COLOR( RGB( 0, 255, 0 ));
}

WLDragTabCtrlImp::~WLDragTabCtrlImp(void)
{
	// Destroy();
}

//void WLDragTabCtrl::OnMouseMove( UINT nFlags, CPoint point )
//{
//	if ( m_bBeginDrag )
//	{
//		CRect rcThis;
//		GetRectInClient( &rcThis );
//		if ( rcThis.PtInRect( point ))
//		{
//			CheckOrder( point );
//		}
//		else
//		{
//			if ( !m_bDragging )
//			{	
//				BeginDrag( point );	
//				m_bDragging = TRUE;
//			}
//		}
//	}
//	else
//	{
//		WLTabCtrl::OnMouseMove( nFlags, point );
//	}
//}

//void WLDragTabCtrl::OnLButtonDown( UINT nFlags, CPoint point )
//{
//	int nItem = HitTest(point) ;
//	if ( nItem >= 0 && nItem < m_vecItems.size() )
//	{
//		SelectItem(nItem) ;
//		m_ptPreProcPoint = point ;
//		m_szDragOffset.cx = 0;
//		m_bBeginDrag = TRUE ;
//		m_bDragging = FALSE;
//		if ( GetCapture() != this )
//		{
//			SetCapture();
//		}
//	}
//	m_nLBDownItem = nItem;
//}
//
//void WLDragTabCtrl::OnLButtonUp( UINT nFlags, CPoint point )
//{
//	m_bBeginDrag = FALSE;
//	m_bDragging = FALSE;
//	if ( GetCapture() == this )
//	{
//		ReleaseCapture();
//	}
//	UpdateView();
//}

void WLDragTabCtrlImp::BeginDrag( const CPoint& /*ptDragClient*/)
{
	LOG_FUNC_SCOPE();
	ASSERT( m_pSelItem );
	int nSelectedItem = m_pSelItem->GetIndex();
	ASSERT( nSelectedItem >=0 && nSelectedItem < (int)m_vecItems.size() );
	COleDataSource oleDataSource;

	HGLOBAL hgDrop = NULL;
	FORMATETC      etc = { s_uDragTabClipboardFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, sizeof( DRAGPARAM ) );

	DRAGPARAM* pDragParam = (DRAGPARAM*)GlobalLock( hgDrop );

	CRect rcItem ;
	GetDrawItemRect( nSelectedItem, rcItem ) ;
	CDragTabImage imgDrag;
	CPoint ptItem(rcItem.TopLeft()) ;
	HostToClient( &ptItem );
	ClientToScreen(&ptItem) ;
	//CPoint ptDrag( ptDragClient ) ;
	//ClientToScreen(&ptDrag) ;
	CSize szDragOffset;
	m_pSelItem->GetDragOffset( szDragOffset );
	CPoint ptDrag( ptItem );
	ptDrag.Offset( szDragOffset );
	//imgDrag.Create(&dc, rcItem, this, ptItem, ptDrag) ;

	//CBitmap* bmp = CBitmap::FromHandle( (HBITMAP)::LoadImage( AfxGetInstanceHandle(), L"aa.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE ));
	CBitmap bmpDrag;
	GetDragBitmap( nSelectedItem, bmpDrag );
	CSize szItem( rcItem.Width() / 2, rcItem.Height() / 2 );
	imgDrag.Create( &bmpDrag, szItem, CWnd::FromHandle( GetCenter()->GetHost() ), ptItem, ptDrag );

	//const WLTabItem* pSelItem = m_vecItems[m_nSelectedItem];
	DWORD_PTR dwSelData = m_pSelItem->GetItemData();
	CPoint ptSelItem;
	m_pSelItem->GetPos( &ptSelItem );
	CSize szOffset;
	m_pSelItem->GetDragOffset( szOffset );

	//int nCurrentItem = nSelectedItem;
	//if ( m_vecItems.size() > 1 )
	//{
	//	int nNextItem = ( ( nSelectedItem + 1 ) >= m_vecItems.size() ) ? nSelectedItem -1 : ( nSelectedItem + 1 );
	//	SelectItem( nNextItem );
	//}

	LOG_DEBUG( L"BeginDrag: nSelectedItem " << nSelectedItem << L", dwData " << dwSelData );

	//DeleteItem( nCurrentItem );
	NMWLTABDRAGEVENT item ;
	item.hdr.hdr.hWlFrom = this ;
	item.hdr.hdr.idFrom = GetID() ;
	item.hdr.hdr.code = (UINT)TCN_TAB_DRAG_BEGIN;
	item.nEnterIndex = nSelectedItem;
	GetParent()->SendMessage( WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item ) ;

	m_bDragging = TRUE;
	m_bBeginDrag = FALSE;

	pDragParam->m_ptOffset = szOffset;
	pDragParam->m_dwData = dwSelData;
	//pDragParam->m_pImgIcon = pSelItem.m_pImgIcon;
	//pDragParam->m_uTextStyle = pSelItem.m_uTextStyle;
	//memset( pDragParam->m_sText, 0, sizeof( TCHAR ) * MAX_PATH );
	//CString csText;
	//pSelItem->GetWindowText( csText );
	//_tcsncpy( pDragParam->m_sText, csText, MAX_PATH - 1 );
	//memset( pDragParam->m_sTipText, 0, sizeof( TCHAR ) * MAX_PATH );
	//_tcsncpy( pDragParam->m_sTipText, selItem.m_sTipText.GetString(), MAX_PATH - 1 );

	GlobalUnlock( hgDrop );

	oleDataSource.CacheGlobalData( s_uDragTabClipboardFormat, hgDrop, &etc );

	//CComPtr<IDragSourceHelper> pHelper;
	//InitDragSourceHelper( m_nSelectedItem, oleDataSource, pHelper );

	DROPEFFECT eff = oleDataSource.DoDragDrop( DROPEFFECT_MOVE, NULL, &imgDrag );

	imgDrag.EndDrag();

	m_bDragging = FALSE;
	if ( eff == DROPEFFECT_NONE )
	{
		NMWLTABDRAGEVENT item ;
		item.hdr.hdr.hWlFrom = this ;
		item.hdr.hdr.idFrom = GetID() ;
		item.hdr.hdr.code = (UINT)TCN_DROP_TAB_ITEM ;
		item.hdr.dragParam = *pDragParam;
		item.nEnterIndex = eff;
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item) ;
	}
	else
	{
		if ( GetCapture() == m_wrapper )
		{
			ReleaseCapture();
		}
		RecalcLayout();
		NMWLTABDRAGEVENT item ;
		item.hdr.hdr.hWlFrom = this ;
		item.hdr.hdr.idFrom = GetID() ;
		item.hdr.hdr.code = (UINT)TCN_DROP_TAB_ITEM ;
		item.hdr.dragParam = *pDragParam;
		item.nEnterIndex = eff;
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item) ;
	}
	if ( hgDrop )
	{
		GlobalFree( hgDrop );
	}
}

//void WLDragTabCtrl::CheckOrder( const CPoint& ptCurrent )
//{
//	//ASSERT( m_nLBDownItem == m_nLBDownItem );
//	if ( !m_pSelItem )
//	{
//		ASSERT( FALSE );
//		return;
//	}
//	int nSelectedItem = m_pSelItem->GetIndex();
//	if ( nSelectedItem < 0 || nSelectedItem >= m_vecItems.size() )
//	{
//		ASSERT( FALSE );
//		return;
//	}
//	//WLTabItem* pSelItem = m_vecItems[m_nSelectedItem];
//	CRect rcSelItem;
//	m_pSelItem->GetRectInParent( rcSelItem );
//	int nItemWidth = rcSelItem.Width() + m_nItemSpace;
//	ASSERT( nItemWidth > 0 );
//	int nCurrentBlockPos =rcSelItem.left + nItemWidth / 2;
//	nCurrentBlockPos += ( ptCurrent.x - m_ptPreProcPoint.x );
//
//	// 确定当前需要移动到的位置
//	int nTargetIndex = nCurrentBlockPos / nItemWidth;
//	ASSERT( nTargetIndex >= 0 );
//	if ( nTargetIndex < 0 )
//	{
//		nTargetIndex = 0;
//	}
//	if ( nTargetIndex >= m_vecItems.size() )
//	{
//		nTargetIndex = m_vecItems.size() - 1;
//	}
//	if ( nTargetIndex != nSelectedItem )
//	{
//		m_vecItems.erase( m_vecItems.begin() + nSelectedItem );
//		m_vecItems.insert( m_vecItems.begin() + nTargetIndex, m_pSelItem );
//
//		m_ptPreProcPoint.x += nItemWidth * ( nTargetIndex - nSelectedItem );
//		m_szDragOffset = ptCurrent - m_ptPreProcPoint;
//		RecalcLayout() ;
//	}
//	else
//	{
//		m_szDragOffset = ptCurrent - m_ptPreProcPoint;
//		UpdateView();
//	}
//}

//BOOL WLDragTabCtrl::GetDrawItemRect( int nIndex, CRect& rcItem )
//{
//	BOOL bSuc = WLTabCtrl::GetDrawItemRect( nIndex, rcItem );
//	if( bSuc )
//	{
//		if ( m_bBeginDrag && m_pSelItem && nIndex == m_pSelItem->GetIndex() )
//		{
//			CSize szOffset;
//			m_pSelItem->GetDragOffset( szOffset );
//			rcItem.OffsetRect( szOffset.cx, 0 );
//			CRect rcClient;
//			GetRectInClient( rcClient );
//			if ( rcItem.left < rcClient.left )
//			{
//				rcItem.OffsetRect( rcClient.left - rcItem.left, 0 );
//			}
//			else if ( rcItem.right > rcClient.right )
//			{
//				rcItem.OffsetRect( rcClient.right - rcItem.right, 0 );
//			}
//		}
//	}
//	return bSuc;
//}

DROPEFFECT WLDragTabCtrlImp::OnDragEnter( CWnd* /*pWnd*/, COleDataObject* pDataObject, DWORD /*dwKeyState*/, CPoint point )
{
	LOG_FUNC_SCOPE();
	if ( m_vecItems.empty() || !pDataObject->IsDataAvailable( s_uDragTabClipboardFormat ) )
	{
		return DROPEFFECT_NONE;
	}
	HGLOBAL hgDrag = pDataObject->GetGlobalData( s_uDragTabClipboardFormat );
	if ( hgDrag )
	{
		DRAGPARAM* pDragParam = (DRAGPARAM*)GlobalLock( hgDrag );
		CSize szItem;
		m_vecItems[0]->GetSize( &szItem );
		int nItemWidth = szItem.cx + m_nItemSpace;
		HostToClient( &point );
		int nTargetIndex = ( point.x + nItemWidth / 2 )  / nItemWidth;
		if ( nTargetIndex < 0 )
		{
			nTargetIndex = 0;
		}
		if ( nTargetIndex >= (int)m_vecItems.size() )
		{
			nTargetIndex = m_vecItems.size();
		}

		//InsertItem( nTargetIndex, pDragParam->m_pImgIcon, 
		//	m_vecItems[0].m_pImgBkgnd, pDragParam->m_sText,
		//	pDragParam->m_uTextStyle, 0, 0, pDragParam->m_sTipText );
		//SetItemData( nTargetIndex, pDragParam->m_dwData );

		NMWLTABDRAGEVENT item ;
		item.hdr.hdr.hWlFrom = this ;
		item.hdr.hdr.idFrom = GetID() ;
		item.hdr.hdr.code = (UINT)TCN_TAB_DRAG_ENTER;
		item.hdr.dragParam = *pDragParam;
		item.nEnterIndex = nTargetIndex;
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item);

		if (nTargetIndex < GetItemCount())
		{
			SelectItem( nTargetIndex );
			m_pSelItem->SetDragOffset( pDragParam->m_ptOffset );
			m_pSelItem->SendMessage( WLTabItem::TIM_LDOWN, 0, 0 );
		}


		// CheckOrder( point );

		//CPoint ptTargetItem;
		//m_vecItems[nTargetIndex]->GetPos( &ptTargetItem );


		//m_ptPreProcPoint = ptTargetItem + pDragParam->m_ptOffset;
		//m_szDragOffset.cx = 0;
		m_bBeginDrag = TRUE ;
		GlobalUnlock( hgDrag );
	}
	else
	{
		//ASSERT( FALSE );
		return DROPEFFECT_NONE;
	}
	return DROPEFFECT_MOVE;
}

BOOL WLDragTabCtrlImp::OnCreate(
					  DWORD dwStyle, 
					  int x, int y, int cx, int cy,
					  IWndLess * pParent
					  )
{
	m_dropTarget.Register( this );
	BOOL bCreate = __super::OnCreate( dwStyle, x, y, cx, cy,  pParent );
	if ( !bCreate )
	{
		return FALSE;
	}

	//设置添加按钮
	m_btnAdd = new WLButtonAL();
	m_imgAdd = Shared_ImageEx::FromId( IDI_DRAG_TAB_ADD_PNG );
	bCreate = m_btnAdd->Create( WS_VISIBLE, 0, 0, m_imgAdd->GetWidth()/3, m_imgAdd->GetHeight(), m_wrapper, CTRL_ID_BTN_ADD );
	if ( !bCreate )
	{
		return FALSE;
	}
	m_btnAdd->AddTooltip(_T("添加"));
	WLButtonALHelper::Process( *m_btnAdd, _T("DefPushButton"));
	m_btnAdd->SetFrgnd( m_imgAdd, 3, WLButtonAL::FIXED );

	m_nAddBtnSpace = 0;

	return bCreate;

}

DROPEFFECT WLDragTabCtrlImp::OnDragOver( CWnd* /*pWnd*/, COleDataObject* pDataObject, DWORD /*dwKeyState*/, CPoint point )
{
	if ( m_vecItems.empty() || !pDataObject->IsDataAvailable( s_uDragTabClipboardFormat ) )
	{
		return DROPEFFECT_NONE;
	}
	//CheckOrder( point );
	ASSERT( m_pSelItem );
	if ( m_pSelItem )
	{
		m_pSelItem->UpdateMovedPosition( point );
	}
	return DROPEFFECT_MOVE;
}

BOOL WLDragTabCtrlImp::OnDrop( CWnd* /*pWnd*/, COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/, CPoint /*point*/ )
{
	LOG_FUNC_SCOPE();
	if ( m_vecItems.empty() || !pDataObject->IsDataAvailable( s_uDragTabClipboardFormat ) )
	{
		return TRUE;
	}
	m_bBeginDrag = FALSE;
	m_pSelItem->SendMessage( WLTabItem::TIM_LUP, 0, 0 );
	//m_szDragOffset.cx = 0;
	UpdateView();
	return TRUE;
}

void WLDragTabCtrlImp::OnDragLeave( CWnd* )
{
	LOG_FUNC_SCOPE();
	if ( !m_bBeginDrag || !m_pSelItem )
	{
		return;
	}
	int nSelectedItem = m_pSelItem->GetIndex();
	if ( nSelectedItem >= 0 && nSelectedItem < (int)m_vecItems.size() )
	{
		m_pSelItem->SendMessage( WLTabItem::TIM_LUP, 0, 0 );
		int nCurrentItem = nSelectedItem;
		int nNextItem = ( ( nSelectedItem + 1 ) >= (int)m_vecItems.size() ) ? nSelectedItem -1 : ( nSelectedItem + 1 );
		SelectItem( nNextItem );

		NMWLTABDRAGEVENT item ;
		item.hdr.hdr.hWlFrom = this ;
		item.hdr.hdr.idFrom = GetID() ;
		item.hdr.hdr.code = (UINT)TCN_TAB_DRAG_LEAVE;
		item.nEnterIndex = nCurrentItem;
		GetParent()->SendMessage( WM_NOTIFY, (WPARAM)GetID(), (LPARAM)&item ) ;

		m_bBeginDrag = FALSE;	
		UpdateView();
	}
	else
	{
		ASSERT( FALSE );
	}
}

CBitmap& WLDragTabCtrlImp::GetDragBitmap( UINT uDragIndex, CBitmap& rBmp )
{
	if ( uDragIndex >= m_vecItems.size() )
	{
		return rBmp;
	}
	WLTabItem* pItem = m_vecItems[uDragIndex];
	IWndLess* pWnd =  (IWndLess*)pItem->GetItemData();
	if ( pWnd && pWnd->IsWndLessWindow())
	{
		// tab  所属窗口DC
		CClientDC dc( CWnd::FromHandle( GetCenter()->GetHost() ));
		CRect window;
		pWnd->GetRectInHost(&window);	
		// Draw tab
		CRect rcItem;
		GetDrawItemRect( uDragIndex, rcItem );
		//ClientToHost( rcItem );

		LOG_DEBUG( L"GetDragBitmap: rcItem " << rcItem.left << L", " << rcItem.top << L", " << rcItem.right << L", " << rcItem.bottom );


		rBmp.CreateCompatibleBitmap( &dc, window.Width() / 2, window.Height() / 2 + rcItem.Height() / 2 );
		CDC memDC2;
		memDC2.CreateCompatibleDC( &dc );
		CBitmap* pOldBmp = memDC2.SelectObject( &rBmp );

		memDC2.StretchBlt( 0, 0, rcItem.Width() / 2, rcItem.Height() / 2, &dc, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SRCCOPY );
		// copy tab  所属窗口镜像
		memDC2.StretchBlt( 0, rcItem.Height() / 2, window.Width() / 2, window.Height() / 2, &dc, window.left, window.top, window.Width(), window.Height(), SRCCOPY );
		memDC2.SelectObject( pOldBmp );
	}
	return rBmp;
}

void WLDragTabCtrlImp::OnDragItemMove( LPNMWLHDR lpNMWLHDR )
{
	LPNMWLMOVEDINFO pDragMoveInfo = (LPNMWLMOVEDINFO)lpNMWLHDR;
	if ( m_bBeginDrag )
	{
		CSize szOffset;
		m_pSelItem->GetDragOffset( szOffset );
		CPoint point = CPoint( pDragMoveInfo->ptNew ) + szOffset;
		CRect rcThis;
		GetRectInClient( &rcThis );
		if ( !rcThis.PtInRect( point ))
		{
			if ( !m_bDragging )
			{	
				BeginDrag( point );	
				m_bDragging = TRUE;
			}
		}
		else
		{
			__super::OnDragItemMove( lpNMWLHDR );
		}
	}
	else
	{
		__super::OnDragItemMove( lpNMWLHDR );
	}
}

void WLDragTabCtrlImp::OnLDownItem( LPNMWLHDR lpNMWLHDR )
{
	__super::OnLDownItem( lpNMWLHDR );
	m_bBeginDrag = TRUE ;
	m_bDragging = FALSE;
}

void WLDragTabCtrlImp::RecalcLayout()
{
	if (m_vecItems.empty())
	{
		UpdateView();
		return ;
	}

	DWORD dwStyle = GetStyle();

	CRect rcClient ;
	GetRectInClient(rcClient) ;

	CSize szAddBtn;
	m_btnAdd->GetSize( &szAddBtn );

	int nItemsWidth = rcClient.Width() - szAddBtn.cx - m_nAddBtnSpace;

	int nItemCount = m_vecItems.size() ;

	int nBound = 0 ;
	if (dwStyle & TS_AUTOADJUSTBOUND)
	{
		if (!(dwStyle & TS_VERTICAL))
		{
			nBound = (nItemsWidth - m_nNearSpace - m_nFarSpace - (nItemCount - 1) * m_nItemSpace) / nItemCount ;
		}
		else
		{
			nBound = (nItemsWidth - m_nNearSpace - m_nFarSpace - (nItemCount - 1) * m_nItemSpace) / nItemCount ;
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
				nNextPos = ( nItemsWidth + m_nItemSpace - m_nNearSpace - m_nFarSpace)  / nItemCount * (i + 1) + m_nNearSpace ;
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

		m_btnAdd->SetWndLessPos( ptItemTmp.x + szItemTmp.cx + m_nAddBtnSpace,
			ptItemTmp.y +  szItemTmp.cy - szAddBtn.cy , 0 , 0, SWLP_X | SWLP_Y | SWLP_NOREDRAW );

	}
	UpdateView();
}

void WLDragTabCtrlImp::OnDestroy()
{
	if ( m_btnAdd )
	{
		m_btnAdd->Destroy();
		delete m_btnAdd;
		m_btnAdd = NULL;
	}
	__super::OnDestroy();
}

void WLDragTabCtrlImp::SetAddBtnSpace( int nAddBtnSpace )
{
	m_nAddBtnSpace = nAddBtnSpace;
	RecalcLayout();
}

void WLDragTabCtrlImp::SetAddBtnTipString( const CString& tipString )
{
	if ( m_btnAdd )
	{
		m_btnAdd->UpdateTooltip( tipString );
	}
}

BOOL WLDragTabCtrlImp::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult )
{
	BOOL bRes = __super::OnWndMsg( message, wParam, lParam, pResult );
	if ( bRes )
	{
		return bRes;
	}
	if ( message == WM_COMMAND )
	{
		WORD id = LOWORD(wParam) ;
		WORD code = HIWORD(wParam) ;
		// menu and accelerator command
		if ( lParam == CN_COMMAND )
		{
			// 菜单命令消息
		}
		else	// control command
		{
			switch( id )
			{
			case CTRL_ID_BTN_ADD:
				{
					if ( code == BN_CLICKED )
					{
						NMWLHDR item ;
						item.hWlFrom = this ;
						item.idFrom = GetID() ;
						item.code = (UINT)TCN_ADD_BN_CLICKED;
						GetParent()->SendMessage( WM_NOTIFY, GetID(), LPARAM(&item));
						bRes = TRUE;
					}
				}
				break;
			}
		}
	}
	else if ( message == WM_NOTIFY )
	{
		LPNMWLHDR lpNMWLHDR = (LPNMWLHDR)lParam ;
		if (lpNMWLHDR->code == TCN_CLOSE_BN_CLICKED )
		{
			WLDragTabItem* pItem = dynamic_cast<WLDragTabItem*>(lpNMWLHDR->hWlFrom);
			NMWLHDR item ;
			item.hWlFrom = this ;
			item.idFrom = pItem->GetIndex() ;
			item.code = TCN_CLOSE_BN_CLICKED;
			GetParent()->SendMessage( WM_NOTIFY, GetID(), LPARAM( &item ));
			return TRUE ;
		}
		else
		{

		}
	}
	return bRes;
}

UINT WLDragTabCtrlImp::GetMinimizeSize()
{
	UINT nMinSize = 0;
	CSize szBtnAdd( 0, 0 );
	if ( m_btnAdd )
	{
		m_btnAdd->GetSize( &szBtnAdd );
	}
	nMinSize = szBtnAdd.cx - m_nAddBtnSpace + m_nNearSpace + m_nFarSpace;

	if ( m_vecItems.size() > 0 )
	{
		UINT nItemSize = m_nTabItemMinSize;
		if ( nItemSize <= 0 )
		{
			nItemSize = m_vecItems[0]->GetMinimizeSize();
		}			
		nMinSize += ( nItemSize + m_nItemSpace ) * ( m_vecItems.size() - 1 );
	}
	return nMinSize;
}

WLTabItem* WLDragTabCtrlImp::CreateTabItem( LPCTSTR lpszText, UINT uTextStyle, CPoint ptText, LPCTSTR sTipText, Shared_ImageEx pImgIcon, Shared_ImageEx pImgBkgnd, BOOL bDrag, int nTabState )
{
	WLTabItem *pItem =  new WLDragTabItem( lpszText, uTextStyle, ptText, sTipText, pImgIcon, pImgBkgnd, bDrag, nTabState );
	pItem->SetWrapper(pItem);//no wrapper
	return pItem;
}

void WLDragTabCtrlImp::RepositionAfterDeleteItem( BOOL bResize, int nIndex, CSize &delItemSize )
{
	__super::RepositionAfterDeleteItem( bResize, nIndex, delItemSize );
	if ( !bResize )
	{	
		m_btnAdd->SetWndLessPos( -( delItemSize.cx + m_nItemSpace ), 0 ,0 ,0 , SWLP_X | SWLP_X_OFFSET );
	}
}

void WLDragTabCtrlImp::SetTabItemMinSize( int nMinSize )
{
	m_nTabItemMinSize = nMinSize;
}


template<>
UILIB_EXPORT WLDragTabCtrlImpBase *CreateDefaultImp< WLDragTabCtrlImpBase >(IWLDragTabCtrl * wrapper)

{
	WLDragTabCtrlImp *ret = new WLDragTabCtrlImp(TRUE);
	ret->SetWrapper(wrapper);
	return ret;
};
