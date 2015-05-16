//////////////////////////////////////////////////////////////////////////
//
//	描述:   无句柄窗口系统基类
//			IWndLess是无句柄窗口基类
//			IWndLessCenter是无句柄窗口中心基类，一般要附属有句柄窗口来完成
//	
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

#include "wluser.h"
#include "Concept.h"

class IWndLess ;
typedef IWndLess* HWL ;

class IWndLessCenter ;

class IWndLess
{
public:
	virtual ~IWndLess(void){};
// Init
	virtual BOOL Create(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent,
		UINT uID
		)  = 0;
	virtual BOOL OnCreate(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent
		) = 0 ;
	virtual void Destroy()  = 0;
	virtual void OnDestroy() = 0 ;

	virtual IWndLessCenter * GetCenter() const = 0 ;
	virtual IWndLess * GetParent() const = 0 ;
	virtual DWORD GetStyle() const = 0 ;
	virtual UINT GetID() const = 0 ;

	virtual void SetCenter(IWndLessCenter *) = 0 ;

// Message Handler
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) = 0 ;

// Operation
	virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate) = 0 ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) = 0 ;
	virtual void OnDrawChild(HDC hDC, RECT const &rcUpdate) = 0 ;
	virtual void UpdateView(RECT const* pRect = NULL) = 0 ;

	virtual BOOL IsWndLessVisible() const = 0 ;
	virtual BOOL IsWndLessShow() const = 0 ;
	virtual BOOL IsWndLessEnable() const = 0 ;
    virtual BOOL IsWndLessWindow() const = 0;

	virtual HWL SetFocus() = 0 ;
	virtual HWL GetFocus() const = 0 ;

	virtual HRESULT RegisterDragDrop(LPDROPTARGET pDropTarget) = 0;
	virtual HRESULT RevokeDragDrop() = 0;

	virtual HWL SetCapture() = 0 ;
	virtual HWL GetCapture() const = 0 ;
	virtual HWL ReleaseCapture() = 0 ;

	virtual UINT_PTR SetTimer(
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) = 0 ;
	virtual BOOL KillTimer(
		UINT nIDEvent
		) = 0 ;

	virtual BOOL EnableWindow(BOOL bEnable) = 0 ;

	virtual void GetRectInScreen(LPRECT lpRect) const = 0 ;			// 获得客户区在屏幕坐标系中的位置
	virtual void GetRectInHost(LPRECT lpRect) const = 0 ;			// 获得客户区在宿主坐标系中的位置
	virtual void GetRectInParent(LPRECT lpRect) const = 0 ;			// 获得客户区在父窗口坐标系中的位置
	virtual void GetRectInClient(LPRECT lpRect) const = 0 ;			// 获得客户区在客户区坐标系中的位置

	virtual void ScreenToClient(LPPOINT lpPoint) const = 0 ;		// 坐标转换
	virtual void ScreenToClient(LPRECT lpRect) const = 0 ;

	virtual void ClientToScreen(LPPOINT lpPoint) const = 0 ;
	virtual void ClientToScreen(LPRECT lpRect) const = 0 ;

	virtual void HostToClient(LPPOINT lpPoint) const = 0 ;
	virtual void HostToClient(LPRECT lpRect) const = 0 ;

	virtual void ClientToHost(LPPOINT lpPoint) const = 0 ;
	virtual void ClientToHost(LPRECT lpRect) const = 0 ;

	virtual void ParentToClient(LPPOINT lpPoint) const = 0 ;
	virtual void ParentToClient(LPRECT lpRect) const = 0 ;

	virtual void ClientToParent(LPPOINT lpPoint) const = 0 ;
	virtual void ClientToParent(LPRECT lpRect) const = 0 ;

	virtual void GetSize(LPSIZE lpSize) const = 0 ;
	virtual void GetPos(LPPOINT lpPoint) const  = 0;

	enum 
	{
		SWLP_X			= 0x00000001,
		SWLP_X_OFFSET	= 0x00000010,

		SWLP_Y			= 0x00000002,
		SWLP_Y_OFFSET	= 0x00000020,

		SWLP_CX			= 0x00000004,
		SWLP_CX_OFFSET	= 0x00000040,

		SWLP_CY			= 0x00000008,
		SWLP_CY_OFFSET	= 0x00000080,

		SWLP_SHOW		= 0x00010000,
		SWLP_HIDE		= 0x00020000,
		//SWLP_DEFER		= 0x00040000,	// 延迟执行
		SWLP_NOREDRAW	= 0x00040000,		// 不重绘
		SWLP_NOTIFYCHANGE		= 0x00080000,	// 窗口位置变化、大小变化、消失或隐藏时通知监听者

		SWLP_DEFER		= 0x00100000,	// 延迟执行
	};
	virtual void SetWndLessPos(int x, int y, int cx, int cy, UINT uFlags) = 0 ;
	virtual void OnMoved(POINT const &ptOld, POINT const &ptNew) = 0 ;
    virtual void OnSized(SIZE const &szOld, SIZE const &szNew) = 0 ;
    virtual void OnFlashed(bool old_status, bool new_status) = 0 ;

	virtual BOOL AddChild(HWL hChild) = 0 ;
	virtual BOOL DelChild(HWL hChild) = 0 ;
	virtual HWL GetChild(UINT uID) const = 0 ;
	virtual BOOL IsChild(HWL hChild, BOOL bGrandson) const = 0 ;

	virtual HWL GetFirstChildHwl() const = 0 ;
	virtual HWL GetLastChildHwl() const = 0 ;
	virtual void SetFirstChildHwl(HWL hFirstChild) = 0 ;

	virtual HWL GetPrevSiblingHwl() const = 0 ;
	virtual HWL GetNextSiblingHwl() const = 0 ;
	virtual void SetPrevSiblingHwl(HWL hSibling) = 0 ;
	virtual void SetNextSiblingHwl(HWL hSibling) = 0 ;

	virtual HWL _InternalHitTest(POINT const &pt) = 0 ;
	virtual HWL HitTestChild(POINT const &pt) = 0 ;
	virtual BOOL PtInWndLess(POINT const &pt) const = 0 ;

	virtual BOOL SetWindowText(LPCTSTR lpszText) = 0 ;
	virtual int GetWindowText(CString &sText) const = 0 ;
	virtual CString & GetWindowTextRef() = 0 ;

	virtual void SetWndLessStyle(DWORD dwStyle) = 0 ;
	virtual DWORD GetWndLessStyle() const = 0 ;

    virtual HFONT GetFont() const = 0 ;
    virtual void SetFont(HFONT font) = 0 ;

	virtual LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0 ;

    // 无句柄窗口外观监视者
    class DisplayListener
    {
    public:
        virtual void OnWndLessSizeChanged(IWndLess * /*wnd*/, SIZE const &/*old_size*/, SIZE const &/*new_size*/) {}
        virtual void OnWndLessPositionChanged(IWndLess * /*wnd*/, POINT const &/*old_pos*/, POINT const &/*new_pos*/) {}
        virtual void OnWndLessShowHideChanged(IWndLess * /*wnd*/, bool /*old_status*/, bool /*new_status*/) {}
    };
    virtual void SetDisplayListener(DisplayListener * listener) = 0 ;

	// 用Tab键移动焦点到下一个孩子
	virtual void TabFocusToNextChild(HWL hChild, BOOL bPrevious)  = 0;
	// 获得下一个Tab键移到的孩子
	virtual HWL GetNextTabChild(HWL hChild, BOOL bPrevious) = 0;

	virtual HWL GetTopLevelParent() const  = 0;

	virtual BOOL AddTooltip(LPCTSTR lpszTip) = 0;
	virtual BOOL UpdateTooltip(LPCTSTR lpszTip)  = 0;
	virtual BOOL ShowTooltip(BOOL bShow) = 0 ;
	virtual BOOL RemoveTooltip()  = 0;

	virtual BOOL SetZorder(HWL hInsertAfter) = 0 ;

	// IME
	virtual void UpdateImeCompositionWindowPos(int x, int y) const  = 0;

};

