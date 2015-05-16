#pragma once

#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLButtonAL.h"
#include "uilib/wndless/IWLProgress.h"
#include "uilib/wndless/IWLStatic.h"
#include <vector>

#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"
#include "uilib/common/utils/CanvasDC.h"

class BaseWndLessRoot :
	public CWndLessImpBase<CWndLessRoot>
{
public:
	BaseWndLessRoot(void);
	virtual ~BaseWndLessRoot(void);

public:
    void SetMaxSize(int cx, int cy) ;
    void SetMaxPosition(int x, int y) ;
    void SetMinTrackSize(int cx, int cy) ;
    void SetMaxTrackSize(int cx, int cy) ;

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Interface

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    virtual void OnDestroy() ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler

    LRESULT OnNcHitTest(POINT const &point) ;
    void OnGetMinMaxInfo(MINMAXINFO* lpMMI) ;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

	virtual BOOL OnMenuCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual BOOL OnAcceleratorCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual BOOL OnControlCommand(WORD id, WORD code, HWL wnd, LRESULT * pResult) ;

	virtual void OnClose() ;

	//////////////////////////////////////////////////////////////////////////
	// Implement

	int GetTitleHeight() const { return m_title_height ; }
	HFONT GetFont() const { return m_hFont ; }

protected:

	//////////////////////////////////////////////////////////////////////////
	// Inner Function

	BOOL PollMessage(HWL hChild, UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	void GainWindowTitle() ;
	void InitImage() ;
	void CalcBkgndGraySale() ;
	void PrepareBorder() ;
	void InitCaptionButton() ;

	void InitCtrls() ;

	void PrepareBkgnd() ;
	virtual void DrawBkgnd() ;
	void DrawTitle(HDC hdc, BYTE bkgnd_gray_scale) ;
	void RelayoutCaptionButton(int width) ;

	//////////
	virtual void OnRestored(WNDSIZE enPrev);
	virtual void OnMaximized(WNDSIZE enPrev) ;

protected:
	CCanvasDC m_bkgnd_dc ;
	int m_title_height ;			// 标题高度
	BOOL m_bDrawTitle;

	BYTE m_bkgnd_gray_scale ;
	CCanvasDC m_border_dc ;

	HICON m_hIcon ;		// 窗口图标

	// 窗口风格
	enum
	{
		WND_STYLE_CAPTION		= 0x00010000,
		WND_STYLE_BORDER		= 0x00020000,
		WND_STYLE_RESIZABLE		= 0x00040000,
		WND_STYLE_MINIMIZEBOX	= 0x00080000,
		WND_STYLE_MAXIMIZEBOX	= 0x00100000,
		WND_STYLE_SYSTEMMENU	= 0x00200000,
	};
	DWORD m_wnd_style ;

private:
	int m_nRndDia ;		// 窗口圆角直径
	HFONT m_hFont ;		// 窗口字体

	static BOOL s_vista_upper ;		// 是否Vista操作系统及以上

	int m_border_width ;			// 边框宽度，窗口改变大小用

	// 标题栏按钮
	Shared_ImageEx m_img_min_btn ;
	Shared_ImageEx m_img_max_btn ;
	Shared_ImageEx m_img_restore_btn ;
	Shared_ImageEx m_img_close_btn ;

	enum
	{
		ID_BTN_MIN		= 47, 
		ID_BTN_MAX		= 48, 
		ID_BTN_RESTORE	= 49, 
		ID_BTN_CLOSE	= 50, 
	};

	WLButtonAL m_min_btn ;
	WLButtonAL m_max_btn ;
	WLButtonAL m_restore_btn ;
	WLButtonAL m_close_btn ;

	CCanvasDC m_wnd_top_dc ;
    BOOL m_max_size_valid ;
    BOOL m_max_position_valid ;
    BOOL m_min_track_size_valid ;
    BOOL m_max_track_size_valid ;
    MINMAXINFO m_min_max_info ;

};
