//////////////////////////////////////////////////////////////////////////
//
//	描述:   对应WLTreeAL的树节点
//
//	
//	日期:   2011/01/27
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\tree\wltreebaseitem.h"

#define TIN_HEIGHT_CHANGED		(NM_WLFIRST - 100U)
typedef struct _tagNMTIHEIGHTINFO
{
	NMWLHDR hdr ;
	HWLTREEITEM hItem ;
	long lOldHeight ;
	long lNewHeight ;
	BOOL bNoScroll ;
}NMTIHEIGHTINFO, *LPNMTIHEIGHTINFO ;

#define TIN_SELECTED			(NM_WLFIRST - 101U)
typedef struct _tagNMTREEITEM
{
	NMWLHDR hdr ;
	HWLTREEITEM hItem ;
}NMTREEITEM, *LPNMTREEITEM ;

#define TIN_BEGIN_DRAG			(NM_WLFIRST - 102U)
typedef struct _tagNMTIDRAGINFO
{
	NMWLHDR hdr ;
	POINT ptInItem ;
}NMTIDRAGINFO, *LPNMTIDRAGINFO ;

#define WM_CAN_YOU_DRAG		WM_USER + 1986		// 询问节点能否被拖拽
#define	WM_KILLSELECTED		WM_USER + 1989		// 清除选择状态

// tree item type
#define TIT_AIRLAM			_T("TreeAirlamItem")

class WLTreeItemAL :
	public WLTreeBaseItem
{
public:
	WLTreeItemAL(void);
	virtual ~WLTreeItemAL(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// TreeItem Implement

	// 插入孩子
	virtual BOOL InsertChild(HWLTREEITEM hItem, HWLTREEITEM hInsertAfter) ;
	// 删除孩子
	virtual BOOL DeleteChild(HWLTREEITEM hChild) ;
	virtual void DeleteAllChild() ;
	virtual void Type(CString &sType) const { sType = TIT_AIRLAM ; }

	virtual BOOL Expand(UINT uCode) ;
	virtual BOOL Show(BOOL bShow) ;
	virtual BOOL Filter(BOOL bShow) ;
	virtual BOOL Select(BOOL bSelect) ;
	virtual BOOL DragOver(BOOL bEnter) ;

	virtual int CalcBranchHeight() ;					// 计算分支高度(包括节点高度)

	WLTreeItemAL* GetFirstVisibleChild(int nTreeTopInThis) ;

	// 自定义的绘制函数
	// 由于树上很多节点是不显示的，并且都是有规律的队列排列，
	// 所以实际绘制中可以不绘制那些不显示的节点，以提高绘制效率
	virtual void _CustomInternalDraw(HDC hDC, RECT const &rcUpdate, RECT const &rcViewInThis) ;
	virtual void CustomOnDrawChild(HDC hDC, RECT const &rcUpdate, RECT const &rcViewInThis) ;

	// 自定义的测试函数
	// 意义同上
	virtual HWL _CustomInternalHitTest(POINT const &pt, RECT const &rcViewInThis) ;
	virtual HWL CustomHitTestChild(POINT const &pt, RECT const &rcViewInThis) ;

	virtual WLTreeItemAL * HitTestTreeItem(POINT const &pt, RECT const &rcViewInThis) ;

	void GetItemRectInHost(CRect &rcItem) ;
	void UpdateItemView() ;

	// 位移“指定子项”及其后面所有的孩子
	static void OffsetChildItemsY(
		HWLTREEITEM hFirstBackChild,
		HWLTREEITEM hLastBackChild,
		long lOffsetY
		) ;

	int GetItemHeight() const 
	{
		return m_nItemHeight ;
	}

public:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement

	//virtual void GetRectInHost(LPRECT lpRect) ;

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler

	void OnNotifyChildItemHeightChanged(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;

	void OnRButtonDown(UINT nFlags, CPoint point) ;
	void OnRButtonUp(UINT nFlags, CPoint point) ;

	void OnMouseHover() ;
	void OnMouseMove(UINT nFlags, CPoint point) ;
	void OnMouseLeave() ;

	void OnCaptureChanged(HWL hWndLess) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// 内部函数

	// 合上
	BOOL DoCollapse() ;

	// 展开
	BOOL DoExpand() ;

public:
	// 通知父，自己的高度改变
	void NotifyParentHeightChanged(long lOldHeight, long lNewHeight, BOOL bNoScroll) ;

protected:
	int m_nItemHeight ;				// 节点高度，也是子节点的Y坐标起始值
	//BOOL m_bLButtonDown ;			// 鼠标左键是否按下
	//BOOL m_bRButtonDown ;			// 鼠标右键是否按下

	// 拖拽使用到的数据
	BOOL m_bPrepareDrag ;			// 是否已经准备拖拽
	DWORD m_dwPrepareDragTime ;		// 准备开始拖拽的时间
	CPoint m_ptPrepareDragPoint ;	// 准备开始拖拽时鼠标在节点中的位置
	//BOOL m_bEnterDragging ;			// 是否正在拖拽

};
