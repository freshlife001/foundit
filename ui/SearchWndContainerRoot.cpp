#include "StdAfx.h"
#include "uilib/common/wnds/Menu/hdmenu.h"
#include "uilib/common/utils/ImageEx/ImageEx.h"
#include "MessageDefine.h"
#include "SearchWnd.h"
#include "SearchWndContainerRoot.h"
#include "logex.h"


const UINT_PTR FLASH_TIMER_ID = 1;
const int FLASH_TIMES = 5 * 2;				//因为一次闪烁指的是从一个状态转换到另一个状态（系统），但对我们来说 是亮一次 
const int FLASH_TIME_SPACE = 495;

const int DEFAULT_TAB_WIDTH = 150;
const int TAB_ITEM_SPACE = -10;
const int TAB_ADD_BTN_SPACE = 1;
const int TAB_ITEM_MIN_SIZE = 50;
const int TAB_TOP_INTERVAL = 7;// 离顶部高度
const int TAB_STATES_COUNT = 5;

const int FRAME_EDGE_HORIZENTAL = 1;
const int FRAME_EDGE_BOTTOM = 1;

CSearchWndContainerRoot::CSearchWndContainerRoot(void)
{
	m_imgTabItemBk = Shared_ImageEx::FromId( IDI_DRAG_TAB_ITEM_PNG );
	m_imgTabBg = Shared_ImageEx::FromId( IDI_DRAG_TAB_BKGND_PNG );
	m_wnd_style = 0;
	m_bDrawTitle = FALSE;
}


CSearchWndContainerRoot::~CSearchWndContainerRoot(void)
{
}


HRESULT CSearchWndContainerRoot::Add( IWndLess* pWnd, int nIndexAfter/*=-1*/, BOOL bActiveNewItem )
{
	LOG_DEBUG( "add wnd: " << pWnd );
	if ( pWnd == NULL || !pWnd->IsWndLessWindow())
	{
		ASSERT( FALSE );
		return E_FAIL;
	}

	if ( !bActiveNewItem )
	{
		pWnd->SetWndLessPos(0,0,0,0,SWLP_HIDE);
	}
	AddChild(pWnd);

	Shared_ImageEx imageIcon;
	CString csTitleFull;
	pWnd->GetWindowText(csTitleFull);
	int nItem = m_tabChatDlgs.InsertItem( nIndexAfter, imageIcon, m_imgTabItemBk, TAB_STATES_COUNT, csTitleFull, 0x00000010, 10, 0, csTitleFull, DEFAULT_TAB_WIDTH );
	CRect rcItem;
	if( m_tabChatDlgs.GetItemRect( nItem, rcItem ))
	{
		CSize size( 16, 16 );
		m_tabChatDlgs.SetItemIconSize( nItem, size );
	}

	CRect rcTabCtrl;
	m_tabChatDlgs.GetRectInParent( &rcTabCtrl );
	m_tabChatDlgs.SetItemData( nItem, (DWORD_PTR)pWnd );

	if ( bActiveNewItem )
	{
		m_tabChatDlgs.SelectItem( nItem );
	}

	MINMAXINFO maxminInfo;
	OnGetMinMaxInfo(  &maxminInfo );

	CRect rcClient;

	GetRectInClient( rcClient );

	int nWidth = max( rcClient.Width(), maxminInfo.ptMinTrackSize.x );
	::SetWindowPos( GetHost(), NULL, 0, 0, 
		nWidth,
		max( rcClient.Height(), maxminInfo.ptMinTrackSize.y  ), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW );

	m_tabChatDlgs.SetWndLessPos(  0, 0, nWidth - 100, m_imgTabItemBk->GetHeight(), SWLP_CX | SWLP_CY );


	
	GetRectInClient( &rcClient );
	rcClient.DeflateRect( FRAME_EDGE_HORIZENTAL, rcTabCtrl.bottom, FRAME_EDGE_HORIZENTAL, FRAME_EDGE_BOTTOM );
	pWnd->SetWndLessPos( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWLP_X|SWLP_Y|SWLP_CX|SWLP_CY);

	UpdateTitle();
	UpdateView();

	return S_OK;
}

