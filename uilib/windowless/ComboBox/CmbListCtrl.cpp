// CmbListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "CmbListCtrl.h"
#include "common/utils/Graphics.h"
#include "common/MemoryLeakChecker.h"

// CCmbListCtrl

IMPLEMENT_DYNAMIC(CCmbListCtrl, CWnd)

CCmbListCtrl::CCmbListCtrl() : m_pImgDelBtn(NULL)
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, ST_CMBLISTCTRL, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS ;
		wndcls.lpfnWndProc		= ::DefWindowProc ;					// 关联消息处理函数
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0 ;
		wndcls.hInstance		= hInst ;							// 实例句柄
		wndcls.hIcon			= (HICON)0 ;						// 图标
		wndcls.hCursor			= ::LoadCursor( NULL, IDC_ARROW ) ;	// 光标
		wndcls.hbrBackground	= (HBRUSH)(0) ;						// 画刷(空画刷，不删除背景)
		wndcls.lpszMenuName		= NULL ;
		wndcls.lpszClassName	= ST_CMBLISTCTRL ;					// 类名称

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}

	m_pResData = NULL ;

	// 初始化属性
	m_crFrame = RGB(0, 147, 209) ;

	m_ftFontNormal.CreateFont(
		16,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH,				// nPitchAndFamily
		_T("MS Reference sans serif")
		) ;

	m_crTextColorNormal = hdutils::crTextColor ;
	m_crBackColorNormal = RGB(255, 255, 255) ;
	m_crTextColorHovered = RGB(255, 255, 255) ;
	m_crBackColorHovered = RGB(108, 198, 255) ;

	// 由于边框占1个像素
	m_nItemPosOrigin	= 1 ;
	m_nTrackItemIndex	= -1 ;
	m_nTextOffsetLeft	= 4 ;
	m_nBtnOffsetRight	= 6 ;
	m_nBtnWidth			= 0 ;
	m_nBtnHeight		= 0 ;
	m_nBtnPushedItem	= -1 ;
	m_bBtnPushed		= FALSE ;

	m_bBeginHandleLBtnUp = FALSE ;

	m_nItemHeightNormal = 32 ;
}

CCmbListCtrl::~CCmbListCtrl()
{
}

int CCmbListCtrl::Popup(
						int x, int y, int cx, int cy,
						HWND hParent,
						RESDATA &res
						) 
{
	BOOL bRes = CWnd::CreateEx(
		WS_EX_TOPMOST, 
		ST_CMBLISTCTRL,
		NULL,
		WS_POPUP,
		x, y, cx, cy,
		hParent,
		NULL,
		0
		) ;
	m_rcClient.SetRect(0, 0, cx, cy) ;

	m_pResData = &res ;

	if (!bRes)
	{
		return -1 ;
	}

	AutoAdjustSize() ;
	SetWindowPos(
		NULL,
		0, 0, 0, 0,
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW
		) ;

	if (GetCapture() != this)
		SetCapture() ;

	m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL) ;
	MSG msg ;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_NULL)
		{
			if (!ContinueModal())
				break ;
		}
		else if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE || msg.wParam == VK_RETURN)
			{
				break ;
			}
		}

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	DestroyWindow() ;

	return 0 ;
}

void CCmbListCtrl::LoadImage(
							Image * pImgDelBtn
							) 
{
	if (pImgDelBtn == NULL || pImgDelBtn->GetLastStatus() != Ok)
		return ;

	m_pImgDelBtn = pImgDelBtn ;
	m_nBtnWidth = m_pImgDelBtn->GetWidth() / 3 ;
	m_nBtnHeight = m_pImgDelBtn->GetHeight() ;
}

// 添加字符串
int CCmbListCtrl::AddString(LPCTSTR lpszString) 
{
	// 数据添加
	LCITEM item(lpszString) ;
	if (m_vecItems.size() > 0)
	{
		item.m_nPosition = m_vecItems.back().m_nPosition + m_vecItems.back().m_nCurHeight ;
	}
	else
	{
		item.m_nPosition = 0 ;
	}
	item.m_nCurHeight = m_nItemHeightNormal ;
	item.m_nHeight = m_nItemHeightNormal ;

	m_vecItems.push_back(item) ;

	// 如果窗口句柄有效，则刷新界面
	if (::IsWindow(m_hWnd))
	{
		InvalidateRect(NULL, FALSE) ;
	}

	return m_vecItems.size() - 1 ;
}

