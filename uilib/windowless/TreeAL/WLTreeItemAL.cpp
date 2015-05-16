#include "StdAfx.h"
#include "WLTreeItemAL.h"
#include "common/utils/UIUtils.h"

//#define _TRACEDRAW

WLTreeItemAL::WLTreeItemAL(void)
: m_nItemHeight(0)
// , m_bLButtonDown(FALSE)
// , m_bRButtonDown(FALSE)

, m_bPrepareDrag(FALSE)			// 是否已经准备拖拽
, m_dwPrepareDragTime(0)		// 准备开始拖拽的时间
, m_ptPrepareDragPoint(0, 0)	// 准备开始拖拽时鼠标在节点中的位置
//, m_bEnterDragging(FALSE)		// 是否正在拖拽
{
}

WLTreeItemAL::~WLTreeItemAL(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

// 插入孩子
BOOL WLTreeItemAL::InsertChild(HWLTREEITEM hItem, HWLTREEITEM hInsertAfter) 
{
	if (!__super::InsertChild(hItem, hInsertAfter))
		return FALSE ;

	int nY = m_nItemHeight ;
	HWLTREEITEM hPrevSibling = hItem->GetPrevSibling() ;
	while (hPrevSibling != NULL)
	{
		if (hPrevSibling->IsVisible())
		{
			CRect rc ;
			hPrevSibling->GetRectInParent(rc) ;
			nY = rc.bottom ;
			break ;
		}
		hPrevSibling = hPrevSibling->GetPrevSibling() ;
	}

	hItem->SetWndLessPos(0, nY, m_szClient.cx, 0, SWLP_Y | SWLP_CX | SWLP_NOREDRAW) ;

	// 如果节点不显示，不影响其他兄弟节点和父节点
	if (!hItem->IsVisible())
		return TRUE ;

	// 后面的孩子往下移动
	CRect rcItem ;
	hItem->GetRectInParent(rcItem) ;
	OffsetChildItemsY(hItem->GetNextSibling(), NULL, rcItem.Height()) ;

	// 如果是展开的，则调整高度
	if (IsExpand())
	{
		// 记录旧高度
		long lOldHeight = m_szClient.cy ;

		// 自己变高
		SetWndLessPos(0, 0, 0, rcItem.Height(), SWLP_CY | SWLP_CY_OFFSET) ;

		// 通知父窗体变高
		long lNewHeight = m_szClient.cy ;
		NotifyParentHeightChanged(lOldHeight, lNewHeight, FALSE) ;
	}

	return TRUE ;
}

// 删除孩子
BOOL WLTreeItemAL::DeleteChild(HWLTREEITEM hChild) 
{
	if (hChild == NULL)
		return FALSE ;

	CRect rcChild ;
	hChild->GetRectInParent(rcChild) ;
	HWLTREEITEM hNextChild = hChild->GetNextSibling() ;

	// 记录旧高度
	long lOldHeight = m_szClient.cy ;

	BOOL bIsChildShow = hChild->IsVisible() ;

	if (!__super::DeleteChild(hChild))
		return FALSE ;

	if (bIsChildShow)
	{
		// 后面的孩子往上移动
		OffsetChildItemsY(hNextChild, NULL, -rcChild.Height()) ;

		// 如果是展开的，则调整高度
		if (IsExpand())
		{
			// 自己变矮
			SetWndLessPos(0, 0, 0, -rcChild.Height(), SWLP_CY | SWLP_CY_OFFSET) ;

			// 通知父窗体变矮
			long lNewHeight = m_szClient.cy ;
			NotifyParentHeightChanged(lOldHeight, lNewHeight, FALSE) ;
		}
	}

	return TRUE ;
}

void WLTreeItemAL::DeleteAllChild() 
{
	__super::DeleteAllChild() ;

	if (IsExpand())
	{
		// 记录旧高度
		long lOldHeight = m_szClient.cy ;

		// 自己变矮
		SetWndLessPos(0, 0, 0, m_nItemHeight, SWLP_CY) ;

		// 通知父窗体变矮
		long lNewHeight = m_szClient.cy ;
		NotifyParentHeightChanged(lOldHeight, lNewHeight, FALSE) ;
	}
}

// 返回值TRUE表示操作执行，否则表示已展开或已合上
BOOL WLTreeItemAL::Expand(UINT uCode) 
{
	switch (uCode)
	{
	case TE_COLLAPSE :
		{
			return DoCollapse() ;
			//return TRUE ;
		}
		break ;

	case TE_EXPAND :
		{
			return DoExpand() ;
			//return TRUE ;
		}
		break ;

	case TE_TOGGLE :
		{
			if (m_dwStatus & TIS_EXPAND)
			{
				return DoCollapse() ;
			}
			else
			{
				return DoExpand() ;
			}

			//return TRUE ;
		}
		break ;
	}

	return FALSE ;
}

BOOL WLTreeItemAL::Show(BOOL bShow) 
{
	if (bShow)
	{
		if (m_dwStatus & TIS_HIDE)
		{
			m_dwStatus &= ~TIS_HIDE ;

			if (IsVisible())
			{
				// 通知父窗体高度变化
				//long lNewHeight = m_szClient.cy ;
				long lNewHeight = CalcBranchHeight() ;
				NotifyParentHeightChanged(0, lNewHeight, FALSE) ;

				int y = ((WLTreeItemAL*)GetParentItem())->GetItemHeight() ;
				HWLTREEITEM hPrevSibling = GetPrevSibling() ;
				while (hPrevSibling != NULL)
				{
					if (hPrevSibling->IsVisible())
					{
						RECT rcItem ;
						hPrevSibling->GetRectInParent(&rcItem) ;
						y = rcItem.bottom ;
						break ;
					}

					hPrevSibling = hPrevSibling->GetPrevSibling() ;
				}
				SetWndLessPos(0, y, 0, lNewHeight, SWLP_Y | SWLP_CY | SWLP_SHOW) ;
			}
			return TRUE ;
		}
	}
	else
	{
		if (!(m_dwStatus & TIS_HIDE))
		{
			m_dwStatus |= TIS_HIDE ;

			if (!IsVisible())
			{
				// 通知父窗体高度变化
				long lOldHeight = m_szClient.cy ;
				NotifyParentHeightChanged(lOldHeight, 0, FALSE) ;
				SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ; 
			}
			return TRUE ;
		}
	}

	return FALSE ;
}

BOOL WLTreeItemAL::Filter(BOOL bShow) 
{
	if (bShow)
	{
		if (m_dwStatus & TIS_FILTERED)
		{
			m_dwStatus &= ~TIS_FILTERED ;
			return TRUE ;
		}
	}
	else
	{
		if (!(m_dwStatus & TIS_FILTERED))
		{
			m_dwStatus |= TIS_FILTERED ;
			return TRUE ;
		}
	}

	return FALSE ;
}

BOOL WLTreeItemAL::Select(BOOL bSelect) 
{
	if (bSelect)
	{
		if (!(m_dwStatus & TIS_SELECTED))
		{
			m_dwStatus |= TIS_SELECTED ;
			UpdateItemView() ;

			// 告诉父窗口自己被选择了
			NMTREEITEM ti ;
			ti.hdr.hWlFrom = this ;
			ti.hdr.idFrom = m_uID ;
			ti.hdr.code = TIN_SELECTED ;
			ti.hItem = this ;
			GetTree()->SendMessage(WM_NOTIFY, (WPARAM)m_uID, (LPARAM)&ti) ;

			return TRUE ;
		}
	}
	else
	{
		if (m_dwStatus & TIS_SELECTED)
		{
			m_dwStatus &= ~TIS_SELECTED ;
			UpdateItemView() ;

			return TRUE ;
		}
	}

	return FALSE ;
}

BOOL WLTreeItemAL::DragOver(BOOL bEnter) 
{
	if (bEnter)
	{
		if (!(m_dwStatus & TIS_DRAGOVER))
		{
			m_dwStatus |= TIS_DRAGOVER ;
			UpdateItemView() ;

			return TRUE ;
		}
	}
	else
	{
		if (m_dwStatus & TIS_DRAGOVER)
		{
			m_dwStatus &= ~TIS_DRAGOVER ;
			UpdateItemView() ;

			return TRUE ;
		}
	}

	return FALSE ;
}

// 计算分支高度(包括节点高度)
int WLTreeItemAL::CalcBranchHeight() 
{
	if (!IsVisible())
		return 0 ;

	int nRes = m_nItemHeight ;

	if (IsExpand())
	{
		WLTreeItemAL* hChild = (WLTreeItemAL*)m_hFirstChild ;
		while (hChild != NULL)
		{
			nRes += hChild->CalcBranchHeight() ;

			hChild = (WLTreeItemAL*)(hChild->m_hNextSibling) ;
		}
	}

	return nRes ;
}

WLTreeItemAL* WLTreeItemAL::GetFirstVisibleChild(int nTreeTopInThis) 
{
	HWLTREEITEM hChild = GetFirstChild() ;
	while (hChild != NULL)
	{
		if (hChild->IsVisible())
		{
			CRect rcChild ;
			hChild->GetRectInParent(rcChild) ;
			if (rcChild.bottom > nTreeTopInThis)
				return (WLTreeItemAL*)hChild ;
		}

		hChild = hChild->GetNextSibling() ;
	}

	return NULL ;
}

void WLTreeItemAL::_CustomInternalDraw(HDC hDC, RECT const &rcUpdate, RECT const &rcViewInThis) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;
	RECT rcInsterset ;
	if (!IsWndLessVisible() || !::IntersectRect(&rcInsterset, &rcInHost, &rcUpdate))
		return ;

#ifdef _TRACEDRAW
	TRACE(_T("--> %S::_InternalOnDraw [%p]\n"), typeid(*this).name(), this) ;
	CString s ;
	s.Format(_T("<-- %S::_InternalOnDraw [%p]"), typeid(*this).name(), this) ;
	hdutils::CPUPerformance cp(s) ;
#endif

	CRect rc ;
	GetRectInParent(rc) ;

	if (m_nItemHeight > rcViewInThis.top)
	{
		OnDraw(hDC, rcUpdate) ;
	}

	CustomOnDrawChild(hDC, rcUpdate, rcViewInThis) ;
}

