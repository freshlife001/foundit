#include "StdAfx.h"
#include <algorithm>
#include "WLComboBox.h"
#include "CmbListCtrl.h"
#include "common/MemoryLeakChecker.h"
WLComboBoxImp::WLComboBoxImp(void)
{
	// 初始化各属性
	m_pImgFrame				= NULL ;
	m_pImgDropBtn			= NULL ;
	m_pImgListDelBtn		= NULL ;

	m_bMouseInner			= FALSE ;
	m_bListVisible			= FALSE ;
	m_dwBtnState			= BS_NORMAL ;

	m_nEditLeftMargin		= 5 ;
	m_nEditTopMargin		= 5 ;
	m_nEditBottomMargin		= 5 ;

	m_nEditBtnSpace			= 2 ;

	m_nBtnWidth				= 24 ;
	m_nBtnTopMargin			= 1 ;
	m_nBtnBottomMargin		= 1 ;
	m_nBtnRightMargin		= 1 ;

	// 当前选中项索引
	m_nCurSel				= -1 ;

	m_crFrame				= RGB(0, 0, 0) ;
}

WLComboBoxImp::~WLComboBoxImp(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

void WLComboBoxImp::LoadImage(
						   Image * pImgFrame,
						   Image * pImgDropBtn,
						   Image * pImgListDelBtn
						   ) 
{
	m_pImgFrame = pImgFrame ;
	m_pImgDropBtn = pImgDropBtn ;
	if (m_pImgDropBtn != NULL && m_pImgDropBtn->GetLastStatus() == Ok)
		m_nBtnWidth = m_pImgDropBtn->GetWidth() / 3 ;
	m_pImgListDelBtn = pImgListDelBtn ;
}

// 设置、获取提示文字
void WLComboBoxImp::SetCueBanner(LPCTSTR lpszText) 
{
	m_wlEdit.SetCueBanner(lpszText) ;
}

CString WLComboBoxImp::GetCurBanner() const 
{
	return m_wlEdit.GetCueBanner() ;
}

// 添加字符串
int WLComboBoxImp::AddString(LPCTSTR lpszString, DWORD dwData/* = 0*/) 
{
	CBITEM item ;
	item.m_strText = lpszString ;
	item.m_dwData = dwData ;
	m_vecItems.push_back(item) ;

	return m_vecItems.size() - 1 ;
}

// 插入字符串
int WLComboBoxImp::InsertString(UINT nIndex, LPCTSTR lpszString, DWORD dwData/* = 0*/) 
{
	if (nIndex > m_vecItems.size())
		return CB_ERR ;

	CBITEM item ;
	item.m_strText = lpszString ;
	item.m_dwData = dwData ;

	std::vector<CBITEM>::iterator pos = m_vecItems.insert(m_vecItems.begin() + nIndex, item) ;
	return pos - m_vecItems.begin() ;
}

// 函数功能：删除节点
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/22/2009]
int WLComboBoxImp::DeleteString(UINT nIndex) 
{
	if (nIndex >= m_vecItems.size())
		return CB_ERR ;

	// 如果当前文本和被删除子项文本一样(不区分大小写)，则清空当前文本
	CString strText = m_vecItems[nIndex].m_strText ;
	CString strCurText ;
	m_wlEdit.GetWindowText(strCurText) ;
	if (strText.CompareNoCase(strCurText) == 0)
		m_wlEdit.SetWindowText(_T("")) ;

	m_vecItems.erase(m_vecItems.begin() + nIndex) ;

	return m_vecItems.size() ;
}

void WLComboBoxImp::ResetContent()
{
	m_vecItems.clear();
}

// 函数功能：获取子项文本
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
void WLComboBoxImp::GetText(int nIndex, CString& rString) const 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return ;

	rString = m_vecItems[nIndex].m_strText ;
}

// 函数功能：获得当前选择
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
int WLComboBoxImp::GetCurSel() 
{
	CString strCurSel ;
	m_wlEdit.GetWindowText(strCurSel) ;
	std::vector<CBITEM>::iterator pos = std::find(m_vecItems.begin(), m_vecItems.end(), strCurSel) ;
	if (pos != m_vecItems.end())
		return pos - m_vecItems.begin() ;

	return -1 ;
}

