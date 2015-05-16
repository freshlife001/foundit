//////////////////////////////////////////////////////////////////////////
//
//	描述:   无句柄滚动条
//
//	备注:	STYLE_DISABLENOSCROLL尚未完成
//
//	
//	日期:   2011/02/10
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "IWLScrollBar.h"
#include "WLScrollBarArrow.h"
#include "WLScrollBarThumb.h"
#include "common\utils\CanvasDC.h"
#include "common/utils/ImageEx/Shared_ImageEx.h"

// typedef struct _tagSCROLLINFOEX
// {
// 	UINT cbSize;
// 	UINT fMask;
// 	UINT nMin;
// 	UINT nMax;
// 	UINT nPage;
// 	UINT nPos;
// 	UINT nTrackPos;
// }SCROLLINFOEX, *LPSCROLLINFOEX ;
// typedef SCROLLINFOEX const * LPCSCROLLINFOEX ;

class WLScrollBarImp :
	public WLScrollBarImpBase
{
public:
	WLScrollBarImp(void);
	virtual ~WLScrollBarImp(void);

	enum
	{
		STYLE_HORZ				= 0x00000000L,
		STYLE_VERT				= 0x00000001L,
		STYLE_TOPALIGN			= 0x00000000L,
		STYLE_LEFTALIGN			= 0x00000000L,
		STYLE_BOTTOMALIGN		= 0x00000002L,
		STYLE_RIGHTALIGN		= 0x00000002L,
		STYLE_NOARROW			= 0x00000004L,
		//STYLE_DISABLENOSCROLL	= 0x00000008L,
	};

public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	BOOL SetImage(UINT uID) ;
	int GetScrollWidth() const { return m_nButtonSide ; }

	int SetScrollInfo(LPCSCROLLINFO lpsi) ;
	BOOL GetScrollInfo(
		LPSCROLLINFO lpScrollInfo,
		UINT nMask = SIF_ALL 
		) ;

public:
	virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual void OnDestroy() ;

	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler
	void OnBtnUpCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBtnDownCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBtnThumbCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;

	void OnBtnUpLDown(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBtnDownLDown(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;

	void OnBtnThumbBeginDrag(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBtnThumbDragging(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;
	void OnBtnThumbEndDrag(LPNMWLHDR pNMWLHDR, LRESULT * pResult) ;

	void OnMouseMove(UINT nFlags, CPoint point) ;
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnCaptureChanged(HWL hWnd) ;
	void OnTimer(UINT_PTR nIDEvent) ;

	//////////////////////////////////////////////////////////////////////////
	// Inner Function
	int ScrollPosFromThumbPos(int nThumbPos) ;
	int ThumbPosFromScrollPos(int nScrollPos) ;

	int ThumbLengthFromScrollPage(UINT nPage) ;
	int inline GetMinThumbLength() const ;

	//int ThumbLengthFromScrollPage(int nScrollPage) ;
	//int ThumbLengthFromScrollPosAndPage(int nScrollPos, int nScrollPage) ;
	void SendScrollMessage(UINT nSBCode, UINT nPos) ;

	enum LDOWNPOS ;
	LDOWNPOS HitTestLDownPos(CPoint const &point) ;

// Data
protected:
	// 图像信息
// 	CBitmap m_bmpCombo ;
	CSize m_szComboImageSize ;
	CCanvasDC m_cvComboImage ;	// 组合图像

// 	CBitmap * m_pComboImageOldBmp ;
// 	CDC m_dcComboImage ;

	Shared_ImageEx m_imgCombo ;

	// 控件信息
	enum CTRLID
	{
		IDC_BTN_UP		= 0,
		IDC_BTN_DOWN	= 1,
		IDC_BTN_THUMB	= 2,
	};

	int m_nButtonSide ;
	int m_nChannelBegin ;
	int m_nChannelEnd ;

	int m_nThumbHeadLength ;
	int m_nThumbTailLength ;
	//int m_nMinThumbLength ;
	int m_nThumbLength ;
	//int m_nThumbLength ;				// 由于总是存在误差，即使在页值固定的情况下，
										// 滑块长度也可能因为滑块位置变化而变化，所以不记录该值

	WLScrollBarArrow m_btnUp ;			// 向上向左按钮
	WLScrollBarArrow m_btnDown ;		// 向下向右按钮
	WLScrollBarThumb m_btnThumb ;		// 滑块按钮

	CPoint m_ptBeginDragCursorInThumb ;	// 滑块按钮开始移动时鼠标在滑块中的位置

	// 滚动条信息
	int		m_nMin ;
	int		m_nMax ;
	UINT	m_uPage ;
	int		m_nPos ;
	int		m_nTrackPos ;

	// 鼠标按在滑道上的相关属性
	enum LDOWNPOS
	{
		LDOWN_NOTHING = 0,
		LDOWN_FRONT = 1,
		LDOWN_BACK = 2,
	} m_enLDownPos ;
	enum HIGHLIGHTPART
	{
		HIGHLIGHT_NOTHING = 0,
		HIGHLIGHT_FRONT = 1,
		HIGHLIGHT_BACK = 2,
	} m_enHighLightPart ;

	// 定时器
	UINT_PTR m_uFirstLDownTimer ;
	UINT_PTR m_uKeepLDownTimer ;

};