void WLTreeItemAL::CustomOnDrawChild(HDC hDC, RECT const &rcUpdate, RECT const &rcViewInThis) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;

	if (IsExpand())
	{
		WLTreeItemAL* hChild = GetFirstVisibleChild(rcViewInThis.top) ;

		for ( ; hChild != NULL ; hChild = (WLTreeItemAL*)hChild->GetNextSibling())
		{
			if (!hChild->IsVisible() || !hChild->IsWndLessVisible())
				continue ;

			CRect rcChild ;
			hChild->GetRectInParent(rcChild) ;

			if (rcChild.bottom + rcInHost.top > rcUpdate.top)
			{
				if (rcChild.top >= rcViewInThis.bottom || rcChild.top + rcInHost.top > rcUpdate.bottom)
					break ;

				CRect rcViewInChild(rcViewInThis) ;
				rcViewInChild.OffsetRect(0, -rcChild.top) ;

				hChild->_CustomInternalDraw(hDC, rcUpdate, rcViewInChild) ;
			}
		}
	}
}

HWL WLTreeItemAL::_CustomInternalHitTest(POINT const &pt, RECT const &rcViewInThis) 
{
	//DWORD dwBegin = GetTickCount() ;
	//////////////////////////////////////////////////////////////////////////
	// 1. Origin HitTest
/*	
	HWL hRes = _InternalHitTest(pt) ;
	TRACE(_T("_InternalHitTest cost %u\n"), GetTickCount() - dwBegin) ;
	return hRes ;*/

	if (!PtInWndLess(pt))
		return NULL ;

	HWL hRes = CustomHitTestChild(pt, rcViewInThis) ;
	if (hRes == NULL)
		hRes = this ;

	return hRes ;
}