// 函数功能：点击测试
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
int CCmbListCtrl::HitTest(CPoint point) 
{
	if (point.x < m_rcClient.left || point.x >= m_rcClient.right
		|| point.y < m_rcClient.top || point.y >= m_rcClient.bottom)
		return -1 ;

	VEC_LCITEM::iterator pos ;
	for (pos = m_vecItems.begin() ; pos != m_vecItems.end() ; ++ pos)
	{
		if (IsItemVisible(*pos))
			break ;
	}

	for (pos = m_vecItems.begin() ; pos != m_vecItems.end() ; ++ pos)
	{
		if (pos->m_nPosition + m_nItemPosOrigin <= point.y 
			&& pos->m_nPosition + m_nItemPosOrigin + pos->m_nCurHeight > point.y)
			return pos - m_vecItems.begin() ;
	}

	return -1 ;
}

// 函数功能：判断子项是否可见
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
BOOL CCmbListCtrl::IsItemVisible(UINT nIndex) 
{
	if (nIndex < 0 || nIndex >= m_vecItems.size())
		return FALSE ;

	return IsItemVisible(m_vecItems[nIndex]) ;
}

// 函数功能：判断子项是否可见
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
BOOL CCmbListCtrl::IsItemVisible(LCITEM const & item) 
{
	CRect rcClient ;
	GetClientRect(rcClient) ;

	if (item.m_nPosition + m_nItemPosOrigin >= rcClient.bottom
		|| item.m_nPosition + m_nItemPosOrigin + item.m_nCurHeight <= rcClient.top)
		return FALSE ;

	return TRUE ;
}

// 函数功能：跟踪子项
// 输入参数：nIndex为-1时表示清除选择
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
int CCmbListCtrl::SetCurSel(int nIndex) 
{
	if (nIndex < -1 || nIndex >= (int)m_vecItems.size())
		return CB_ERR ;
	//	if (nIndex == -1)
	//		return FALSE ;

	if (nIndex != m_nTrackItemIndex)
	{
		if (m_nTrackItemIndex != -1)
		{
			m_vecItems[m_nTrackItemIndex].m_uState = LCIS_NORMAL ;
		}

		if (nIndex != -1)
		{
			m_vecItems[nIndex].m_uState = LCIS_PRIHOVERED ;
		}

		m_nTrackItemIndex = nIndex ;
	}

	// 更新界面
	if (::IsWindow(GetSafeHwnd()))
		InvalidateRect(NULL, FALSE) ;

	return nIndex ;
}

// 设置子项高度
void CCmbListCtrl::SetItemHeight(int nHeight) 
{
	m_nItemHeightNormal = nHeight ;
}

// 函数功能：自适应大小
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
void CCmbListCtrl::AutoAdjustSize() 
{
	if (!IsWindow(m_hWnd))
		return ;

	if (!m_vecItems.empty())
	{
		CRect rcClient ;
		GetClientRect(rcClient) ;

		CClientDC dc(this) ;
		CFont * pOldFont = dc.SelectObject(&m_ftFontNormal) ;

		int nHeight = 2 ;	// 必须包含边框高度
		int nWidth = rcClient.Width() ;	// 记录最小宽度
		CSize szTemp ;
		VEC_LCITEM::iterator pos ;
		for (pos = m_vecItems.begin() ; pos != m_vecItems.end() ; ++ pos)
		{
			nHeight += pos->m_nCurHeight ;
			//			nHeight += m_nItemHeightHovered ;

			szTemp = dc.GetTextExtent(pos->m_strText) ;
			int item_width = szTemp.cx + m_nTextOffsetLeft + m_nBtnOffsetRight + m_nBtnWidth + m_nBtnOffsetRight + 2 ;
			if (nWidth < item_width)
				nWidth = item_width ;
		}

		dc.SelectObject(pOldFont) ;

		SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE) ;
	}
}


BEGIN_MESSAGE_MAP(CCmbListCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()



// CCmbListCtrl 消息处理程序

void CCmbListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	m_rcClient.SetRect(0, 0, cx, cy) ;
}

void CCmbListCtrl::OnCaptureChanged(CWnd *pWnd)
{
	if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
		EndModalLoop(0);
	CWnd::OnCaptureChanged(pWnd);
}

