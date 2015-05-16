//////////////////////////////////////////////////////////////////////////
//
//	描述:   菜单窗口
//
//	
//	日期:   2011/03/22
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "hdmenu.h"
#include <vector>
#include "common/utils/CanvasDC.h"

#include "common/utils/ImageEx/Shared_ImageEx.h"

class CMenuWindowMannger ;

// CMenuWindow
#define ST_MENUWINDOW	_T("St.MenuWindow")

class CMenuWindow : public CWnd
{
	DECLARE_DYNAMIC(CMenuWindow)

public:
	CMenuWindow(CMenuWindowMannger * pMgr, BOOL bRootMenu);
	virtual ~CMenuWindow();

public:
	void SetImage(
		CCanvasDC * pBkgnd,
		CCanvasDC * pItemHotBkgnd
		) ;
	void SetFont(HFONT hFont) { m_hFont = hFont ; }
	void SetFlags(unsigned int uFlags) { m_uFlags = uFlags ; }
	void SetExcludeRect(LPRECT lprcExclude)
	{
		if (lprcExclude != NULL)
			::SetRect(&m_rcExclude, lprcExclude->left, lprcExclude->top, lprcExclude->right, lprcExclude->bottom) ;
	}
	void SetParentMenuItemHeight(int nHeight) { m_nParentMenuItemHeight = nHeight ; }

	BOOL Create(hdui::PMENUCORE pMenuCore, int x, int y, HWND hParent) ;

protected:
// 	enum POPTREND ;
// 	void SetPopTrend(POPTREND enPopTrend) { m_enPopTrend = enPopTrend ; }

protected:
	// 根据菜单内容计算布局
	void CalcLayout(SIZE &sz, hdui::PMENUCORE pMenuCore) ;
	// 根据大小和标记来计算位置
	void CalcPos(
		POINT &pos,
		//POPTREND &enPopTrend,
		unsigned int &uFlags,
		SIZE const &sz,
		RECT const &rcExclude
		) ;

	struct ITEMINFO ;

	// 弹出子菜单
	void PopSubMenuWindow(ITEMINFO * pMenuItem) ;
	// 弹入子菜单
	void PushSubMenuWindow() ;

	// 准备弹出子菜单
	void PreparePopSubMenu(ITEMINFO * pItem) ;
	// 停止弹出子菜单操作
	void StopPopSubMenu() ;

	// 绘制菜单
	void DrawMenu(HDC hDC, RECT const &rcMenu) ;

	// 绘制背景
	void DrawBkgnd(HDC hdcBkgnd, RECT const &rcBkgnd) ;

	// 绘制菜单项
	void DrawItem(HDC hDC, ITEMINFO * pItem) ;

	// 更新视图
	void UpdateView() ;

protected:
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam) ;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	CMenuWindowMannger * m_pMannger ;

	// 数据
	hdui::PMENUCORE m_pMenuCore ;

	// 是否根菜单
	BOOL m_bRootMenu ;

	// 标记
	unsigned int m_uFlags ;			// 弹出位置、左右键响应

	// 弹出趋势
// 	enum POPTREND
// 	{
// 		LEFT, RIGHT
// 	} ;
// 	POPTREND m_enPopTrend ;

	// 父菜单项高度
	int m_nParentMenuItemHeight ;

	// 不可弹到的矩形区域
	RECT m_rcExclude ;

	// 尺寸
	RECT m_rcMargin ;

	int m_nItemHeight ;				// 普通菜单项高度
	int m_nSeparatorItemHeight ;	// 分隔条菜单项高度

	int m_nIconAreaWidth ;			// 图标区域宽度
	int m_nIconSide ;				// 图标边长
	int m_nTextLeftMargin ;			// 文字左边缘
	int m_nTextRightMargin ;		// 文字右边缘
	int m_nPopArrowRightMargin ;	// 弹出箭头右边缘

	int m_nMenuWindowInterval ;		// 菜单窗口间隔

	// 图片
	//Image * m_pImgBkgnd ;
	CCanvasDC * m_pBkgnd ;
	CCanvasDC * m_pItemHotBkgnd ;

	// 绘制
	HFONT m_hFont ;
	CCanvasDC m_cvBkgnd ;

	COLORREF m_crTextNormal ;
	COLORREF m_crTextHot ;
	COLORREF m_crBkgndNormal ;
	COLORREF m_crBkgndHot ;

	struct ITEMINFO
	{
		enum STATUS
		{
			IS_NORMAL	= 0x00,
			IS_HOT		= 0x01,
			IS_POP		= 0x02,
		};

		hdui::PMENUITEMCORE pCore ;
		RECT rcItem ;
		DWORD dwStatus ;
		Shared_ImageEx imgIcon ;
	};

	typedef std::vector<ITEMINFO*> VCT_PITEM ;
	VCT_PITEM m_vctItems ;

	// 鼠标经过的菜单项
	ITEMINFO * m_pHotItem ;

	// 弹出子菜单的菜单项
	ITEMINFO * m_pPopItem ;

	// 子菜单
	BOOL m_bPop ;
	ITEMINFO * m_pPopMenuItem ;
	HWND m_hPopMenuWindow ;

	// 准备弹出子菜单
	UINT_PTR m_uTimerShowSubMenu ;
	ITEMINFO * m_pPreparePopItem ;

};


