//////////////////////////////////////////////////////////////////////////
//
//	描述：	自定义CTabCtrl
//			目前不支持多行
//	
//	日期：	06/23/2009
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WL_TAB_CTRL__INCLUDED_)
#define AFX_WL_TAB_CTRL__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabCtrlST.h : header file
//

#include <vector>
#include "common/utils/ImageEx/Shared_ImageEx.h"
#include "common/utils/CanvasDC.h"
#include "IWLTabCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// WLTabCtrl window
class WLTabItem;

template <class IT = IWLTabCtrl>
class WLTabCtrlImp : public CWndLessImpBase< CWndLessShell<IT> > 
{

// Construction
public:
	WLTabCtrlImp( BOOL bCanDrag );
	virtual ~WLTabCtrlImp();
// Attributes
public:
	// 设置背景图片
	virtual void SetImage(Image * pImgBkgnd, BOOL bRedraw) ;

	// 设置子项间隔
	virtual void SetSpace(int nNearSpace, int nItemSpace, int nFarSpace) ;

	// 设置文字颜色
	virtual void SetTextColor(COLORREF crText, BOOL bRedraw) ;

	// 设置选中文字颜色
	virtual void SetSelectTextColor( COLORREF crText, BOOL bRedraw  );

	// 设置背景颜色
	virtual void SetBkgndColor(COLORREF crBkgnd, BOOL bRedraw) ;

	// 设置是否可以获取焦点
	virtual void SetFocusable(BOOL bFocusable) ;

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw( HDC hDC, RECT const &rcUpdate );
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew);
	virtual void OnDestroy();

protected:

	// 是否获得焦点
	BOOL m_bFocusable ;

	// 记录当前选中和鼠标经过的子项索引
	//int m_nHotItem ;
	//int m_nSelectedItem ;
	//int m_nLBDownItem ;
	WLTabItem* m_pSelItem;

	// 间隔
	int m_nNearSpace ;	// 第一个子项与起点的间隔
	int	m_nItemSpace ;	// 子项之间的间隔
	int m_nFarSpace ;	// 最后一个子项与终点的间隔

	// 记录背景图片或背景dc
	Image * m_pImgBkgnd ;

	// 记录子项数组
	std::vector<WLTabItem*> m_vecItems;
	
	// 文字颜色
	COLORREF	m_crText ;
	COLORREF m_crTextSelected;

	// 背景颜色
	COLORREF	m_crBkgnd ;

	// 提示控件
	//CToolTipCtrl	m_tooltip ;
	//UINT			m_uLastTipID ;

	//BOOL m_bIsTrackingMouse;

	BOOL m_bCanDrag;

	int m_nMaxItemWidth; // item最大宽度

	BOOL m_bDelayCalItemSize;

// Operations
protected:
	// 重新计算布局
	virtual void RecalcLayout() ;

	// 跟踪子项
	//BOOL Track(int nIndex) ;

	// 绘制子项
	// BOOL DrawItem( HDC hDC, int nIndex) ;

	virtual BOOL GetDrawItemRect( int nIndex, CRect& rcItem );

	virtual WLTabItem* CreateTabItem( LPCTSTR lpszText, 
		UINT uTextStyle,
		CPoint	ptText,
		LPCTSTR sTipText,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		BOOL bDrag,
		int nTabStateCount );

	virtual void RepositionAfterDeleteItem( BOOL bResize, int nIndex, CSize &delItemSize );

public:
	// 插入子项
	virtual int InsertItem(
		int nIndex,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		int nTabStateCount,
		LPCTSTR lpszText,
		UINT uTextStyle,
		int nTextX,
		int nTextY,
		LPCTSTR lpszTip,
		int maxWidth
		) ;

	virtual void ResetIndexAndZorder();

	// 设置子项文本
	virtual BOOL SetItemText(int nIndex, LPCTSTR lpszText) ;

	// 设置子项数据
	virtual BOOL SetItemData(int nIndex, DWORD_PTR dwData) ;

	// 获取子项数据
	virtual DWORD_PTR GetItemData(int nIndex) const ;

	// 删除子项
	virtual BOOL DeleteItem(int nIndex, BOOL bResize) ;

	// 选择子项
	virtual BOOL SelectItem(int nIndex) ;

	virtual BOOL HightLightItem( int nIndex );

	// 获得子项区域
	virtual BOOL GetItemRect(int nIndex, CRect &rcItem) ;

	virtual BOOL SetItemIconSize( int nIndex, CSize& size ); 

	virtual BOOL SetItemIcon( int nIndex, Shared_ImageEx icon );

	// 测试鼠标位置
	// int HitTest(CPoint point) ;
	
	// 获得当前选择的子项索引
	virtual int GetCurSel() const ;
	
	// 获得子项数量
	virtual int GetItemCount() const ;

	virtual void OnItemRBtnClicked( LPNMWLHDR lpNMWLHDR );

protected:

	//virtual HWL HitTestChild(POINT const &pt) ;
	//virtual void OnDrawChild(HDC hDC, RECT const &rcUpdate) ;

	virtual void OnLDownItem( LPNMWLHDR lpNMWLHDR );

	virtual void OnDragItemMove( LPNMWLHDR lpNMWLHDR  );

	
public:
	void OnRButtonUp(UINT nFlags, CPoint point);
};

#include "WLTabCtrl.hpp"

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLST_H__504C243B_F516_40F2_89EB_B1E0EC8C7C23__INCLUDED_)
