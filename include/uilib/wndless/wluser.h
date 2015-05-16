
#ifndef _WLUSER_
#define _WLUSER_

#pragma once

class IWndLess ;
typedef IWndLess * HWL ;

//////////////////////////////////////////////////////////////////////////
// Skin
#define MSG_SKIN_CHANGED							WM_USER + 1000


//////////////////////////////////////////////////////////////////////////
// Internal Window Less Handle

#define HWL_TOP			((HWL)0)
#define HWL_BOTTOM		((HWL)1)

//////////////////////////////////////////////////////////////////////////
// Internal Window Style

#define WS_SIZEPOSCHANGEDNOTIFYPARENT WS_POPUP			// 大小、位置改变通知父窗口
#define WS_ILLUSION			WS_CHILD					// 幻象风格，看得见，但是无法被HitTest
#define WS_GLOWTEXT			WS_CLIPSIBLINGS				// 发光文字	

//////////////////////////////////////////////////////////////////////////
// Internal Message

#define WM_INTERNAL_MAX		0x0000FFFF					// High Word内部使用

//////////////////////////////////////////////////////////////////////////
// utility
#define WM_LBUTTONCLICK		(WM_INTERNAL_MAX)			// 左键点击消息
#define WM_RBUTTONCLICK		(WM_INTERNAL_MAX - 1)		// 右键点击消息

// 特殊
#define WM_LDOWNHOST		(WM_INTERNAL_MAX - 1000)	// 左键点击宿主，该消息发给焦点窗口

// wparam: HDC
// lparam: LPRECT
#define WM_QUERYBKGND		(WM_INTERNAL_MAX - 1001)	// 请求背景

// wparam: 背景平均颜色灰度
#define WM_BKGNDCHANGED		(WM_INTERNAL_MAX - 1002)	// 背景改变通知消息

//////////////////////////////////////////////////////////////////////////
// Scrollbar

// 滚动条显示或隐藏
// wParam: show(TRUE)/hide(FALSE)
// lParam: Scrollbar pointer
#define WM_SCROLLBAR_SHOW	(WM_INTERNAL_MAX - 1100)

//////////////////////////////////////////////////////////////////////////
// WndLess Notify

#define NM_WLFIRST	(0U - 2010U)
typedef struct _tagNMWLHDR
{
	HWL hWlFrom ;
	UINT_PTR idFrom ;
	UINT code ;
} NMWLHDR, *LPNMWLHDR ;

#define NM_SIZED	(NM_WLFIRST - 1)	// 窗口大小改变
typedef struct _tagNMWLSIZEDINFO
{
	NMWLHDR hdr ;
	SIZE szOld ;
	SIZE szNew ;
}NMWLSIZEDINFO, *LPNMWLSIZEDINFO ;

#define NM_MOVED	(NM_WLFIRST - 2)	// 窗口位置改变
typedef struct _tagNMWLMOVEDINFO
{
	NMWLHDR hdr ;
	POINT ptOld ;
	POINT ptNew ;
}NMWLMOVEDINFO, *LPNMWLMOVEDINFO ;

//////////////////////////////////////////////////////////////////////////
// Internal Control Id

#define CTRLID_TOOLTIP							0x00000000

//////////////////////////////////////////////////////////////////////////
// Internal Timer Id
// 系统预留2^31个定时器
// 自定义定时器必须小于TIMERID_INTERNAL_FIRST

#define TIMERID_INTERNAL_FIRST					(UINT_PTR)0x10000000
#define TIMERID_INTERNAL_LAST					(UINT_PTR)0xFFFFFFFF

// Edit
#define TIMERID_EDIT_CARET						(UINT_PTR)(TIMERID_INTERNAL_FIRST)

// Scroll Bar
#define TIMERID_SCROLLBAR_ARROW_FIRSTDOWN		(UINT_PTR)(TIMERID_INTERNAL_FIRST + 1)
#define TIMERID_SCROLLBAR_ARROW_KEEPDOWN		(UINT_PTR)(TIMERID_INTERNAL_FIRST + 2)
#define TIMERID_SCROLLBAR_CHANNEL_FIRSTDOWN		(UINT_PTR)(TIMERID_INTERNAL_FIRST + 3)
#define TIMERID_SCROLLBAR_CHANNEL_KEEPDOWN		(UINT_PTR)(TIMERID_INTERNAL_FIRST + 4)

// Animation
#define TIMERID_ANIMATION_PLAY					(UINT_PTR)(TIMERID_INTERNAL_FIRST + 5)

//////////////////////////////////////////////////////////////////////////
// Scroll Bar Commands

#define SB_WHEELUP		47
#define SB_WHEELDOWN	48

//////////////////////////////////////////////////////////////////////////
// String

// 由所有者决定其文本
#ifdef UNICODE
#define LPSTR_TEXTCALLBACK_OWNER	((LPWSTR)-2L)
#else
#define LPSTR_TEXTCALLBACK_OWNER	((LPSTR)-2L)
#endif

#endif