// 函数功能：设置当前选择
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
int WLComboBoxImp::SetCurSel(int nSelect) 
{
	if (nSelect < 0 || nSelect >= (int)m_vecItems.size())
	{
		// 清除
		//m_bSelLeadEditChange = TRUE ;
		m_nCurSel = -1 ;
		m_wlEdit.SetWindowText(_T("")) ;

		return CB_ERR ;
	}
	else
	{
		//m_bSelLeadEditChange = TRUE ;
		m_nCurSel = nSelect ;
		m_wlEdit.SetWindowText(m_vecItems[m_nCurSel].m_strText) ;
		m_wlEdit.SetSel(0, -1) ;
		return nSelect ;
	}
}

// 函数功能：设置子项数据
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
int WLComboBoxImp::SetItemData(int nIndex, DWORD dwItemData)
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return CB_ERR ;

	m_vecItems[nIndex].m_dwData = dwItemData ;

	return nIndex ;
}

// 函数功能：获取子项数据
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/24/2009]
DWORD WLComboBoxImp::GetItemData(int nIndex) const 
{
	if (nIndex < 0 || nIndex >= (int)m_vecItems.size())
		return (DWORD)CB_ERR ;

	return m_vecItems[nIndex].m_dwData ;
}

// 函数功能：寻找字符串
// 输入参数：
// 返 回 值：
// 编写人及日期 ：林珽震[08/23/2009]
int WLComboBoxImp::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const 
{
	if (nIndexStart < -1 || nIndexStart >= (int)m_vecItems.size())
		return CB_ERR ;

	int i ;
	for (i = nIndexStart + 1 ; i < nIndexStart + 1 + (int)m_vecItems.size() ; ++ i)
	{
		int j = i %= m_vecItems.size() ;
		CBITEM const &item = m_vecItems[j] ;
		if (item.m_strText.CompareNoCase(lpszFind) == 0)
			return j ;
	}

	return CB_ERR ;
}

// 函数功能：限制编辑框文字数量
// 输入参数：
// 返 回 值： 
// 编写人及日期 ：林珽震[08/23/2009]
BOOL WLComboBoxImp::LimitText(int nMaxChars) 
{
	//if (::IsWindow(m_cEdit.GetSafeHwnd()))
	{
		m_wlEdit.LimitText(nMaxChars) ;
		return TRUE ;
	}

	return FALSE ;
}

// 设置编辑框选择文本
void WLComboBoxImp::SetEditSel(int nStartChar, int nEndChar, BOOL bNoScroll /*= FALSE*/) 
{
	//if (::IsWindow(m_wlEdit.GetSafeHwnd()))
	{
		m_wlEdit.SetSel(nStartChar, nEndChar/*, bNoScroll*/) ;
	}
}

BOOL WLComboBoxImp::CanAutoCompleteWord() const 
{
	return m_wlEdit.CanAutoCompleteWord() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLComboBoxImp::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	int nX, nY, nCX, nCY ;
	nX = m_nEditLeftMargin ;
	nY = m_nEditTopMargin ;
	nCX = cx - m_nEditLeftMargin - m_nEditBtnSpace - m_nBtnWidth - m_nBtnRightMargin ;
	nCY = cy - m_nEditTopMargin - m_nEditBottomMargin ;
	if (!m_wlEdit.Create(
		WS_VISIBLE,
		nX, nY, nCX, nCY,
		this, IDC_EDIT
		))
		return FALSE ;
	m_wlEdit.SetFrameWidth(0) ;
	m_wlEdit.SetMargin(0, 0, 0, 0) ;
	m_wlEdit.SetBkgndColor(Color(255, 255, 255, 255)) ;

	nX = cx - m_nBtnWidth - m_nBtnRightMargin ;
	nY = m_nBtnTopMargin ;
	nCX = m_nBtnWidth ;
	nCY = cy - m_nBtnTopMargin - m_nBtnBottomMargin ;
// 	if (!m_wlDropBtn.Create(
// 		WS_VISIBLE,
// 		nX, nY, nCX, nCY,
// 		this, IDC_DROPBTN
// 		))
// 		return FALSE ;
	m_rcDropBtn.SetRect(nX, nY, nX + nCX, nY + nCY) ;

	return TRUE ;
}

