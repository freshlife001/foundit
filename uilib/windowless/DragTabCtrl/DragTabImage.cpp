// DragImage.cpp : 实现文件
//

#include "stdafx.h"

#include "logex.h"
#include "common/MemoryLeakChecker.h"

#include "DragTabImage.h"

LPCTSTR const ST_DRAGIMAGE = TEXT("St.DragImage");

IMPLEMENT_DYNAMIC(CDragTabImage, CWnd)

CDragTabImage::CDragTabImage()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, ST_DRAGIMAGE, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_OWNDC ;
		wndcls.lpfnWndProc		= ::DefWindowProc ;					// 关联消息处理函数
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0 ;
		wndcls.hInstance		= hInst ;							// 实例句柄
		wndcls.hIcon			= NULL;								// 图标
		wndcls.hCursor			= ::LoadCursor( NULL, IDC_ARROW ) ;	// 光标
		wndcls.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH) ;
		// 画刷(空画刷，不删除背景)
		wndcls.lpszMenuName		= NULL ;
		wndcls.lpszClassName	= ST_DRAGIMAGE ;					// 类名称

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}

	m_pdcImage = NULL ;
	m_pOldImageBmp = NULL ;
}

CDragTabImage::~CDragTabImage()
{
}

BOOL CDragTabImage::Create(
						CDC * pDC,
						CRect const &rcInDC,			// 所截取的内容在dc中的范围
						CWnd * pParent,
						CPoint const &ptInScreen,		// 所截取的内容在屏幕中的位置
						CPoint ptDragInScreen			// 鼠标拖拽点在屏幕中的位置
						) 
{
	CRect rcWnd ;
	rcWnd.SetRect(ptInScreen, ptInScreen + rcInDC.Size()) ;
	BOOL bRes = CreateEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		ST_DRAGIMAGE,
		NULL,
		WS_POPUP,
		rcWnd,
		pParent, 0) ;

	if (bRes)
	{
		m_pdcImage = new CDC ;
		m_pdcImage->CreateCompatibleDC(pDC) ;
		CBitmap bmp ;
		bmp.CreateCompatibleBitmap(pDC, rcWnd.Width(), rcWnd.Height()) ;
		m_pOldImageBmp = m_pdcImage->SelectObject(&bmp) ;
		m_pdcImage->BitBlt(0, 0, rcInDC.Width(), rcInDC.Height(), pDC, rcInDC.left, rcInDC.top, SRCCOPY) ;
		m_szOffset = ptInScreen - ptDragInScreen ;

		SetLayeredWindowAttributes(0, 155, LWA_ALPHA) ;
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW) ;
	}

	return bRes ;
}

BOOL CDragTabImage::Create( CBitmap* pBmp, /* 拖动图标的内容 */
						CSize const &szItem,			// 所截取的内容在dc中的范围
						CWnd * pParent,
						CPoint const &ptInScreen, /* 所截取的内容在屏?中的?置 */
						CPoint ptDragInScreen /* 鼠标拖拽点在屏?中的?置 */ )
{
	if ( pBmp == NULL )
	{
		return FALSE;
	}
	BITMAP bmpInfo;
	pBmp->GetBitmap( &bmpInfo );
	CRect rcWnd ;
	CSize szWnd( bmpInfo.bmWidth, bmpInfo.bmHeight );
	rcWnd.SetRect( ptInScreen, ptInScreen + szWnd ) ;
	BOOL bRes = CreateEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		ST_DRAGIMAGE,
		NULL,
		WS_POPUP,
		rcWnd,
		pParent, 0) ;

	if (bRes)
	{
		CPaintDC dc(this);
		m_pdcImage = new CDC ;
		m_pdcImage->CreateCompatibleDC(&dc) ;
		m_pOldImageBmp = m_pdcImage->SelectObject( pBmp ) ;

		m_szOffset = ptInScreen - ptDragInScreen ;
		m_szOffset.cx /= 2;
		m_szOffset.cy /= 2;
		SetLayeredWindowAttributes(0, 155, LWA_ALPHA) ;
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW) ;

		CRgn rgn, rgnPixel;
		rgn.CreateRectRgn( 0 ,0, szWnd.cx, szWnd.cy );

		rgnPixel.CreateRectRgn( szItem.cx, 0, rcWnd.Width(), szItem.cy );
		rgn.CombineRgn( &rgn, &rgnPixel, RGN_XOR );
		rgnPixel.DeleteObject();

		SetWindowRgn( rgn, TRUE );
	}

	return bRes ;
}

void CDragTabImage::DoDragging(CPoint ptDrag) 
{
//	TRACE("CDragTabImage::DoDragging\n") ;
	if (!::IsWindow(GetSafeHwnd()))
		return ;

	CPoint ptNew = ptDrag + m_szOffset ;
	SetWindowPos(NULL, ptNew.x, ptNew.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE) ;
}

void CDragTabImage::EndDrag() 
{
//	TRACE("CDragTabImage::EndDrag\n") ;
	if (m_pdcImage != NULL)
	{
		m_pdcImage->SelectObject(m_pOldImageBmp) ;
		delete m_pdcImage ;
		m_pdcImage = NULL ;
	}
	DestroyWindow() ;
}


BEGIN_MESSAGE_MAP(CDragTabImage, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CDragTabImage 消息处理程序



void CDragTabImage::OnPaint()
{
// 	TRACE("CDragTabImage::OnPaint\n") ;
	CPaintDC dc(this);

	CRect rcClient ;
	GetClientRect(rcClient) ;
	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), m_pdcImage, 0, 0, SRCCOPY) ;

// 	dc.MoveTo(0, 0) ;
// 	dc.LineTo(rcClient.BottomRight()) ;
}

BOOL CDragTabImage::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE ;
}

SCODE CDragTabImage::GiveFeedback( DROPEFFECT dropEffect )
{
	if ( dropEffect == DROPEFFECT_MOVE )
	{
		ShowWindow( SW_HIDE );
	}
	else
	{
		ShowWindow( SW_SHOW );
	}
	return S_OK;
}

SCODE CDragTabImage::QueryContinueDrag( BOOL bEscapePressed, DWORD dwKeyState )
{
	if ( bEscapePressed ) {
		return DRAGDROP_S_CANCEL;
	}

	if (!(dwKeyState & MK_LBUTTON)) {
		return DRAGDROP_S_DROP;
	}

	::SetCursor( ::LoadCursor( NULL, IDC_ARROW ));

	CPoint pt;
	GetCursorPos( &pt );
	DoDragging( pt );
	return S_OK;
}