HRESULT CSearchWndContainerRoot::Remove( int nIndex, BOOL bResizeTab, BOOL bNeedDestroy )
{
	DWORD_PTR dwData = m_tabChatDlgs.GetItemData( nIndex );
	IWndLess* pWnd = (IWndLess*)dwData;
	ASSERT( pWnd );
	if ( pWnd && pWnd->IsWndLessWindow())
	{
		DelChild(pWnd);
		m_tabChatDlgs.SetItemData( nIndex, NULL );
	}
	m_tabChatDlgs.DeleteItem( nIndex, bResizeTab );
	if ( m_tabChatDlgs.GetItemCount() <=0 && bNeedDestroy )
	{
		::DestroyWindow( GetHost() );
	}
	else
	{
		UpdateTitle();
	}

	return S_OK;
}



BOOL CSearchWndContainerRoot::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;
	switch ( message )
	{
	case WM_ERASEBKGND:
		{
			*pResult = 1 ;
			bRes = TRUE ;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			::PostMessage(GetHost(), WM_NCLBUTTONDOWN, HTCAPTION, 0) ;
			bRes = TRUE;
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if ( !::IsZoomed( GetHost() ))
			{
				::SendMessage(GetHost(), WM_SYSCOMMAND, SC_MAXIMIZE, 0 ) ;
			}
			else
			{
				::SendMessage(GetHost(), WM_SYSCOMMAND, SC_RESTORE, 0 ) ;
			}
			bRes = TRUE;
		}
		break;
// 	case WM_GETFONT :
// 		{
// 			CFont* pFont = AfxGetApp()->GetMainWnd()->GetFont();
// 			*pResult = (LRESULT)pFont->GetSafeHandle() ;
// 			bRes = TRUE ;
// 		}
// 		break ;
	case WM_COMMAND :
		{
			bRes = OnCommand(wParam, lParam, pResult ) ;
		}
		break ;

	case WM_NOTIFY :
		{
			bRes = OnNotify(wParam, lParam, pResult) ;
		}
		break ;
	case MSG_SKIN_CHANGED :
		{
			//CalcBkgndGraySale() ;
			//if ( m_bkgnd_gray_scale < 192 )
			//{
			//	m_tabChatDlgs.SetTextColor( RGB( 220, 220, 220 ));
			//	m_tabChatDlgs.SetSelectTextColor( RGB( 255, 255, 255 ) ) ;
			//}
			//else
			//{
			//	m_tabChatDlgs.SetTextColor( RGB( 51, 51, 51 ));
			//	m_tabChatDlgs.SetSelectTextColor( RGB( 0, 0, 0 ) ) ;
			//}
			//DrawBkgnd() ;
			//UpdateView() ;
			bRes = TRUE;
		}
		break ;
	case WM_NCACTIVATE:
		{
			*pResult = 1 ;
			bRes = TRUE;
		}
		break;
	case WM_ACTIVATE:
		{
			OnActivate(LOWORD(wParam),
				CWnd::FromHandle(reinterpret_cast<HWND>(lParam)), HIWORD(wParam)) ;
			bRes = TRUE;
		}
		break;
	case WM_TIMER:
		{
			OnTimer( wParam );
			bRes = TRUE;
		}
		break;
	case WM_GETMINMAXINFO:
		{
			OnGetMinMaxInfo((MINMAXINFO*)lParam) ;
			bRes = TRUE;
		}
		break;

	default:
		bRes = BaseWndLessRoot::OnWndMsg( message, wParam, lParam, pResult );
		break;
	}
	return bRes;
}
SIZE CSearchWndContainerRoot::GetDefaultSize()
{
	int tabHeight = m_imgTabItemBk->GetHeight();
	DWORD dwDefaultSize = MAKELONG( 500, 420 + tabHeight );
	DWORD dwSize = dwDefaultSize;
	SIZE ret;
	ret.cx = LOWORD( dwSize );
	ret.cy = HIWORD( dwSize );
	return ret;
}

