//////////////////////////////////////////////////////////////////////////
//
//	描述:   无句柄窗口的实现
//			没有任何功能，需进一步响应OnDraw和OnWndMsg来完成具体功能
//
//	
//	日期:   2010/12/24
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "iwndless.h"

#include <map>
#include <set>

typedef CWndLessImpBase< IWndLess >  WndLessImpBase;

class CWndLessImp :public WndLessImpBase
{
public:
	CWndLessImp();
	virtual ~CWndLessImp(void);

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	// Init
	BOOL Create(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent,
		UINT uID
		) ;
	virtual BOOL OnCreate(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent) ;
	virtual void Destroy() ;
	virtual void OnDestroy() ;

	virtual IWndLessCenter * GetCenter() const ;
	virtual IWndLess * GetParent() const ;
	virtual DWORD GetStyle() const ;
	virtual UINT GetID() const ;

	virtual void SetCenter(IWndLessCenter *);

	// Message Handler
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

	// Operation
	virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual void OnDrawChild(HDC hDC, RECT const &rcUpdate) ;
	virtual void UpdateView(RECT const* pRect = NULL) ;

	virtual BOOL IsWndLessVisible() const ;
	virtual BOOL IsWndLessShow() const ;
	virtual BOOL IsWndLessEnable() const ;
    virtual BOOL IsWndLessWindow() const ;

	virtual HWL SetFocus() ;
	virtual HWL GetFocus() const ;

	virtual HRESULT RegisterDragDrop(LPDROPTARGET pDropTarget) ;
	virtual HRESULT RevokeDragDrop() ;

	virtual HWL SetCapture() ;
	virtual HWL GetCapture() const ;
	virtual HWL ReleaseCapture() ;

	virtual UINT_PTR SetTimer(
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) ;
	virtual BOOL KillTimer(
		UINT nIDEvent
		) ;

	virtual BOOL EnableWindow(BOOL bEnable) ;

	virtual void GetRectInScreen(LPRECT lpRect) const ;			// 获得客户区在屏幕坐标系中的位置
	virtual void GetRectInHost(LPRECT lpRect) const ;			// 获得客户区在宿主坐标系中的位置
	virtual void GetRectInParent(LPRECT lpRect) const ;			// 获得客户区在父窗口坐标系中的位置
	virtual void GetRectInClient(LPRECT lpRect) const ;			// 获得客户区在客户区坐标系中的位置

	virtual void ScreenToClient(LPPOINT lpPoint) const ;		// 坐标转换
	virtual void ScreenToClient(LPRECT lpRect) const ;

	virtual void ClientToScreen(LPPOINT lpPoint) const ;
	virtual void ClientToScreen(LPRECT lpRect) const ;

	virtual void HostToClient(LPPOINT lpPoint) const ;
	virtual void HostToClient(LPRECT lpRect) const ;

	virtual void ClientToHost(LPPOINT lpPoint) const ;
	virtual void ClientToHost(LPRECT lpRect) const ;

	virtual void ParentToClient(LPPOINT lpPoint) const ;
	virtual void ParentToClient(LPRECT lpRect) const ;

	virtual void ClientToParent(LPPOINT lpPoint) const ;
	virtual void ClientToParent(LPRECT lpRect) const ;

	virtual void GetPos(LPPOINT lpPoint) const ;
	virtual void GetSize(LPSIZE lpSize) const ;

	virtual void SetWndLessPos(int x, int y, int cx, int cy, UINT uFlags) ;
	virtual void OnMoved(POINT const &ptOld, POINT const &ptNew) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
    virtual void OnFlashed(bool old_status, bool new_status) { old_status ; new_status ; }

	virtual BOOL AddChild(HWL hChild) ;
	virtual BOOL DelChild(HWL hChild) ;
	virtual HWL GetChild(UINT uID) const ;
	virtual BOOL IsChild(HWL hChild, BOOL bGrandson) const ;

	virtual HWL GetFirstChildHwl() const ;
	virtual HWL GetLastChildHwl() const ;
	virtual void SetFirstChildHwl(HWL hFirstChild) ;

	virtual HWL GetPrevSiblingHwl() const ;
	virtual HWL GetNextSiblingHwl() const ;
	virtual void SetPrevSiblingHwl(HWL hSibling) ;
	virtual void SetNextSiblingHwl(HWL hSibling) ;

	virtual HWL _InternalHitTest(POINT const &pt) ;
	virtual HWL HitTestChild(POINT const &pt) ;
	virtual BOOL PtInWndLess(POINT const &pt) const ;

	virtual BOOL SetWindowText(LPCTSTR lpszText) ;
	virtual int GetWindowText(CString &sText) const ;
	virtual CString & GetWindowTextRef();

	virtual void SetWndLessStyle(DWORD dwStyle) ;
    virtual DWORD GetWndLessStyle() const ;

    virtual HFONT GetFont() const ;
    virtual void SetFont(HFONT font) ;

	virtual LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam) ;
	virtual void SetDisplayListener(DisplayListener * listener) { m_display_listener = listener ; }

	// End Of Implement
	//////////////////////////////////////////////////////////////////////////

	// 用Tab键移动焦点到下一个孩子
	virtual void TabFocusToNextChild(HWL hChild, BOOL bPrevious) ;

	// 获得下一个Tab键移到的孩子
	HWL GetNextTabChild(HWL hChild, BOOL bPrevious) ;

	HWL GetTopLevelParent() const ;

	BOOL AddTooltip(LPCTSTR lpszTip) ;
	BOOL UpdateTooltip(LPCTSTR lpszTip) ;
	BOOL ShowTooltip(BOOL bShow) ;
	BOOL RemoveTooltip() ;

	BOOL SetZorder(HWL hInsertAfter) ;

	// IME
	void UpdateImeCompositionWindowPos(int x, int y) const ;

protected:
	// 属性
	DWORD m_dwStyle ;
	UINT m_uID ;
	CString m_sText ;
	CPoint m_ptTopLeftInParent ;	// 窗口左上角在父窗口坐标系中的位置
	CSize m_szClient ;				// 窗口客户区大小
    BOOL m_isCreated;

	// 关系网
	IWndLessCenter * m_pCenter ;

	IWndLess * m_pParent ;
	HWL m_hPrevSiblingHwl ;
	HWL m_hNextSiblingHwl ;
	HWL m_hFirstChildHwl ;

	std::map<UINT, HWL> m_mapID2Child ;		// 有id子窗口
	std::set<HWL> m_setNoIDChildren ;		// 无id子窗口
	std::set<HWL> m_setAllChildren ;		// 所有子窗口

	// 辅助数据
	BOOL m_bLButtonDown ;					// 左键按下
	BOOL m_bRButtonDown ;					// 右键按下
	
	// 字体
    HFONT m_font ;

    DisplayListener * m_display_listener ;

#ifdef _DEBUG // 绘制Debug用Frame
protected:
	COLORREF m_debugFrameColor;
#define DEFINE_DEBUG_FRAME_COLOR( color ) ( m_debugFrameColor = color )
#endif

};