class IWndLessCenter : public IWndLess
{
public:
// With Host
	virtual BOOL CreateEx(HWND hHost) = 0 ;
	virtual HWND GetHost() const = 0 ;

// Manage
	virtual BOOL AddWndLess(HWL) = 0 ;
	virtual BOOL DelWndLess(HWL) = 0 ;

	virtual BOOL IsWndLess(HWL) const = 0 ;
	virtual LRESULT SendMessageEx(HWL, UINT, WPARAM, LPARAM) = 0 ;

	virtual HWL C_SetFocus(HWL) = 0 ;
	virtual HWL C_GetFocus() const = 0 ;

	virtual HRESULT C_RegisterDragDrop(HWL hChild, LPDROPTARGET pDropTarget) = 0 ;
	virtual HRESULT C_RevokeDragDrop(HWL hChild) = 0 ;

	virtual HWL C_SetCapture(HWL) = 0 ;
	virtual HWL C_GetCapture() const = 0 ;
	virtual HWL C_ReleaseCapture(HWL) = 0 ;

	virtual UINT_PTR C_SetTimer(
		HWL hChild,
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) = 0 ;
	virtual BOOL C_KillTimer(
		HWL hChild,
		UINT nIDEvent
		) = 0 ;

	virtual void C_EnableWindow(
		HWL hChild,
		BOOL bEnable
		) = 0 ;