BOOL CSearchWndContainerRoot::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent )
{
	BaseWndLessRoot::OnCreate( dwStyle, x, y, cx, cy, pParent );

	::SendMessage(GetHost(), WM_SETICON, TRUE, (LPARAM)m_hIcon);
	::SendMessage(GetHost(), WM_SETICON, FALSE, (LPARAM)m_hIcon);

	//GetFrame()->RegisterSkinWindow( GetHost() );

	EnableTooltips(TRUE) ;	

	m_tabChatDlgs.Create( WS_VISIBLE | WLTabCtrl::TS_AUTOADJUSTBOUND, 0, 0, 0, 0, this, IDC_DRAG_TAB );
	m_tabChatDlgs.SetSpace( 0, TAB_ITEM_SPACE, 0 );
	m_tabChatDlgs.SetAddBtnSpace( TAB_ADD_BTN_SPACE );
	m_tabChatDlgs.SetTabItemMinSize( TAB_ITEM_MIN_SIZE );

	m_tabChatDlgs.SetWndLessPos( 4, TAB_TOP_INTERVAL, cx - 100, m_imgTabItemBk->GetHeight(),  SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY );


	CRect rcTab;
	m_tabChatDlgs.GetRectInHost( rcTab );
	m_title_height = rcTab.bottom;

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CSearchWndContainerRoot::OnSized( SIZE const& szOld, SIZE const& szNew )
{
	if ( m_tabChatDlgs.GetItemCount() > 0 )
	{
		int nTopInterval = ::IsZoomed( GetHost() ) ? TAB_TOP_INTERVAL - 2 : TAB_TOP_INTERVAL;
		m_tabChatDlgs.SetWndLessPos(  0, nTopInterval, szNew.cx - 100, m_imgTabItemBk->GetHeight(), SWLP_Y | SWLP_CX | SWLP_CY );
		CRect rcTab;
		m_tabChatDlgs.GetRectInHost( rcTab );
		m_title_height = rcTab.bottom;
	}
	BaseWndLessRoot::OnSized( szOld, szNew );
	if ( m_tabChatDlgs.GetItemCount() > 0 )
	{
		int curSel = m_tabChatDlgs.GetCurSel();
		UpdateTabWndRect(curSel);
	}
}


BOOL CSearchWndContainerRoot::OnCommand(WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	return __super::OnCommand( wParam, lParam, pResult );;
}
BOOL CSearchWndContainerRoot::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
	BOOL bRes = FALSE ;
	switch (wParam)
	{
	case IDC_DRAG_TAB :
		{
			LPNMWLHDR pNMHDR = (LPNMWLHDR)lParam ;
			switch( pNMHDR->code )
			{
			case TCN_SELCHANGE:
				{
					OnTabSelChange( pNMHDR, pResult );
				}
				break;
			case TCN_TAB_DRAG_BEGIN:
			case TCN_TAB_DRAG_LEAVE:
				{
					OnRemoveDragItem( pNMHDR, pResult );
				}
				break;
			case TCN_DROP_TAB_ITEM:
				{
					OnDropTabItem( pNMHDR, pResult );
				}
				break;
			case TCN_TAB_DRAG_ENTER:
				{
					OnTabDragEnter( pNMHDR, pResult );
				}
				break;
			case TCN_ADD_BN_CLICKED:
				{
					OnTabAddNewItem( pNMHDR, pResult );
				}
				break;
			case TCN_CLOSE_BN_CLICKED:
				{
					OnTabCloseItem( pNMHDR, pResult );
				}
				break;
			case NM_RCLICK:
				{
					OnTabRClick( pNMHDR, pResult );
				}
				break;
			}
			bRes = TRUE;
		}
		break;
	default:
		bRes = __super::OnNotify( wParam, lParam, pResult );
		break;
	}
	return bRes;
}
BOOL CSearchWndContainerRoot::OnKeyDown(	UINT nChar,	UINT nRepCnt,	UINT nFlags )
{
	BOOL bRes = FALSE;
	if ( nChar == VK_TAB )
	{
		BOOL bCtrl = ( GetKeyState(VK_CONTROL) & 0x80 );
		BOOL bShift = ( GetKeyState(VK_SHIFT) & 0x80 );
		if ( bCtrl || bShift )
		{
			int nItemCount = m_tabChatDlgs.GetItemCount();
			if ( nItemCount > 1 )
			{
				int nCurSel = m_tabChatDlgs.GetCurSel();
				if ( bShift)
				{
					nCurSel = ( nCurSel - 1 + nItemCount ) % nItemCount;
				}
				else
				{
					nCurSel = ( nCurSel + 1 ) % nItemCount;
				}
				m_tabChatDlgs.SelectItem( nCurSel );
			}
			bRes = TRUE;
		}
	}
	else if ( nChar == _T('T') )
	{
		BOOL bCtrl = ( GetKeyState(VK_CONTROL) & 0x80 );
		BOOL bAlt = ( GetKeyState(VK_MENU) & 0x80 );
		if ( bCtrl && !bAlt )
		{

			bRes = TRUE;
		}
	}
	else if ( nChar == _T('N'))
	{
		BOOL bCtrl = ( GetKeyState(VK_CONTROL) & 0x80 );
		BOOL bAlt = ( GetKeyState(VK_MENU) & 0x80 );
		if ( bCtrl && !bAlt )
		{
			bRes = TRUE;
		}
	}
	else if( nChar == _T('W') || nChar == VK_F4 )
	{
		BOOL bCtrl = ( GetKeyState(VK_CONTROL) & 0x80 );
		BOOL bAlt = ( GetKeyState(VK_MENU) & 0x80 );
		if ( bCtrl && !bAlt )
		{
			bRes = TRUE;
		}
	}
	else if ( nChar == VK_ESCAPE )
	{
		bRes = TRUE;
	}
	return bRes;
}

