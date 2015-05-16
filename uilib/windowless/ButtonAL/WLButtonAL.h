//////////////////////////////////////////////////////////////////////////
//
//	描述:   按钮
//			支持Push, Check, Radio, Link四种风格
//			前景图片由各种状态图片水平拼装而成，根据状态数自动匹配
//			Push类型按钮各状态拼装格式一般有如下3种情况(详见s_arPushStateMap)：
//			a. 5 states - normal, hot, pushed, focus/default, disable
//			b. 4 states - normal, hot, pushed, disable
//			c. 3 states - normal, hot, pushed
//			Check或Radio各状态拼装格式一般有如下2种情况(详见s_arCheckStateMap)：
//			a. 8 states - normal, normal_hot, normal_pushed, normal_disable,
//						  selected, selected_hot, selected_pushed, selected_disable
//			b. 3 states - normal, hot, selected
//
//	缺陷:	按钮位置改变时未刷新
//	
//	日期:   2011/02/16
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "IWLButtonAL.h"
#include "common/utils/CanvasDC.h"

class WLButtonALImp :
	public WLButtonALImpBase
{
public:
	WLButtonALImp(void);
	virtual ~WLButtonALImp(void);

public:
	// 按钮类型
	enum BTN_TYPE
	{
		PUSH		= 0x0000,
		CHECK		= 0x0001,
		RADIO		= 0x0002,
		LINK		= 0x0003,		// 按钮按下时不移动文本，鼠标移上去时变为手状
	} ;

	// 按钮风格
	enum BTN_STYLE
	{
		NORMAL			= 0x0000,
		PUSHLIKE		= 0x0001,		// Check和Radio风格时有效
		FITFRGNDSIZE	= 0x0002,		// 根据前景自动调整大小
		FITTEXTSIZE		= 0x0004,		// 根据文字自动调整大小
	} ;

	// 背景风格
	enum BKGND_STYLE
	{
		NOBKGND		= 0x0000,
		HANDPAINT	= 0x0001,		// 手绘背景，即指定各种颜色进行填充
		TRANSLUCENT	= 0x0002,		// 背景透明，使用父窗口背景作为背景，
	} ;

	// 前景风格
	enum FRGND_STYLE
	{
		NOFRGND		= 0x0000,		// 无前景
		FIXED		= 0x0001,		// 固定整个前景
		FITX		= 0x0002,		// 固定前景高度，x方向采用01...12拆分绘制
		FITY		= 0x0004,		// 固定前景宽度，y方向采用01...12拆分绘制
		FITXY		= 0x0006,		// x、y方向均采用01...12绘制(未实现)
	} ;

	// 文本风格，文本包括icon和text
	// BTN_STYLE不自动调整大小时，CENTER、LEFT、RIGHT、TOP、BOTTOM有效
	// FITTEXTSIZE
	//	FITFRGNDSIZE + FITX: TOP, BOTTOM 有效
	//	FITFRGNDSIZE + FITY: LEFT, RIGHT 有效
	enum TEXT_STYLE
	{
// 		TS_NEAR		= 0x0001,
// 		TS_CENTER	= 0x0002,
// 		TS_FAR		= 0x0004,
// 		TS_VERTICAL	= 0x0008,		// 垂直绘制(未实现)

		TS_LEFT		= 0x0001,
		TS_HCENTER	= 0x0000,
		TS_RIGHT	= 0x0002,
		TS_TOP		= 0x0010,
		TS_VCENTER	= 0x0000,
		TS_BOTTOM	= 0x0020,

		TS_CENTER	= 0x0000,
	} ;

	enum BKGND_COLOR_INDEX
	{
		BCI_NORMAL				= 0x0001,
		BCI_HOT					= 0x0002,
		BCI_PUSHED				= 0x0004,
		BCI_DEFAULT				= 0x0008,
		BCI_DISABLE				= 0x0010,

		BCI_UNCHECKED			= 0x0001,
		BCI_UNCHECKED_HOT		= 0x0002,
		BCI_UNCHECKED_PUSHED	= 0x0004,
		BCI_UNCHECKED_DISABLE	= 0x0008,
		BCI_CHECKED				= 0x0010,
		BCI_CHECKED_HOT			= 0x0020,
		BCI_CHECKED_PUSHED		= 0x0040,
		BCI_CHECKED_DISABLE		= 0x0080,

		BCI_ALL					= 0xFFFF,
	};

protected:
	DWORD m_dwBtnType ;
	DWORD m_dwBtnStyle ;
	DWORD m_dwBkgndStyle ;
	DWORD m_dwFrgndStyle ;
	DWORD m_dwTextStyle ;

	COLORREF m_crBkgnd[8] ;
	COLORREF m_crText[8] ;

	Image * m_pImgFrgnd ;
	//CCanvasDC m_cvFrgnd ;

	BYTE m_arStateMap[8] ;
	int m_nFrgndStateCount ;
	Image * m_pImgCheckSign ;		// checkbox和radiobox按钮都使用该图为符号图

	static BYTE s_arPushStateMap[5][5] ;
	static BYTE s_arCheckStateMap[8][8] ;

	Image * m_pImgIcon ;
	//CCanvasDC m_cvIcon ;

	// 当存在FITTEXTSIZE风格时有用
	int m_nLeftMargin ;		// 边缘
	int m_nRightMargin ;
	int m_nTopMargin ;
	int m_nBottomMargin ;

	int m_nSignTextSpace ;	// 符号和文本(文本包括图标和文字)之间的间隙
	int m_nIconTextSpace ;	// 图标和文字之间的间隙

	CPoint m_ptSign ;
	CPoint m_ptIcon ;
	CPoint m_ptText ;

	// CHECK/RADIO类型按钮内部数据
	int m_nCheck ;

	// LINK类型按钮内部数据
	CString m_sUrl ;

	//CDC * m_pDCParentBkgnd ;

	// 发光的与背景反色的文字颜色
	COLORREF m_crGlowText ;
	COLORREF m_crGlowTextHalo ;
	COLORREF m_crGlowTextBkgnd ;

	BOOL m_bIconAciton;

public:
	// 设置前景
	void SetFrgnd(
		Image * pImgFrgnd,
		int nStateCount,
		DWORD dwFrgndStyle,
		BOOL bDefer = FALSE
		) ;

	// 设置符号
	void SetSign(
		Image * pImgCheckSign,
		BOOL bDefer = FALSE
		) ;

	// 设置图标
	void SetIcon(
		Image * pImgIcon,
		BOOL bDefer = FALSE,
		BOOL bAction = TRUE
		) ;

	// 设置类型
	void SetType(
		DWORD dwBtnType,
		BOOL bDefer = FALSE
		) ;

	// 设置风格
	void SetStyle(
		DWORD dwBtnStyle,
		BOOL bDefer = FALSE
		) ;

	// 设置背景风格
	void SetBkgndStyle(
		DWORD dwBkgndStyle,
		BOOL bUpdate = FALSE
		) ;

	// 设置文本风格
	void SetTextStyle(
		DWORD dwTextStyle,
		BOOL bDefer = FALSE
		) ;

	void SetMargin(int nLeft, int nTop, int nRight, int nBottom, BOOL bDefer = FALSE) ;
	void SetSignTextSpace(int nSignTextSpace, BOOL bDefer = FALSE) ;
	void SetIconTextSpace(int nIconTextSpace, BOOL bDefer = FALSE) ;

	void SetBkgndColor(DWORD dwMask, ...) ;
	void SetTextColor(DWORD dwMask, ...) ;

	void EndDeferRecalcLayout() ;

	void SetUrl(CString const & sUrl) 
	{
		m_sUrl = sUrl ;
	}

protected:
	void RecalcLayout() ;
	void CalcSignIconText(CSize &szSignIconText, CPoint &ptSign, CPoint &ptIcon, CPoint &ptText) ;

	void Draw(HDC hDC, CRect const &rc, DWORD dwState) ;
	void DrawPush(HDC hDC, CRect const &rc, DWORD dwState, BOOL bNormalOrCheck) ;
	void DrawCheckRadio(HDC hDC, CRect const &rc, DWORD dwState) ;
	void DrawLink(HDC hDC, CRect const &rc, DWORD dwState) ;

	void GuessStateMap(int nStateCount) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

public:
	virtual BOOL SetWindowText(LPCTSTR lpszText) ;

	//////////////////////////////////////////////////////////////////////////
	// WLButton Implement

protected:
	virtual BOOL DrawBySelf(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler
	BOOL OnSetCursor() ;

protected:
	afx_msg BOOL OnBnClicked();
};