HWL WLTreeItemAL::CustomHitTestChild(POINT const &pt, RECT const &rcViewInThis) 
{
	HWL hRes = NULL ;

	WLTreeItemAL* hChild = GetFirstVisibleChild(rcViewInThis.top) ;
	for ( ; hChild != NULL ; hChild = (WLTreeItemAL*)hChild->GetNextSibling())
	{
		if (!hChild->IsVisible() || !hChild->IsWndLessVisible())
			continue ;

		CRect rcChild ;
		hChild->GetRectInParent(rcChild) ;

		if (rcChild.top >= rcViewInThis.bottom)
			break ;

		CRect rcViewInChild(rcViewInThis) ;
		rcViewInChild.OffsetRect(0, -rcChild.top) ;

		hRes = hChild->_CustomInternalHitTest(pt, rcViewInChild) ;
		if (hRes != NULL)
			return hRes ;
	}

	//TRACE(_T("_CustomInternalHitTest cost %u\n"), GetTickCount() - dwBegin) ;
	return hRes ;
}

WLTreeItemAL * WLTreeItemAL::HitTestTreeItem(POINT const &pt, RECT const &rcViewInThis) 
{
	if (!PtInWndLess(pt))
		return NULL ;

	WLTreeItemAL * pRes = NULL ;

	WLTreeItemAL* hChild = GetFirstVisibleChild(rcViewInThis.top) ;
	for ( ; hChild != NULL ; hChild = (WLTreeItemAL*)hChild->GetNextSibling())
	{
		if (!hChild->IsVisible() || !hChild->IsWndLessVisible())
			continue ;

		CRect rcChild ;
		hChild->GetRectInParent(rcChild) ;

		if (rcChild.top >= rcViewInThis.bottom)
			break ;

		CRect rcViewInChild(rcViewInThis) ;
		rcViewInChild.OffsetRect(0, -rcChild.top) ;

		pRes = hChild->HitTestTreeItem(pt, rcViewInChild) ;
		if (pRes != NULL)
			return pRes ;
	}

	if (pRes == NULL)
		pRes = this ;

	return pRes ;
}