void CSearchWndContainerRoot::OnTabRClick( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	LPNMWLTABITEM pNMTab = (LPNMWLTABITEM)pNMHDR;
	int nIndex = pNMTab->nNewItem;
	if ( nIndex >= 0 && nIndex < m_tabChatDlgs.GetItemCount() )
	{
		HHDMENU hMenu = hdui::CreatePopupMenu() ;
		hdui::AppendMenu(hMenu, MF_BYCOMMAND | MF_STRING, IDM_CLOSE_ME, _T("关闭当前")) ;
		hdui::AppendMenu(hMenu, MF_BYCOMMAND | MF_STRING, IDM_CLOSE_OTHERS, _T("关闭其他")) ;

		CPoint point;
		GetCursorPos( &point );

		int nCommand = hdui::TrackPopupMenu(
			hMenu,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
			point.x, point.y,
			GetCenter()->GetHost(),
			NULL
			) ;
		hdui::DestroyMenu( hMenu );

		switch( nCommand )
		{
		case IDM_CLOSE_ME:
			{
				Remove( nIndex );
			}
			break;
		case IDM_CLOSE_OTHERS:
			{
				for ( int i = m_tabChatDlgs.GetItemCount() - 1; i >= 0;  --i )
				{
					if ( i != nIndex )
					{
						Remove( i );
					}
				}
			}
			break;
		}
	}
}


void CSearchWndContainerRoot::OnTimer(UINT_PTR nIDEvent )
{
	switch ( nIDEvent )
	{
	case FLASH_TIMER_ID:
		{
			FLASHWINFO fwi ;
			fwi.cbSize = sizeof(fwi) ;
			fwi.hwnd = GetHost() ;
			fwi.uCount = 1 ;
			fwi.dwFlags = FLASHW_STOP;
			fwi.dwTimeout = FLASH_TIME_SPACE ;
			::FlashWindowEx(&fwi) ;

			KillTimer(FLASH_TIMER_ID);
		}
		break;
	default:
		break;
	}
	
}
void CSearchWndContainerRoot::OnDestroy()
{
	m_tabChatDlgs.Destroy();

	__super::OnDestroy();
}
void CSearchWndContainerRoot::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rcWorkArea ;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) ;
	lpMMI->ptMaxPosition.x = rcWorkArea.left - 2 ;
	lpMMI->ptMaxPosition.y = rcWorkArea.top - 2 ;
	lpMMI->ptMaxSize.x = rcWorkArea.Width() + 4 ;
	lpMMI->ptMaxSize.y = rcWorkArea.Height() + 4;

	lpMMI->ptMinTrackSize.x = 120 + (TAB_ITEM_MIN_SIZE + TAB_ITEM_SPACE) * m_tabChatDlgs.GetItemCount() ;
	lpMMI->ptMinTrackSize.y = 120 ;