BOOL WLComboBoxImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_NOTIFY :
		{
			UINT uID = (UINT)wParam ;
// 			switch (uID)
// 			{
// 			case IDC_DROPBTN :
// 				{
// 					LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
// 					switch (pNMWLHDR->code)
// 					{
// 					case NM_LDOWN :
// 						{
// 							OnNotifyDropBtnDown(pNMWLHDR, pResult) ;
// 							bRes = TRUE ;
// 						}
// 						break ;
// 					}
// 				}
// 				break ;
// 			}
		}
		break ;

	case WM_COMMAND :
		{
			bRes = OnCommand(wParam, lParam) ;
		}
		break ;

	case WM_LBUTTONDOWN :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnLButtonDown(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSEMOVE :
		{
			CPoint point(lParam) ;
			HostToClient(&point) ;
			OnMouseMove(wParam, point) ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSELEAVE :
		{
			OnMouseLeave() ;
			bRes = TRUE ;
		}
		break ;

	case WM_KEYDOWN :
		{
			bRes = OnKeyDown(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam)) ;
		}
		break ;

	case WM_MOUSEWHEEL :
		{
			OnMouseWheel(LOWORD(wParam), HIWORD(wParam), CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLComboBoxImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;

/*	HPEN hPen ;
	COLORREF crFrameColor ;
	if (m_bMouseInner)
		crFrameColor = RGB(255, 0, 0) ;
	else
		crFrameColor = RGB(0, 0, 0) ;
	hPen = ::CreatePen(PS_SOLID, 1, crFrameColor) ;
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen) ;

	::MoveToEx(hDC, rcInHost.left, rcInHost.top, NULL) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.top) ;
	::LineTo(hDC, rcInHost.right - 1, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.bottom - 1) ;
	::LineTo(hDC, rcInHost.left, rcInHost.top) ;

	::SelectObject(hDC, hOldPen) ;*/

	{
		Graphics g(hDC) ;

		int nUnitWidth, nHeight, nOffset ;

		// 绘制边框
		if (m_pImgFrame != NULL)
		{
			nUnitWidth = m_pImgFrame->GetWidth() / (2 * 3) ;
			nHeight = m_pImgFrame->GetHeight() ;
			nOffset = (m_bMouseInner || m_bListVisible) ? nUnitWidth * 3 : 0 ;
			g.DrawImage(
				m_pImgFrame,
				Rect(rcInHost.left, rcInHost.top, nUnitWidth, nHeight),
				nOffset, 0, nUnitWidth, nHeight, UnitPixel, NULL, NULL, NULL
				) ;
			g.DrawImage(
				m_pImgFrame,
				Rect(rcInHost.left + nUnitWidth, rcInHost.top, rcInHost.Width() - nUnitWidth - nUnitWidth, nHeight),
				nOffset + nUnitWidth, 0, nUnitWidth, nHeight, UnitPixel, NULL, NULL, NULL
				) ;
			g.DrawImage(
				m_pImgFrame,
				Rect(rcInHost.right - nUnitWidth, rcInHost.top, nUnitWidth, nHeight),
				nOffset + nUnitWidth + nUnitWidth, 0, nUnitWidth, nHeight, UnitPixel, NULL, NULL, NULL
				) ;
		}
		else
		{
			HPEN penFrame = ::CreatePen(PS_SOLID, 1, m_crFrame) ;
			HGDIOBJ penOldFrame = ::SelectObject(hDC, penFrame) ;

			::MoveToEx(hDC, rcInHost.left, rcInHost.top, NULL) ;
			::LineTo(hDC, rcInHost.right - 1, rcInHost.top) ;
			::LineTo(hDC, rcInHost.right - 1, rcInHost.bottom - 1) ;
			::LineTo(hDC, rcInHost.left, rcInHost.bottom - 1) ;
			::LineTo(hDC, rcInHost.left, rcInHost.top) ;

			::DeleteObject(penFrame) ;
		}

		// 绘制下拉按钮
		nUnitWidth = m_pImgDropBtn->GetWidth() / 3 ;
		nHeight = m_pImgDropBtn->GetHeight() ;
		nOffset = 0 ;
		switch (m_dwBtnState)
		{
		case BS_NORMAL :
			nOffset = 0 ;
			break ;
		case BS_HOT :
			nOffset = nUnitWidth ;
			break ;
		case BS_SELECTED :
			nOffset = nUnitWidth + nUnitWidth ;
			break ;
		default :
			ASSERT(FALSE) ;
		}
		g.DrawImage(
			m_pImgDropBtn,
			Rect(rcInHost.left + m_rcDropBtn.left, rcInHost.top + m_rcDropBtn.top, m_rcDropBtn.Width(), m_rcDropBtn.Height()),
			nOffset, 0, nUnitWidth, nHeight, UnitPixel, NULL, NULL, NULL
			) ;
	}

	//DrawDropBtn(hDC) ;
}

void WLComboBoxImp::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	int nX, nY, nCX, nCY ;
// 	nX = m_nEditLeftMargin ;
// 	nY = m_nEditTopMargin ;
	nCX = szNew.cx - m_nEditLeftMargin - m_nEditBtnSpace - m_nBtnWidth - m_nBtnRightMargin ;
	nCY = szNew.cy - m_nEditTopMargin - m_nEditBottomMargin ;
	m_wlEdit.SetWndLessPos(0, 0, nCX, nCY, SWLP_CX | SWLP_CY) ;

	nX = szNew.cx - m_nBtnWidth - m_nBtnRightMargin ;
	nY = m_nBtnTopMargin ;
	nCX = m_nBtnWidth ;
	nCY = szNew.cy - m_nBtnTopMargin - m_nBtnBottomMargin ;
	m_rcDropBtn.SetRect(nX, nY, nX + nCX, nY + nCY) ;

	UpdateDropBtn() ;
}

BOOL WLComboBoxImp::SetWindowText(LPCTSTR lpszText) 
{
	if (__super::SetWindowText(lpszText))
	{
		return m_wlEdit.SetWindowText(lpszText) ;
	}

	return FALSE ;
}

void WLComboBoxImp::GetWindowText(CString &sText) 
{
	m_wlEdit.GetWindowText(sText) ;
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

BOOL WLComboBoxImp::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD whwp = HIWORD(wParam) ;
	WORD wlwp = LOWORD(wParam) ;

	// process Menu or Accelerator command
	if (lParam == 0)
	{

	}
	else	// process control command
	{
		switch (wlwp)
		{
		case IDC_EDIT :
			{
				switch (whwp)
				{
				case EN_CHANGE :
					{
						OnEnChanged() ;
						return TRUE ;
					}
					break ;
				}
			}
			break ;
		}

	}
	return FALSE ;
}

void WLComboBoxImp::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_wlEdit.SetFocus() ;
	if (m_rcDropBtn.PtInRect(point))
	{
		CRect rcInScreen ;
		GetRectInScreen(rcInScreen) ;

		m_dwBtnState |= BS_SELECTED ;
		m_bListVisible = TRUE ;
		UpdateDropBtn() ;

		CCmbListCtrl droplist ;
		droplist.SetItemHeight(rcInScreen.Height()) ;

		CCmbListCtrl::RESDATA res ;

		std::vector<CBITEM>::iterator pos ;
		for (pos = m_vecItems.begin() ; pos != m_vecItems.end() ; ++ pos)
		{
			droplist.AddString(pos->m_strText) ;
		}
		droplist.SetCurSel(GetCurSel()) ;
		droplist.LoadImage(m_pImgListDelBtn) ;
		droplist.Popup(
			rcInScreen.left, rcInScreen.bottom, rcInScreen.Width(), 100,
			GetCenter()->GetHost(),
			res
			) ;

		m_dwBtnState &= ~BS_SELECTED ;
		m_bListVisible = FALSE ;
		UpdateDropBtn() ;

		if (res.ot == CCmbListCtrl::SELITEM && res.nData != -1)
		{
			CBITEM &item = m_vecItems[res.nData] ;
			m_wlEdit.SetWindowText(item.m_strText) ;
			m_wlEdit.SetSel(0, -1) ;

			// 发送选择改变事件
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), CBN_SELCHANGE), LPARAM(this)) ;
		}
		else if (res.ot == CCmbListCtrl::DELITEM)
		{
// 			CBITEM &item = m_vecItems[res.nData] ;
// 			CString sReport ;
// 			sReport.Format(_T("确定要删除游戏“%s”？"), item.m_strText) ;
// 			if (IDYES == MessageBox(GetCenter()->GetHost(), sReport, _T("删除"), MB_YESNO))
// 			{
// 			}
			NMCBITEM item ;
			item.hdr.hWlFrom = this ;
			item.hdr.idFrom = GetID() ;
			item.hdr.code = WLCBN_DELITEM ;
			item.nIndex = res.nData ;

			GetParent()->SendMessage(WM_NOTIFY, (WPARAM)item.hdr.idFrom, (LPARAM)&item) ;
		}
	}
}

