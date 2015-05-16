#pragma once
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"
#include "uilib/wndless/IWLDragTabCtrl.h"
#include "mainwnd/BaseWndLessRoot.h"

class CSearchWndContainerRoot : public BaseWndLessRoot
{
	enum
	{
		IDM_CLOSE_ME = 20, //关闭自己
		IDM_CLOSE_OTHERS, //关闭全部
	};
	enum
	{
		IDC_DRAG_TAB = 10,
	};

public:
	CSearchWndContainerRoot(void);
	virtual ~CSearchWndContainerRoot(void);

	HRESULT Add( IWndLess* pWnd, int nIndexAfter/*=-1*/, BOOL bActiveNewItem );
	HRESULT Remove( int nIndex,
		BOOL bReSizeTab = TRUE, // 需要重新计算Tab位置，当连续删除的时候，保持Tab的大小
		BOOL bNeedDestroy = TRUE // 当Tab为空时，是否销毁父窗口
		);

	SIZE GetDefaultSize();

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent );
	virtual void OnSized( SIZE const& szOld, SIZE const& szNew );

private:
	BOOL OnCommand(WPARAM wParam, LPARAM lParam, LRESULT* pResult ) ;
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	BOOL OnKeyDown(	UINT nChar,	UINT nRepCnt,	UINT nFlags );
	void OnTabRClick( NMWLHDR * pNMHDR, LRESULT * pResult );

	//system msg handler
	afx_msg void OnTimer(UINT_PTR nIDEvent );
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnSkinChanged(WPARAM, LPARAM) ;

	//tab msg handler
	afx_msg LRESULT OnTabTitleChange( WPARAM, LPARAM );
	afx_msg void OnTabSelChange(NMWLHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnRemoveDragItem( NMWLHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnDropTabItem( NMWLHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnTabDragEnter( NMWLHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnTabAddNewItem( NMWLHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnTabCloseItem( NMWLHDR * pNMHDR, LRESULT * pResult );

	void UpdateTabWndRect( int curSel );
	void UpdateTitle();
	IWndLess* GetCurWnd();
	IWndLess* CreateSearchWnd();
private:
	WLDragTabCtrl m_tabChatDlgs;
	Shared_ImageEx m_imgTabItemBk;
	Shared_ImageEx m_imgTabBg;


	
};
