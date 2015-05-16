//////////////////////////////////////////////////////////////////////////
//
//	描述:   顶层窗口，实现了无句柄窗口中心
//			一般要作为有句柄窗口的成员变量
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
#include "WndLess.h"
#include <vector>
#include <map>
#include <set>
#pragma comment(lib, "Imm32")
#include <Imm.h>

#include "timerid.h"

class RootDropTarget ;


typedef CWndLessImpBase< CWndLessShell<IWndLessCenter> >  WndLessCenterImpBase;
class CWndLessRootImp : public WndLessCenterImpBase
{
public:
	CWndLessRootImp();
	virtual ~CWndLessRootImp(void);

	//////////////////////////////////////////////////////////////////////////
	// IWndLessCenter Implements
	// With Host
	virtual BOOL CreateEx(HWND hHost) ;
	virtual HWND GetHost() const ;

	// Manage
	virtual BOOL AddWndLess(HWL) ;
	virtual BOOL DelWndLess(HWL) ;

	virtual BOOL IsWndLess(HWL) const ;
	virtual LRESULT SendMessageEx(HWL, UINT, WPARAM, LPARAM) ;

	virtual HWL C_SetFocus(HWL) ;
	virtual HWL C_GetFocus() const ;

	virtual HRESULT C_RegisterDragDrop(HWL hChild, LPDROPTARGET pDropTarget) ;
	virtual HRESULT C_RevokeDragDrop(HWL hChild) ;

	virtual HWL C_SetCapture(HWL) ;
	virtual HWL C_GetCapture() const ;
	virtual HWL C_ReleaseCapture(HWL) ;

	virtual UINT_PTR C_SetTimer(
		HWL hChild,
		UINT_PTR nIDEvent,
		UINT nElapse,
		TIMERPROC lpTimerFunc
		) ;
	virtual BOOL C_KillTimer(
		HWL hChild,
		UINT nIDEvent
		) ;

	virtual void C_EnableWindow(
		HWL hChild,
		BOOL bEnable
		) ;

	virtual BOOL PreDispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) ;

	virtual BOOL DispatchMessage(
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam, 
		LRESULT * pResult
		) ;

	virtual void EnableIme() ;
	virtual void DisableIme() ;

	virtual void EnableTooltips(BOOL bEnable) ;
	virtual BOOL AddTooltipEx(HWL hChild, LPCTSTR lpszTip) ;
	virtual BOOL UpdateTooltipEx(HWL hChild, LPCTSTR lpszTip) ;
	virtual BOOL ShowTooltipEx(HWL hChild, BOOL bShow) ;
	virtual BOOL RemoveTooltipEx(HWL hChild) ;

	IDropTarget * GetChildDropTarget(HWL hChild) const ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implements
	// Init
	BOOL Create(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent,
		UINT uID
		) ;
	virtual IWndLessCenter * GetCenter() const ;

	// Operation
	virtual void _InternalOnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual void GetRectInScreen(LPRECT lpRect) const ;			// 获得客户区在屏幕坐标系中的位置
	virtual void GetRectInHost(LPRECT lpRect) const ;			// 获得客户区在宿主坐标系中的位置

	// Message Handler
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

	//////////////////////////////////////////////////////////////////////////
	// Root Implements

	virtual void OnMinimized(WNDSIZE enPrev) ;
	virtual void OnMaximized(WNDSIZE enPrev) ;
	virtual void OnRestored(WNDSIZE enPrev) ;

protected:
	BOOL PollMessage(HWL hChild, UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	BOOL ProcessTooltipNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) ;
	void TrackTooltip(HWL hChild, BOOL bEnter, UINT message, WPARAM wParam, LPARAM lParam) ;


protected:
	// IWndLessCenter Data
	HWND m_hHost ;
	HIMC m_hImc ;
	HWND m_hTooltip ;

	struct TOOLTIPINFO
	{
		TOOLTIPINFO() : type(0), show(TRUE) {}

		int type ;
		BOOL show ;
		CString tip_text ;
	};
	typedef std::map<HWL, TOOLTIPINFO>	MAP_HWL2STRING ;
	MAP_HWL2STRING m_mapChildTooltips ;

	HWL m_hFocusChild ;
	HWL m_hCaptureChild ;
	HWL m_hHotChild ;
	BOOL m_bIsTrackMouse ;

	//std::vector<HWL> m_vctFocusChild ;

	std::set<HWL> m_setChildTree ;
	WNDSIZE m_enWndSize ;

	TimerIdAlloter m_timerid_alloter ;
	std::map<UINT_PTR, WndLessTimerId> m_timerid_map_win2wndless ;
	std::map<WndLessTimerId, UINT_PTR> m_timerid_map_wndless2win ;

	BOOL m_register_drag_drop ;
	std::map<HWL, IDropTarget*> m_wl2droptarget ;

	RootDropTarget * m_drop_target ;
};


class RootDropTarget : public IDropTarget
{
public:
	RootDropTarget(CWndLessRootImp * root) ;

public:
	//////////////////////////////////////////////////////////////////////////
	// IUnknown methods

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) ;

	virtual ULONG STDMETHODCALLTYPE AddRef( void) ;

	virtual ULONG STDMETHODCALLTYPE Release( void) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IDropTarget methods

	virtual HRESULT STDMETHODCALLTYPE DragEnter( 
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect) ;

	virtual HRESULT STDMETHODCALLTYPE DragOver( 
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect) ;

	virtual HRESULT STDMETHODCALLTYPE DragLeave( void) ;

	virtual HRESULT STDMETHODCALLTYPE Drop( 
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect) ;

protected:
	ULONG m_ref_count ;
	CWndLessRootImp * m_root ;
	IDropTarget * m_curr_drop_target ;
	IDataObject * m_curr_drag_data_object ;

};