	virtual BOOL PreDispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) = 0 ;

	virtual BOOL DispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) = 0 ;

	virtual void EnableIme() = 0 ;
	virtual void DisableIme() = 0 ;

	virtual void EnableTooltips(BOOL bEnable) = 0 ;
	virtual BOOL AddTooltipEx(HWL hChild, LPCTSTR lpszTip) = 0 ;
	virtual BOOL UpdateTooltipEx(HWL hChild, LPCTSTR lpszTip) = 0 ;
	virtual BOOL ShowTooltipEx(HWL hChild, BOOL bShow) = 0 ;
	virtual BOOL RemoveTooltipEx(HWL hChild) = 0 ;

	enum WNDSIZE
	{
		MINIMIZED	= 0,
		RESTORED	= 1,
		MAXIMIZED	= 2,
	};
	virtual void OnMinimized(WNDSIZE enPrev) = 0;
	virtual void OnMaximized(WNDSIZE enPrev) = 0;
	virtual void OnRestored(WNDSIZE enPrev) = 0 ;
};


template <class IT = IWndLess, class IMPT = CWndLessImpBase<IWndLess> > 
class CWndLessShell :public CWndLessShellBase<IT, IMPT>
{
public:
	CWndLessShell ()
	{
	}
	virtual ~CWndLessShell()
	{
	}
	virtual BOOL Create(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent,
		UINT uID
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->Create(	dwStyle, x, y, cx, cy, pParent, uID);
		}
		return FALSE;
	};
	virtual BOOL OnCreate(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnCreate(	dwStyle, x, y, cx, cy, pParent);
		}
		return FALSE;
	};
	virtual void Destroy()
	{
		if (m_wlimp)
		{
			return m_wlimp->Destroy();
		}
		return;
	};
	virtual void OnDestroy()
	{
		if (m_wlimp)
		{
			return m_wlimp->OnDestroy();
		}
		return;
	};

	virtual IWndLessCenter * GetCenter() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCenter();
		}
		return NULL;
	};

	virtual void SetCenter(IWndLessCenter *center)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCenter(center);
		}
		return;
	}
	virtual IWndLess * GetParent() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetParent();
		}
		return NULL;
	};
	virtual DWORD GetStyle() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetStyle();
		}
		return 0 ;
	};
	virtual UINT GetID() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetID();
		}
		return 0 ;
	};

	// Message Handler
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnWndMsg(message, wParam, lParam, pResult);
		}
		return 0 ;
	};

	// Operation
	virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate)
	{
		if (m_wlimp)
		{
			return m_wlimp->_InternalOnDraw(hDC, rcUpdate);
		}
		return ;
	};
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) 
	{
		if (m_wlimp)
		{
			return m_wlimp->OnDraw(hDC, rcUpdate);
		}
		return ;
	};
	virtual void OnDrawChild(HDC hDC, RECT const &rcUpdate)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnDrawChild(hDC, rcUpdate);
		}
		return ;
	};
	virtual void UpdateView(RECT const* pRect = NULL) 
	{
		if (m_wlimp)
		{
			return m_wlimp->UpdateView(pRect);
		}
		return ;
	};
	virtual BOOL IsWndLessVisible() const
	{
		if (m_wlimp)
		{
			return m_wlimp->IsWndLessVisible();
		}
		return FALSE;
	};
	virtual BOOL IsWndLessShow() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->IsWndLessShow();
		}
		return FALSE;
	};
	virtual BOOL IsWndLessEnable() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->IsWndLessEnable();
		}
		return FALSE;
	};

	virtual BOOL IsWndLessWindow() const	
	{
		if (m_wlimp)
		{
			return m_wlimp->IsWndLessWindow();
		}
		return FALSE;
	};

	virtual HWL SetFocus() 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFocus();
		}
		return NULL;
	};
	virtual HWL GetFocus() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetFocus();
		}
		return NULL;
	};

	virtual HRESULT RegisterDragDrop(LPDROPTARGET pDropTarget)
	{
		if (m_wlimp)
		{
			return m_wlimp->RegisterDragDrop(pDropTarget);
		}
		return S_FALSE;
	};
	virtual HRESULT RevokeDragDrop()
	{
		if (m_wlimp)
		{
			return m_wlimp->RevokeDragDrop();
		}
		return S_FALSE;
	};

	virtual HWL SetCapture()
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCapture();
		}
		return NULL;
	};
	virtual HWL GetCapture() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCapture();
		}
		return NULL;
	};
	virtual HWL ReleaseCapture() 
	{
		if (m_wlimp)
		{
			return m_wlimp->ReleaseCapture();
		}
		return NULL;
	};

	virtual UINT_PTR SetTimer(
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetTimer(nIDEvent, nElapse, lpTimerFunc);
		}
		return NULL;
	};
	virtual BOOL KillTimer(
		UINT nIDEvent
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->KillTimer(nIDEvent);
		}
		return FALSE;
	};

	virtual BOOL EnableWindow(BOOL bEnable) 
	{
		if (m_wlimp)
		{
			return m_wlimp->EnableWindow(bEnable);
		}
		return FALSE;
	};

	virtual void GetRectInScreen(LPRECT lpRect) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRectInScreen(lpRect);
		}
		return;
	};
	virtual void GetRectInHost(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRectInHost(lpRect);
		}
		return;
	};
	virtual void GetRectInParent(LPRECT lpRect) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRectInParent(lpRect);
		}
		return;
	};
	virtual void GetRectInClient(LPRECT lpRect) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRectInClient(lpRect);
		}
		return;
	};

	virtual void ScreenToClient(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ScreenToClient(lpPoint);
		}
		return;
	};
	virtual void ScreenToClient(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ScreenToClient(lpRect);
		}
		return;
	};

	virtual void ClientToScreen(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToScreen(lpPoint);
		}
		return;
	};
	virtual void ClientToScreen(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToScreen(lpRect);
		}
		return;
	};
	virtual void HostToClient(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->HostToClient(lpPoint);
		}
		return;
	};
	virtual void HostToClient(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->HostToClient(lpRect);
		}
		return;
	};

	virtual void ClientToHost(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToHost(lpPoint);
		}
		return;
	};
	virtual void ClientToHost(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToHost(lpRect);
		}
		return;
	};

	virtual void ParentToClient(LPPOINT lpPoint) const
	{
		if (m_wlimp)
		{
			return m_wlimp->ParentToClient(lpPoint);
		}
		return;
	};
	virtual void ParentToClient(LPRECT lpRect) const
	{
		if (m_wlimp)
		{
			return m_wlimp->ParentToClient(lpRect);
		}
		return;
	};

	virtual void ClientToParent(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToParent(lpPoint);
		}
		return;
	};
	virtual void ClientToParent(LPRECT lpRect) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->ClientToParent(lpRect);
		}
		return;
	};

	virtual void GetPos(LPPOINT lpPoint) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetPos(lpPoint);
		}
		return;
	};

	virtual void GetSize(LPSIZE lpSize) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetSize(lpSize);
		}
		return;
	};

	virtual void SetWndLessPos(int x, int y, int cx, int cy, UINT uFlags) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetWndLessPos(x, y, cx, cy, uFlags);
		}
		return;
	};
	virtual void OnMoved(POINT const &ptOld, POINT const &ptNew)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnMoved(ptOld, ptNew);
		}
		return;
	};
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) 
	{
		if (m_wlimp)
		{
			return m_wlimp->OnSized(szOld, szNew);
		}
		return;
	};

	virtual void OnFlashed(bool old_status, bool new_status)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnFlashed(old_status, new_status);
		}
		return;
	};

	virtual BOOL AddChild(HWL hChild) 
	{
		if (m_wlimp)
		{
			return m_wlimp->AddChild(hChild);
		}
		return FALSE;
	};
	virtual BOOL DelChild(HWL hChild) 
	{
		if (m_wlimp)
		{
			return m_wlimp->DelChild(hChild);
		}
		return FALSE;
	};
	virtual HWL GetChild(UINT uID) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetChild(uID);
		}
		return NULL;
	};
	virtual BOOL IsChild(HWL hChild, BOOL bGrandson) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->IsChild(hChild, bGrandson);
		}
		return FALSE;
	};

	virtual HWL GetFirstChildHwl() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetFirstChildHwl();
		}
		return NULL;
	};
	virtual HWL GetLastChildHwl() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetLastChildHwl();
		}
		return NULL;
	};
	virtual void SetFirstChildHwl(HWL hFirstChild) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFirstChildHwl(hFirstChild);
		}
		return;
	};

	virtual HWL GetPrevSiblingHwl() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetPrevSiblingHwl();
		}
		return NULL;
	};
	virtual HWL GetNextSiblingHwl() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetNextSiblingHwl();
		}
		return NULL;
	};
	virtual void SetPrevSiblingHwl(HWL hSibling) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetPrevSiblingHwl(hSibling);
		}
		return;
	};
	virtual void SetNextSiblingHwl(HWL hSibling) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetNextSiblingHwl(hSibling);
		}
		return;
	};

	virtual HWL _InternalHitTest(POINT const &pt) 
	{
		if (m_wlimp)
		{
			return m_wlimp->_InternalHitTest(pt);
		}
		return NULL;
	};
	virtual HWL HitTestChild(POINT const &pt) 
	{
		if (m_wlimp)
		{
			return m_wlimp->HitTestChild(pt);
		}
		return NULL;
	};
	virtual BOOL PtInWndLess(POINT const &pt) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->PtInWndLess(pt);
		}
		return FALSE;
	};

	virtual BOOL SetWindowText(LPCTSTR lpszText) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetWindowText(lpszText);
		}
		return FALSE;
	};
	virtual int GetWindowText(CString &sText) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetWindowText(sText);
		}
		return 0;
	};
	virtual CString & GetWindowTextRef() 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetWindowTextRef();
		}
		static CString nullStr;
		return nullStr;
	};

	virtual void SetWndLessStyle(DWORD dwStyle) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetWndLessStyle(dwStyle);
		}
		return;
	};
	virtual DWORD GetWndLessStyle() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetWndLessStyle();
		}
		return 0;
	};

	virtual HFONT GetFont() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetFont();
		}
		return NULL;
	};
	virtual void SetFont(HFONT font) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFont(font);
		}
		return;
	};

	virtual LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (m_wlimp)
		{
			return m_wlimp->SendMessage(message, wParam, lParam);
		}
		return 0;
	};

	virtual void SetDisplayListener(IWndLess::DisplayListener * listener)
	{
		if (m_wlimp)
		{
			m_wlimp->SetDisplayListener(listener);
		}
		return ;
	};
	virtual void TabFocusToNextChild(HWL hChild, BOOL bPrevious)
	{
		if (m_wlimp)
		{
			return m_wlimp->TabFocusToNextChild(hChild, bPrevious);
		}
	};
	virtual HWL GetNextTabChild(HWL hChild, BOOL bPrevious)
	{
		if (m_wlimp)
		{
			return m_wlimp->GetNextTabChild(hChild, bPrevious);
		}
		return NULL;
	};
	virtual HWL GetTopLevelParent() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetTopLevelParent();
		}
		return NULL;
	};
	virtual BOOL AddTooltip(LPCTSTR lpszTip)
	{
		if (m_wlimp)
		{
			return m_wlimp->AddTooltip(lpszTip);
		}
		return FALSE;
	};
	virtual BOOL UpdateTooltip(LPCTSTR lpszTip)
	{
		if (m_wlimp)
		{
			return m_wlimp->UpdateTooltip(lpszTip);
		}
		return FALSE;
	};
	virtual BOOL ShowTooltip(BOOL bShow) 
	{
		if (m_wlimp)
		{
			return m_wlimp->ShowTooltip(bShow);
		}
		return FALSE;
	};
	virtual BOOL RemoveTooltip()
	{
		if (m_wlimp)
		{
			return m_wlimp->RemoveTooltip();
		}
		return FALSE;
	}
	virtual BOOL SetZorder(HWL hInsertAfter) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetZorder(hInsertAfter);
		}
		return FALSE;
	}
	virtual void UpdateImeCompositionWindowPos(int x, int y) const 
	{
		if (m_wlimp)
		{
			return m_wlimp->UpdateImeCompositionWindowPos(x, y);
		}
	}
};