// 	if ( m_tabChatDlgs.GetItemCount() > 0 )
// 	{
// 		CDlgMainChat* pCurDlg = GetCurDlg();
// 		if ( pCurDlg )
// 		{
// 			CSize minSize = pCurDlg->GetMinimizeSize();
// 			int nTitleBarMinsize = m_tabChatDlgs.GetMinimizeSize() + 100;
// 			if ( minSize.cx <  nTitleBarMinsize )
// 			{
// 				minSize.cx = nTitleBarMinsize;
// 			}
// 			lpMMI->ptMinTrackSize.x = minSize.cx;
// 			lpMMI->ptMinTrackSize.y = minSize.cy + m_title_height;
// 		}
// 		else
// 		{
// 			AddContactTabWnd* pWnd = (AddContactTabWnd*)GetCurWnd();
// 			CSize minSize = pWnd->GetMinimizeSize();
// 			int nTitleBarMinsize = m_tabChatDlgs.GetMinimizeSize() + 100;
// 			if ( minSize.cx <  nTitleBarMinsize )
// 			{
// 				minSize.cx = nTitleBarMinsize;
// 			}
// 			lpMMI->ptMinTrackSize.x = minSize.cx;
// 			lpMMI->ptMinTrackSize.y = minSize.cy;
// 		}
// 	}

}

void CSearchWndContainerRoot::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	{
		FLASHWINFO fwi ;
		fwi.cbSize = sizeof(fwi) ;
		fwi.hwnd = GetHost() ;
		fwi.uCount = 0 ;
		fwi.dwFlags = FLASHW_STOP;
		fwi.dwTimeout = FLASH_TIME_SPACE ;
		::FlashWindowEx(&fwi) ;
		KillTimer(FLASH_TIMER_ID);
	}

	if ( !::IsIconic( GetHost() ))
	{

	}
}
LRESULT CSearchWndContainerRoot::OnSkinChanged(WPARAM, LPARAM)
{
	IWndLess* dlg = GetCurWnd() ;
	if (dlg != NULL && dlg->IsWndLessWindow())
	{
		dlg->SendMessage(MSG_SKIN_CHANGED, 0, 0) ;
	}
	return 0 ;
}

