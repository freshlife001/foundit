#pragma once
#include "uilib/wndless/IWLBubbleEdit.h"
#include "uilib/wndless/IWLButton.h"
typedef struct _tagNMSEARCHINFO 
{
	NMWLHDR hdr ;
	CString sKeyWord ;
} NMSEARCHINFO, * LPNMSEARCHINFO ;

// command
#define SBN_UPDATESEARCH	(NM_WLFIRST - 300U)
#define SBN_ENDSEARCH		(NM_WLFIRST - 301U)
#define SBN_UP				(NM_WLFIRST - 302U)
#define SBN_DOWN			(NM_WLFIRST - 303U)
#define SBN_RETURN			(NM_WLFIRST - 304U)

class CSearchInput :
	public WLBubbleEdit
{
public:
	CSearchInput(void);
	virtual ~CSearchInput(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	void SetMagnifierImage(UINT uMagnifierId) ;
	void SetOwner(HWL hOwner) ;
	void EndSearch() ;				// 退出搜索
	virtual void BeginEdit() ;
	virtual void EndEdit() ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler

	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	void OnBtnExitCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBnClickClearSearchKeyword() ;

	//////////////////////////////////////////////////////////////////////////
	// Inner Functions
	void ClearSearch() ;		// 清理搜索现场

protected:
	//////////////////////////////////////////////////////////////////////////
	// Data
	Shared_ImageEx m_imgMagnifier ;
	//Shared_ImageEx m_imgBkgnd ;

	// 状态
	bool m_hasContent;

	// 控件
	enum
	{
		CTRLID_BTN_CLEAR_SEARCH_KEYWORD	= 48,
	};
	WLButton m_wlBtnClearSearchKeyword ;

	// 所有者
	HWL m_hOwner ;

	// 得到焦点之前的输入法
	HKL m_hklBeforeSetFocus ;
};