template <class IT = IWndLessCenter, class IMPT = CWndLessImpBase<CWndLessShell<IWndLessCenter>>> 
class CWndLessCenterShell :public CWndLessShell<IT, IMPT>
{
public:
	CWndLessCenterShell() 
	{
	}
	virtual ~CWndLessCenterShell()
	{

	}
	// With Host
	virtual BOOL CreateEx(HWND hHost)
	{
		if (m_wlimp)
		{
			return m_wlimp->CreateEx(hHost);
		}
		return FALSE;
	}
	virtual HWND GetHost() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetHost();
		}
		return NULL;
	}
	// Manage
	virtual BOOL AddWndLess(HWL w) 
	{
		if (m_wlimp)
		{
			return m_wlimp->AddWndLess(w);
		}
		return FALSE;
	}
	virtual BOOL DelWndLess(HWL w) 
	{
		if (m_wlimp)
		{
			return m_wlimp->DelWndLess(w);
		}
		return FALSE;
	}

	virtual BOOL IsWndLess(HWL w) const
	{
		if (m_wlimp)
		{
			return m_wlimp->IsWndLess(w);
		}
		return FALSE;
	}
	virtual LRESULT SendMessageEx(HWL w, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (m_wlimp)
		{
			return m_wlimp->SendMessageEx(w, msg, wp, lp);
		}
		return FALSE;
	}

	virtual HWL C_SetFocus(HWL w) 
	{
		if (m_wlimp)
		{
			return m_wlimp->C_SetFocus(w);
		}
		return NULL;
	}
	virtual HWL C_GetFocus() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->C_GetFocus();
		}
		return NULL;
	}

	virtual HRESULT C_RegisterDragDrop(HWL hChild, LPDROPTARGET pDropTarget)
	{
		if (m_wlimp)
		{
			return m_wlimp->C_RegisterDragDrop(hChild, pDropTarget);
		}
		return S_FALSE;
	};
	virtual HRESULT C_RevokeDragDrop(HWL hChild)
	{
		if (m_wlimp)
		{
			return m_wlimp->C_RevokeDragDrop(hChild);
		}
		return S_FALSE;
	};

	virtual HWL C_SetCapture(HWL w)
	{
		if (m_wlimp)
		{
			return m_wlimp->C_SetCapture(w);
		}
		return NULL;
	}
	virtual HWL C_GetCapture() const
	{
		if (m_wlimp)
		{
			return m_wlimp->C_GetCapture();
		}
		return NULL;
	}
	virtual HWL C_ReleaseCapture(HWL w)
	{
		if (m_wlimp)
		{
			return m_wlimp->C_ReleaseCapture(w);
		}
		return NULL;
	}

	virtual UINT_PTR C_SetTimer(
		HWL hChild,
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->C_SetTimer(hChild, nIDEvent, nElapse, lpTimerFunc);
		}
		return 0;
	}
	virtual BOOL C_KillTimer(
		HWL hChild,
		UINT nIDEvent
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->C_KillTimer(hChild, nIDEvent);
		}
		return FALSE;
	}
	virtual void C_EnableWindow(
		HWL hChild,
		BOOL bEnable
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->C_EnableWindow(hChild, bEnable);
		}
		return;
	}

	virtual BOOL PreDispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->PreDispatchMessage(message, wParam, lParam, pResult);
		}
		return FALSE;
	}

	virtual BOOL DispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->DispatchMessage(message, wParam, lParam, pResult);
		}
		return FALSE;
	}

	virtual void EnableIme() 
	{
		if (m_wlimp)
		{
			return m_wlimp->EnableIme();
		}
		return;
	}
	virtual void DisableIme()
	{
		if (m_wlimp)
		{
			return m_wlimp->DisableIme();
		}
		return;
	}

	virtual void EnableTooltips(BOOL bEnable) 
	{
		if (m_wlimp)
		{
			return m_wlimp->EnableTooltips(bEnable);
		}
		return;
	}
	virtual BOOL AddTooltipEx(HWL hChild, LPCTSTR lpszTip)
	{
		if (m_wlimp)
		{
			return m_wlimp->AddTooltipEx(hChild, lpszTip);
		}
		return FALSE;
	}
	virtual BOOL UpdateTooltipEx(HWL hChild, LPCTSTR lpszTip)
	{
		if (m_wlimp)
		{
			return m_wlimp->UpdateTooltipEx(hChild, lpszTip);
		}
		return FALSE;
	}
	virtual BOOL ShowTooltipEx(HWL hChild, BOOL bShow)
	{
		if (m_wlimp)
		{
			return m_wlimp->ShowTooltipEx(hChild, bShow);
		}
		return FALSE;
	}
	virtual BOOL RemoveTooltipEx(HWL hChild) 
	{
		if (m_wlimp)
		{
			return m_wlimp->RemoveTooltipEx(hChild);
		}
		return FALSE;
	}

	virtual void OnMinimized(typename IMPT::WNDSIZE enPrev)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnMinimized(enPrev);
		}
		return;
	}
	virtual void OnMaximized(typename IMPT::WNDSIZE enPrev)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnMaximized(enPrev);
		}
		return;
	}
	virtual void OnRestored(typename IMPT::WNDSIZE enPrev)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnRestored(enPrev);
		}
		return;
	}

};


