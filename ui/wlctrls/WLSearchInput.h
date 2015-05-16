//////////////////////////////////////////////////////////////////////////
//
//	描述:   搜索栏
//
//	
//	日期:   2011/02/22
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "WLBubbleEdit.h"
#include "uilib/wndless/IWLButton.h"

typedef struct _tagNMSEARCHINFO 
{
	NMWLHDR hdr ;
	CString sKeyWord ;
} NMSEARCHINFO, * LPNMSEARCHINFO ;

// message
#define WM_ENDSEARCH		(WM_USER + 2000)

// command
#define SBN_UPDATESEARCH	(NM_WLFIRST - 300U)
#define SBN_BEGINSEARCH		(NM_WLFIRST - 301U)
#define SBN_ENDSEARCH		(NM_WLFIRST - 302U)
#define SBN_UP				(NM_WLFIRST - 303U)
#define SBN_DOWN			(NM_WLFIRST - 304U)
#define SBN_RETURN			(NM_WLFIRST - 305U)

class WLSearchInput :
	public WLBubbleEdit
{
public:
	WLSearchInput(void);
	virtual ~WLSearchInput(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	void SetSearchIcon(UINT uMagnifierId) ;
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

	void SetEnglishIME() ;		// 设置英文输入法
	void RevertIME() ;			// 还原输入法

protected:
	//////////////////////////////////////////////////////////////////////////
	// Data
	Shared_ImageEx m_imgMagnifier ;
	//Shared_ImageEx m_imgBkgnd ;

	// 状态
	enum
	{
		SBS_HASCONTENT	= 0x08,
	};

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