void WLComboBoxImp::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bUpdateAll = FALSE ;
	if (!m_bMouseInner)
	{
		bUpdateAll = TRUE ;
		m_bMouseInner = TRUE ;
	}

	BOOL bUpdateDropBtn = FALSE ;
	if (m_dwBtnState & BS_HOT)
	{
		if (!m_rcDropBtn.PtInRect(point))
		{
			bUpdateDropBtn = TRUE ;
			m_dwBtnState &= ~BS_HOT ;
		}
	}
	else
	{
		if (m_rcDropBtn.PtInRect(point))
		{
			bUpdateDropBtn = TRUE ;
			m_dwBtnState |= BS_HOT ;
		}
	}

	if (bUpdateAll)
	{
		UpdateView() ;
	}
	else if (bUpdateDropBtn)
	{
		UpdateDropBtn() ;
	}
}

void WLComboBoxImp::OnMouseLeave()
{
	BOOL bUpdateAll = FALSE ;
	if (m_bMouseInner)
	{
		bUpdateAll = TRUE ;
		m_bMouseInner = FALSE ;
	}

	BOOL bUpdateDropBtn = FALSE ;
	if (m_dwBtnState & BS_HOT)
	{
		bUpdateDropBtn = TRUE ;
		m_dwBtnState &= ~BS_HOT ;
	}

	if (bUpdateAll)
	{
		UpdateView() ;
	}
	else if (bUpdateDropBtn)
	{
		UpdateDropBtn() ;
	}
}