//特化IWndLess的实现模型，因为IWndLess已经是最基础的类型，其的实现不需要依赖父亲

template <> 
class CWndLessImpBase<IWndLess>:  public IWndLess
{
public:
	typedef IWndLess BaseType;

	CWndLessImpBase () : m_wrapper(NULL)
	{
	}

	virtual ~CWndLessImpBase()
	{
	}

	virtual void SetWrapper(IWndLess * wrapper)
	{
		m_wrapper = wrapper;
	}

protected:
	IWndLess * m_wrapper;

};


typedef CWndLessShell< >  CWndLess;

typedef CWndLessCenterShell< > CWndLessRoot;





class WLDC : public CDC
{
public:
	WLDC(HWL hWl)
	{
		m_hWnd = hWl->GetCenter()->GetHost() ;
		Attach(::GetDC(m_hWnd)) ;

		HFONT hFont = hWl->GetFont() ;
		if (hFont == NULL)
			hFont = (HFONT)::SendMessage(hWl->GetCenter()->GetHost(), WM_GETFONT, 0, 0) ;

		if (m_hDC != m_hAttribDC)
			m_hOldObj = ::SelectObject(m_hDC, hFont);
		if (m_hAttribDC != NULL)
			m_hOldObj = ::SelectObject(m_hAttribDC, hFont);
	}
	~WLDC()
	{
		if (m_hDC != m_hAttribDC)
			::SelectObject(m_hDC, m_hOldObj);
		if (m_hAttribDC != NULL)
			::SelectObject(m_hAttribDC, m_hOldObj);

		::ReleaseDC(m_hWnd, Detach()) ;
	}

protected:
	HGDIOBJ m_hOldObj ;
	HWND m_hWnd ;
};