// 函数功能：绘制子项
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
void CCmbListCtrl::DrawItem(CDC * pDC, LCITEM const & item) 
{
	CRect rcItem(
		m_rcClient.left + 1, m_nItemPosOrigin + item.m_nPosition,
		m_rcClient.right - 1, m_nItemPosOrigin + item.m_nPosition + item.m_nCurHeight) ;
	
	COLORREF crText, crBack ;

//	TRACE(_T("%s: %d\n"), item.m_strText, item.m_uState) ;
	if (item.m_uState == LCIS_PRIHOVERED)
	{
//		TRACE(_T("CurSel : %s\n"), item.m_strText) ;
		crText = m_crTextColorHovered ;
		crBack = m_crBackColorHovered ;
	}
	else
	{
		crText = m_crTextColorNormal ;
		crBack = m_crBackColorNormal ;
	}
	
	// 绘制背景块
	CRect rcBack(rcItem) ;
	rcBack.DeflateRect(1, 1) ;
	CBrush brTemp(crBack) ;
	pDC->FillRect(rcBack, &brTemp) ;
	
	int nOldMode = pDC->SetBkMode(TRANSPARENT) ;
	
	// 绘制文字
	CRect rcText(rcItem) ;
	rcText.DeflateRect(1, 1) ;
	rcText.left += m_nTextOffsetLeft ;
	COLORREF crOldText = pDC->SetTextColor(crText) ;
	pDC->DrawText(item.m_strText, rcText, DT_VCENTER | DT_SINGLELINE) ;
	pDC->SetTextColor(crOldText) ;

	// 绘制按钮
	if (m_pImgDelBtn != NULL && item.m_uState == LCIS_PRIHOVERED)
	{
		CPoint ptBtn ;
		ptBtn.x = rcItem.right - m_nBtnOffsetRight - m_nBtnWidth ;
		ptBtn.y = (rcItem.top + rcItem.bottom - m_nBtnHeight) / 2 ;

		int nXOffset = 0 ;
		switch(item.m_uBtnState)
		{
		case LCIBS_NORMAL :
			break ;

		case LCIBS_HOVERED :
			nXOffset = m_nBtnWidth ;
			break ;

		case LCIBS_PUSHED :
			nXOffset = m_nBtnWidth + m_nBtnWidth ;
			break ;

		default :
			break ;
		}

		Graphics gph(pDC->GetSafeHdc()) ;
		gph.DrawImage(
			m_pImgDelBtn,
			Rect(ptBtn.x, ptBtn.y, m_nBtnWidth, m_nBtnHeight),
			nXOffset, 0, m_nBtnWidth, m_nBtnHeight,
			UnitPixel,
			NULL
			) ;
	}

	pDC->SetBkMode(nOldMode) ;
}

void CCmbListCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// 创建二级缓存
	CDC dcMem ;
	dcMem.CreateCompatibleDC(&dc) ;
	CBitmap bmpMem ;
	bmpMem.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height()) ;
	
	CBitmap * pOldBmp = dcMem.SelectObject(&bmpMem) ;
	CFont * pOldFont = dcMem.SelectObject(&m_ftFontNormal) ;
	
	// 填充白色背景
	dcMem.FillSolidRect(m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom, RGB(255, 255, 255)) ;

	// 绘制边框
	CPen penFrame(PS_SOLID, 1, m_crFrame) ;
	CPen * pOldPen = dcMem.SelectObject(&penFrame) ;

	dcMem.MoveTo(m_rcClient.TopLeft()) ;
	dcMem.LineTo(m_rcClient.right - 1, m_rcClient.top) ;
	dcMem.LineTo(m_rcClient.right - 1, m_rcClient.bottom - 1) ;
	dcMem.LineTo(m_rcClient.left, m_rcClient.bottom - 1) ;
	dcMem.LineTo(m_rcClient.TopLeft()) ;

	dcMem.SelectObject(pOldPen) ;

	// 绘制子项
	VEC_LCITEM::iterator pos ;
	BOOL bBeginDraw = FALSE ;
	for (pos = m_vecItems.begin() ; pos != m_vecItems.end() ; ++ pos)
	{
		// 如果子项不可见，则跳过
		if (IsItemVisible(*pos))
		{
			bBeginDraw = TRUE ;

			// 绘制子项
			DrawItem(&dcMem, *pos) ;
		}
		else if (bBeginDraw)		// 如果已开始绘制后还是子项不可见，则以后不再绘制
			break ;
	}

	// 复制
	dc.BitBlt(m_rcClient.left, m_rcClient.top, m_rcClient.Width(), m_rcClient.Height(),
		&dcMem, 0, 0, SRCCOPY) ;

	// 选回旧字体
	dcMem.SelectObject(pOldFont) ;	
	dcMem.SelectObject(pOldBmp) ;
}

void CCmbListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bBeginHandleLBtnUp)
	{
		if (m_rcClient.PtInRect(point))
			m_bBeginHandleLBtnUp = TRUE ;

		CWnd::OnMouseMove(nFlags, point);
		return ;
	}

	if (!m_bBtnPushed)
	{
		// 获得当前鼠标所在的子项
		int nIndex = HitTest(point) ;

//		if (nIndex == -1 && !(nFlags & MK_LBUTTON))
//			return ;
// 		if (nIndex == -1)
// 		{
// 			if (!(nFlags & MK_LBUTTON))
// 				return ;
// 
// 			// 左键按下，允许跟踪-1子项，即失去跟踪
// 		}
		
		if (nIndex != m_nTrackItemIndex)
		{
			m_bBeginHandleLBtnUp = TRUE ;

			// 左键没按下，则m_nTrackItemIndex不会等于-1
			if (!(nFlags & MK_LBUTTON))
			{
				if (nIndex != -1)
				{
					if (m_nTrackItemIndex != -1)
					{
						m_vecItems[m_nTrackItemIndex].m_uState = LCIS_NORMAL ;
						m_vecItems[m_nTrackItemIndex].m_uBtnState = LCIBS_NORMAL ;
						m_vecItems[m_nTrackItemIndex].m_nHeight = m_nItemHeightNormal ;
					}
					m_nTrackItemIndex = nIndex ;
					if (m_nTrackItemIndex != -1)
					{
						m_vecItems[m_nTrackItemIndex].m_uState = LCIS_PRIHOVERED ;
					}
				}
				else
				{
					if (m_nTrackItemIndex != -1)
					{
						m_vecItems[m_nTrackItemIndex].m_uBtnState = LCIBS_NORMAL ;
					}
				}
			}
			else
			{
				if (m_nTrackItemIndex != -1)
				{
					m_vecItems[m_nTrackItemIndex].m_uState = LCIS_NORMAL ;
					m_vecItems[m_nTrackItemIndex].m_uBtnState = LCIBS_NORMAL ;
					m_vecItems[m_nTrackItemIndex].m_nHeight = m_nItemHeightNormal ;
				}
				m_nTrackItemIndex = nIndex ;
				if (m_nTrackItemIndex != -1)
				{
					m_vecItems[m_nTrackItemIndex].m_uState = LCIS_PRIHOVERED ;
				}
			}

			// 更新界面
			InvalidateRect(NULL, FALSE) ;
		}
		
		// 判断是否在删除按钮上
		if (m_nTrackItemIndex != -1)
		{
			CRect rcBtn ;
			rcBtn.left = m_rcClient.right - 1 - m_nBtnOffsetRight - m_nBtnWidth ;
			rcBtn.top = m_vecItems[m_nTrackItemIndex].m_nPosition + m_nItemPosOrigin
				+ (m_vecItems[m_nTrackItemIndex].m_nCurHeight - m_nBtnHeight) / 2 ;
			rcBtn.right = rcBtn.left + m_nBtnWidth ;
			rcBtn.bottom = rcBtn.top + m_nBtnHeight ;

			if (rcBtn.PtInRect(point))
			{
				if (m_vecItems[m_nTrackItemIndex].m_uBtnState != LCIBS_HOVERED)
				{
					m_vecItems[m_nTrackItemIndex].m_uBtnState = LCIBS_HOVERED ;
					InvalidateRect(rcBtn, FALSE) ;
				}
			}
			else
			{
				if (m_vecItems[m_nTrackItemIndex].m_uBtnState == LCIBS_HOVERED)
				{
					m_vecItems[m_nTrackItemIndex].m_uBtnState = LCIBS_NORMAL ;
					InvalidateRect(rcBtn, FALSE) ;
				}
			}
		}
	}
	else
	{
		LCITEM &item = m_vecItems[m_nBtnPushedItem] ;
		if (m_rcCurrentBtn.PtInRect(point))
		{
			if (item.m_uBtnState != LCIBS_PUSHED)
			{
				item.m_uBtnState = LCIBS_PUSHED ;
				InvalidateRect(m_rcCurrentBtn, FALSE) ;
			}
		}
		else
		{
			if (item.m_uBtnState == LCIBS_PUSHED)
			{
				item.m_uBtnState = LCIBS_NORMAL ;
				InvalidateRect(m_rcCurrentBtn, FALSE) ;
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CCmbListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// 获得当前鼠标悬浮的子项索引
	int nIndex = HitTest(point) ;
	if (nIndex != -1)
	{
		// 判断鼠标是否在删除按钮上
		LCITEM &item = m_vecItems[nIndex] ;
		
		CRect rcBtn ;
		rcBtn.left = m_rcClient.right - 1 - m_nBtnOffsetRight - m_nBtnWidth ;
		rcBtn.top = item.m_nPosition + m_nItemPosOrigin
			+ (item.m_nCurHeight - m_nBtnHeight) / 2 ;
		rcBtn.right = rcBtn.left + m_nBtnWidth ;
		rcBtn.bottom = rcBtn.top + m_nBtnHeight ;
		
		if (rcBtn.PtInRect(point))
		{
			item.m_uBtnState = LCIBS_PUSHED ;
			InvalidateRect(rcBtn, FALSE) ;
			
			// 标记删除按钮已被按下
			m_nBtnPushedItem = nIndex ;
			m_bBtnPushed = TRUE ;
			m_rcCurrentBtn = rcBtn ;
			
			// 获取鼠标控制权
			if (GetCapture() != this)
			{
				SetCapture() ;
			}
		}
	}
	else
	{
		// 发送选择结束消息
		
// 		CPoint ptScreen(point) ;
// 		ClientToScreen(&ptScreen) ;
// 		CWnd * pWnd = GetOwner() ;
		//pWnd->SendMessage(WM_SELOVER, nIndex, MAKELPARAM(ptScreen.x, ptScreen.y)) ;
		
		// 销毁窗口
// 		DestroyWindow() ;

		// 点在外面了
		if (GetCapture() == this)
			ReleaseCapture() ;
		ASSERT(m_pResData != NULL) ;
		if (m_pResData != NULL)
		{
			m_pResData->ot = EXITSEL ;
			m_pResData->nData = -1 ;
		}
	}

/*	CRect rcClient ;
	GetClientRect(rcClient) ;

	if (GetCapture() == this)
		ReleaseCapture() ;

	if (!rcClient.PtInRect(point))
	{
		// 点在外面了
		ASSERT(m_pResData != NULL) ;
		if (m_pResData != NULL)
		{
			m_pResData->ot = EXITSEL ;
			m_pResData->nData = -1 ;
		}
	}
	else
	{

	}*/
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CCmbListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bBeginHandleLBtnUp)
	{
		CWnd::OnLButtonUp(nFlags, point);
		return ;
	}

	if (m_bBtnPushed)
	{
		m_bBtnPushed = FALSE ;

		// 判断鼠标是否在当前删除按钮上
		if (m_rcCurrentBtn.PtInRect(point))
		{
			//ReleaseCapture() ;
			//DestroyWindow() ;

			// 发生删除事件
			//GetOwner()->SendMessage(WM_DELITEM, m_nBtnPushedItem) ;

			if (GetCapture() == this)
				ReleaseCapture() ;

			ASSERT(m_pResData != NULL) ;
			if (m_pResData == NULL)
				return ;
			m_pResData->ot = DELITEM ;
			m_pResData->nData = m_nBtnPushedItem ;
		}
		else
		{

		}
	}
	else
	{
		if (GetCapture() == this)
			ReleaseCapture() ;

		int nIndex = HitTest(point) ;

		ASSERT(m_pResData != NULL) ;
		if (m_pResData == NULL)
			return ;
		m_pResData->ot = SELITEM ;
		m_pResData->nData = nIndex ;

// 		CPoint ptScreen(point) ;
// 		ClientToScreen(&ptScreen) ;
// 		CWnd * pWnd = GetOwner() ;
// 		pWnd->SendMessage(WM_SELOVER, nIndex, MAKELPARAM(ptScreen.x, ptScreen.y)) ;
// 
// 		DestroyWindow() ;
	}

	CWnd::OnLButtonUp(nFlags, point);
}