BOOL WLComboBoxImp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bRes = FALSE ;
	switch (nChar)
	{
	case VK_UP :
		{
			SelPrev() ;
			bRes = TRUE ;
		}
		break ;

	case VK_DOWN :
		{
			SelNext() ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

BOOL WLComboBoxImp::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*point*/) 
{
	if (zDelta > 0)
		SelPrev() ;
	else
		SelNext() ;

	return TRUE ;
}

void WLComboBoxImp::OnEnChanged() 
{
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), CBN_EDITCHANGE), (LPARAM)this) ;
}

//////////////////////////////////////////////////////////////////////////
// Inner Function

void WLComboBoxImp::DrawDropBtn(HDC hDC)
{
	CRect rcDropBtn(m_rcDropBtn) ;
	ClientToHost(rcDropBtn) ;

	if (m_dwBtnState & BS_SELECTED)
	{
		::DrawFrameControl(hDC, &rcDropBtn, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED) ;
	}
	else if (m_dwBtnState & BS_HOT)
	{
		COLORREF crOldBk = ::SetBkColor(hDC, RGB(190, 190, 190)) ;
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcDropBtn, NULL, 0, NULL) ;
		::SetBkColor(hDC, crOldBk) ;
		::DrawEdge(hDC, &rcDropBtn, EDGE_RAISED, BF_RECT) ;
	}
	else
	{
		::DrawFrameControl(hDC, &rcDropBtn, DFC_BUTTON, DFCS_BUTTONPUSH) ;
	}
}

void WLComboBoxImp::UpdateDropBtn() 
{
	UpdateView(&m_rcDropBtn) ;
}

void WLComboBoxImp::SelPrev() 
{
	int nCurSel = GetCurSel() ;
	if (nCurSel == -1)
		nCurSel = m_vecItems.size() ;

	if (nCurSel != 0)
	{
		m_nCurSel = nCurSel - 1 ;
		//m_bSelLeadEditChange = TRUE ;
		m_wlEdit.SetWindowText(m_vecItems[m_nCurSel].m_strText) ;
		m_wlEdit.SetSel(0, -1) ;

		// 发送选择改变事件
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), CBN_SELCHANGE), LPARAM(this)) ;
	}
}

void WLComboBoxImp::SelNext() 
{
	int nCurSel = GetCurSel() ;

	if (nCurSel != (int)m_vecItems.size() - 1)
	{		
		m_nCurSel = nCurSel + 1 ;
		//m_bSelLeadEditChange = TRUE ;
		m_wlEdit.SetWindowText(m_vecItems[m_nCurSel].m_strText) ;
		m_wlEdit.SetSel(0, -1) ;

		// 发送选择改变事件
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetID(), CBN_SELCHANGE), LPARAM(this)) ;
	}
}


template<>
UILIB_EXPORT WLComboBoxImpBase *CreateDefaultImp< WLComboBoxImpBase >(IWLComboBox * wrapper)

{
	WLComboBoxImp *ret = new WLComboBoxImp();
	ret->SetWrapper(wrapper);
	return ret;
};