void WLTreeItemAL::GetItemRectInHost(CRect &rcItem) 
{
	GetRectInHost(rcItem) ;
	rcItem.bottom = rcItem.top + m_nItemHeight ;
}

void WLTreeItemAL::UpdateItemView() 
{
	CRect rcItem ;
	GetItemRectInHost(rcItem) ;
	::InvalidateRect(GetCenter()->GetHost(), rcItem, FALSE) ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement
/*
void WLTreeItemAL::GetRectInHost(LPRECT lpRect) 
{
	ASSERT(lpRect != NULL) ;
	if (lpRect == NULL)
		return ;

	m_pParent->GetRectInHost(lpRect) ;

	// 树节点的左右边界和父一样

	//lpRect->left += m_ptTopLeftInParent.x ;
	lpRect->top += m_ptTopLeftInParent.y ;
	//lpRect->right = lpRect->left + m_szClient.cx ;
	lpRect->bottom = lpRect->top + m_szClient.cy ;
}
*/
BOOL WLTreeItemAL::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_NOTIFY :
		{
			LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
			switch (pNMWLHDR->code)
			{
			case TIN_HEIGHT_CHANGED :
				OnNotifyChildItemHeightChanged(pNMWLHDR, pResult) ;
				bRes = TRUE ;
				break ;
			}
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			OnLButtonDown(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			OnLButtonUp(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_RBUTTONDOWN :
		{
			OnRButtonDown(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_RBUTTONUP :
		{
			OnRButtonUp(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSEHOVER :
		{
			OnMouseHover() ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSEMOVE :
		{
			OnMouseMove(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_MOUSELEAVE :
		{
			OnMouseLeave() ;
			bRes = TRUE ;
		}

	case WM_CAPTURECHANGED :
		{
			OnCaptureChanged(HWL(lParam)) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLTreeItemAL::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	if (szOld.cx != szNew.cx)
	{
		HWLTREEITEM hChild = GetFirstChild() ;
		while (hChild != NULL)
		{
			hChild->SetWndLessPos(0, 0, szNew.cx, 0, SWLP_CX) ;
			hChild = hChild->GetNextSibling() ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

void WLTreeItemAL::OnNotifyChildItemHeightChanged(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	LPNMTIHEIGHTINFO pHeightChangedInfo = (LPNMTIHEIGHTINFO)pNMWLHDR ;
	*pResult = 1 ;

	if (pHeightChangedInfo->lOldHeight == pHeightChangedInfo->lNewHeight)
		return ;

	// 目前的树节点只关心高度变化
	// 记录旧高度
	long lOldHeight = m_szClient.cy ;

	// 获得高度变化
	long lOffset = pHeightChangedInfo->lNewHeight - pHeightChangedInfo->lOldHeight ;

	// 移动后面的孩子
	// 这个函数比较低效
	OffsetChildItemsY(pHeightChangedInfo->hItem->GetNextSibling(), NULL, lOffset) ;

	if (IsExpand())
	{
		SetWndLessPos(
			0, 0, 0, lOffset,
			SWLP_CY | SWLP_CY_OFFSET
			) ;

		// 如果是显示的，则通知父窗体变高
		if (IsVisible())
		{
			long lNewHeight = m_szClient.cy ;
			NotifyParentHeightChanged(lOldHeight, lNewHeight, pHeightChangedInfo->bNoScroll) ;
		}
	}
}

void WLTreeItemAL::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	SetFocus() ;
	m_bLButtonDown = TRUE ;
	HostToClient(&point) ;

	if (SendMessage(WM_CAN_YOU_DRAG, 0, 0))
	{
		if (GetCapture() != this)
			SetCapture() ;

		m_bPrepareDrag = TRUE ;
		m_dwPrepareDragTime = GetTickCount() ;
		m_ptPrepareDragPoint = point ;
	}
}

void WLTreeItemAL::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	if (m_bLButtonDown)
	{
		m_bLButtonDown = FALSE ;

		//DWORD dwBegin = GetTickCount() ;
		SendMessage(WM_LBUTTONCLICK, 0, 0) ;	// 由节点自己决定左键单击时的操作：选择、展开、闭合

		//CString sTitle ;
		//sTitle.Format(_T("Expand cost %d ms\n"), GetTickCount() - dwBegin) ;
		//::SendMessage(GetCenter()->GetHost(), WM_SETTEXT, 0, (LPARAM)sTitle.GetString()) ;

		//::SetWindowText(GetCenter()->GetHost(), sTitle) ;
		//TRACE(_T("Expand cost %d ms\n"), GetTickCount() - dwBegin) ;
	}

	if (m_bPrepareDrag)
	{
		m_bPrepareDrag = FALSE ;
		ReleaseCapture() ;
	}
}

void WLTreeItemAL::OnRButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	SetFocus() ;
	m_bRButtonDown = TRUE ;
}

void WLTreeItemAL::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	if (m_bRButtonDown)
	{
		m_bRButtonDown = FALSE ;
		SendMessage(WM_RBUTTONCLICK, 0, 0) ;
	}
}

void WLTreeItemAL::OnMouseHover() 
{
	if (!(m_dwStatus & TIS_HOT))
	{
		m_dwStatus |= TIS_HOT ;
		UpdateItemView() ;
	}
}

void WLTreeItemAL::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	if (!(m_dwStatus & TIS_HOT))
	{
		m_dwStatus |= TIS_HOT ;
		UpdateItemView() ;
	}

	HostToClient(&point) ;

	if (m_bPrepareDrag
		&& GetTickCount() - m_dwPrepareDragTime >= 200
		&& (abs(m_ptPrepareDragPoint.x - point.x) + abs(m_ptPrepareDragPoint.y - point.y)) >= 20)
	{
		m_bPrepareDrag = FALSE ;
		ReleaseCapture() ;

		NMTIDRAGINFO info ;
		info.hdr.hWlFrom = this ;
		info.hdr.idFrom = m_uID ;
		info.hdr.code = TIN_BEGIN_DRAG ;
		info.ptInItem.x = m_ptPrepareDragPoint.x ;
		info.ptInItem.y = m_ptPrepareDragPoint.y ;
		GetTree()->SendMessage(WM_NOTIFY, (WPARAM)m_uID, (LPARAM)&info) ;
	}
}

void WLTreeItemAL::OnMouseLeave() 
{
	if (m_dwStatus & TIS_HOT)
	{
		m_dwStatus &= ~TIS_HOT ;
		UpdateView() ;
	}

	m_bLButtonDown = FALSE ;
	m_bRButtonDown = FALSE ;
	m_bPrepareDrag = FALSE ;
}

void WLTreeItemAL::OnCaptureChanged(HWL /*hWndLess*/) 
{
	m_bLButtonDown = FALSE ;
	m_bRButtonDown = FALSE ;
	m_bPrepareDrag = FALSE ;
}

//////////////////////////////////////////////////////////////////////////
// 内部函数

// 合上
BOOL WLTreeItemAL::DoCollapse() 
{
	if (!(m_dwStatus & TIS_EXPAND))
		return FALSE ;

	m_dwStatus &= ~TIS_EXPAND ;
	UpdateItemView() ;

	//////////////////////////////////////////////////////////////////////////
	// 隐藏孩子
	HWLTREEITEM hChild = m_hFirstChild ;
	while (hChild != NULL)
	{
		if (hChild->IsVisible())
			hChild->SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
		hChild = hChild->GetNextSibling() ;
	}

	//////////////////////////////////////////////////////////////////////////
	// 减小高度
	// 记录旧高度
	long lOldHeight = m_szClient.cy ;
	SetWndLessPos(0, 0, 0, m_nItemHeight, SWLP_CY) ;

	if (IsVisible())
	{
		// 通知父窗体高度变化
		long lNewHeight = m_szClient.cy ;
		NotifyParentHeightChanged(lOldHeight, lNewHeight, FALSE) ;
	}

	return TRUE ;
}

// 展开
BOOL WLTreeItemAL::DoExpand() 
{
	if (m_dwStatus & TIS_EXPAND)
		return FALSE ;

	m_dwStatus |= TIS_EXPAND ;
	UpdateItemView() ;

	//////////////////////////////////////////////////////////////////////////
	// 显示孩子
	HWLTREEITEM hChild = m_hFirstChild ;
	while (hChild != NULL)
	{
		if (hChild->IsVisible())
			hChild->SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;

		hChild = hChild->GetNextSibling() ;
	}

	//////////////////////////////////////////////////////////////////////////
	// 减小高度
	// 记录旧高度
	long lOldHeight = m_szClient.cy ;
	long lNewHeight = CalcBranchHeight() ;
	SetWndLessPos(0, 0, 0, lNewHeight, SWLP_CY) ;

	if (IsVisible())
	{
		// 通知父窗体高度变化
		NotifyParentHeightChanged(lOldHeight, lNewHeight, FALSE) ;
	}

	return TRUE ;
}

// 通知父，自己的高度改变
void WLTreeItemAL::NotifyParentHeightChanged(long lOldHeight, long lNewHeight, BOOL bNoScroll) 
{
	if (m_pParent == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (lOldHeight == lNewHeight)
		return ;

	NMTIHEIGHTINFO info ;
	info.hdr.hWlFrom = this ;
	info.hdr.idFrom = m_uID ;
	info.hdr.code = TIN_HEIGHT_CHANGED ;
	info.hItem = this ;
	info.lOldHeight = lOldHeight ;
	info.lNewHeight = lNewHeight ;
	info.bNoScroll = bNoScroll ;
	m_pParent->SendMessage(WM_NOTIFY, m_uID, (LPARAM)&info) ;
}

// 位移指定子项及其后面所有的孩子
void WLTreeItemAL::OffsetChildItemsY(
									 HWLTREEITEM hFirstBackChild,
									 HWLTREEITEM hLastBackChild,
									 long lOffsetY
									 ) 
{
	if (hFirstBackChild == NULL || lOffsetY == 0)
		return ;

	HWLTREEITEM hTemp = hFirstBackChild ;
	while (hTemp != hLastBackChild)
	{
		hTemp->SetWndLessPos(0, lOffsetY, 0, 0, SWLP_Y | SWLP_Y_OFFSET) ;
		hTemp = hTemp->GetNextSibling() ;
	}

	// 更新视图
}