LRESULT CSearchWndContainerRoot::OnTabTitleChange( WPARAM, LPARAM )
{
	return 0;
}
void CSearchWndContainerRoot::OnTabSelChange(NMWLHDR * pNMHDR, LRESULT * pResult)
{
	LPNMWLTABITEM pItem = (LPNMWLTABITEM)pNMHDR ;

	IWndLess* pOldWnd = (IWndLess*)pItem->dwOldItemData;
	IWndLess* pNewWnd = (IWndLess*)pItem->dwNewItemData;
	if (pOldWnd)
	{
		pOldWnd->SetWndLessPos(0, 0, 0, 0, SWLP_HIDE);
	}
	if (pNewWnd)
	{
		pNewWnd->SetWndLessPos(0, 0, 0, 0, SWLP_SHOW);
	}


}
void CSearchWndContainerRoot::OnRemoveDragItem( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	WLDragTabCtrl::LPNMWLTABDRAGEVENT pDragItem = (WLDragTabCtrl::LPNMWLTABDRAGEVENT)pNMHDR;

	m_tabChatDlgs.DeleteItem( pDragItem->nEnterIndex );

	LOG_DEBUG_C( L"OnRemoveDragItem:"  << pDragItem->hdr.hdr.code );

	if ( m_tabChatDlgs.GetItemCount() <= 0 )
	{
		::ShowWindow( GetHost(), SW_HIDE );
	}
	else
	{
		UpdateTitle();
	}
}
void CSearchWndContainerRoot::OnDropTabItem( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	WLDragTabCtrl::LPNMWLTABDRAGEVENT pDropItem = (WLDragTabCtrl::LPNMWLTABDRAGEVENT)pNMHDR;

	if ( pDropItem->nEnterIndex == DROPEFFECT_NONE )
	{
		CPoint pt;
		GetCursorPos( &pt );

		pt.Offset( -pDropItem->hdr.dragParam.m_ptOffset );

		CRect rcTab;
		m_tabChatDlgs.GetRectInParent( rcTab );
		pt.Offset( -rcTab.left, -rcTab.top );

		DWORD_PTR dwData = pDropItem->hdr.dragParam.m_dwData;
		IWndLess* pWnd = (IWndLess*)dwData;
		Add( pWnd, -1, TRUE );
		if ( !::IsWindowVisible( GetHost() ))
		{
			::SetWindowPos( GetHost(), NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
		}
	}
	else
	{
		if ( !::IsWindowVisible( GetHost() ) && m_tabChatDlgs.GetItemCount() <= 0 )
		{
			::DestroyWindow( GetHost() );
		}
	}
}
void CSearchWndContainerRoot::OnTabDragEnter( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	WLDragTabCtrl::LPNMWLTABDRAGEVENT pDragItem = (WLDragTabCtrl::LPNMWLTABDRAGEVENT)pNMHDR;
	DWORD_PTR dwData = pDragItem->hdr.dragParam.m_dwData;
	if ( dwData != NULL )
	{
		IWndLess* pWnd = (IWndLess*)dwData;
		Add( pWnd, pDragItem->nEnterIndex, TRUE );
	}
}
void CSearchWndContainerRoot::OnTabAddNewItem( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	IWndLess *newWnd = CreateSearchWnd();
	Add(newWnd, -1, TRUE);
}
void CSearchWndContainerRoot::OnTabCloseItem( NMWLHDR * pNMHDR, LRESULT * pResult )
{
	int nIndex = pNMHDR->idFrom;

	IWLDragTabCtrl* pDragTabCtrl = dynamic_cast<IWLDragTabCtrl *> (pNMHDR->hWlFrom);

	ASSERT( nIndex >= 0 && nIndex <= pDragTabCtrl->GetItemCount() );

	Remove( nIndex, FALSE );	
}


void CSearchWndContainerRoot::UpdateTabWndRect( int curSel )
{
	if (curSel >= 0)
	{
		CRect rcItem;
		if( m_tabChatDlgs.GetItemRect( curSel, rcItem ))
		{
			IWndLess* pWnd = (IWndLess*)m_tabChatDlgs.GetItemData( curSel );
			CRect rcClient;
			GetRectInClient( &rcClient );

			int nChatTop = GetTitleHeight();
			if ( m_tabChatDlgs.IsWndLessVisible() ) // 不区分单tab和多tab
			{
				CRect rcTabCtrl;
				m_tabChatDlgs.GetRectInParent( &rcTabCtrl );
				nChatTop = rcTabCtrl.bottom;
			}
			else
			{
				CRect rcTabCtrl;
				m_tabChatDlgs.GetRectInParent( &rcTabCtrl );
				nChatTop = rcTabCtrl.bottom - 2;
			}
			rcClient.DeflateRect( FRAME_EDGE_HORIZENTAL, nChatTop, FRAME_EDGE_HORIZENTAL, FRAME_EDGE_BOTTOM );
			pWnd->SetWndLessPos(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWLP_X|SWLP_Y|SWLP_CX|SWLP_CY);

			UINT nType = SIZE_RESTORED;
			if ( ::IsZoomed( GetHost() ) )
			{
				nType = SIZE_MAXIMIZED;
			}
			else if ( ::IsIconic( GetHost() ) )
			{
				nType = SIZE_MINIMIZED;
			}
		}
	}
}


void CSearchWndContainerRoot::UpdateTitle()
{
	int nItemCount = m_tabChatDlgs.GetItemCount();
	if ( nItemCount > 1 )
	{
		CString csTitle;
		csTitle.Format( L"找到(%d)", nItemCount );
		::SetWindowText( GetHost(), csTitle );
		DrawBkgnd();
	}
}



IWndLess* CSearchWndContainerRoot::GetCurWnd()
{
	IWndLess* pSel = NULL;
	int nCurSel = m_tabChatDlgs.GetCurSel();
	if ( nCurSel >= 0 )
	{
		DWORD_PTR dwData = m_tabChatDlgs.GetItemData( nCurSel );
		pSel = (IWndLess*)dwData;
	}
	return pSel;
}

IWndLess* CSearchWndContainerRoot::CreateSearchWnd()
{
	CSearchWnd *newWnd = new CSearchWnd();
	static UINT s_wndId = 100;
	CRect rcClient;
	GetRectInClient( &rcClient );
	newWnd->Create(WS_VISIBLE, 0, 0, rcClient.Width(), rcClient.Height() - m_title_height, this, s_wndId++);
	return dynamic_cast<IWndLess*>(newWnd);
};