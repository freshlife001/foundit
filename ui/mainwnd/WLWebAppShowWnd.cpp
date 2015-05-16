#include "StdAfx.h"
#include "WLWebAppShowWnd.h"

//////////////////////////////////////////////////////////////////////////
// WebAppShowWnd

IMPLEMENT_DYNAMIC(WebAppShowWnd, BaseWndlessWnd)

WebAppShowWnd::WebAppShowWnd(void)
{
}

WebAppShowWnd::~WebAppShowWnd(void)
{
}

BOOL WebAppShowWnd::CreateWnd(HWND hParent, unsigned int app_id, const IAppMgr::WebAppInfo& info)
{
	WLWebAppShowWnd* wlWnd = new WLWebAppShowWnd(app_id, info);
	SetWndLessRoot(wlWnd);
	int title_height = wlWnd->GetTitleHeight();

	return BaseWndlessWnd::Create(
		WS_EX_APPWINDOW | WS_EX_ACCEPTFILES,
		WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX,
		_T("WebÓ¦ÓÃ"),
		0, 0, info.wnd_width + 20, info.wnd_hight + title_height + 20,
		hParent,
		NULL);
}

//////////////////////////////////////////////////////////////////////////
// WLWebAppShowWnd

WLWebAppShowWnd::WLWebAppShowWnd(unsigned int app_id, const IAppMgr::WebAppInfo& info)
	:m_AppID(app_id), m_WebAppInfo(info)
{
	m_wndWebApp.ShowScroll(info.enable_scroll);
}

WLWebAppShowWnd::~WLWebAppShowWnd(void)
{
}

BOOL WLWebAppShowWnd::OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent)
{
	if(!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
		return FALSE;

	SetWindowText(m_WebAppInfo.title.c_str());

	int title_height = this->GetTitleHeight();
	CRect rcWnd(10, title_height + 10, cx - 10, cy - 10);
	m_wndWebApp.Create(0, WS_CHILD, _T("WebApp"), rcWnd.left, rcWnd.top, rcWnd.Width()
		, rcWnd.Height(), GetHost(), NULL);
	m_wndWebApp.Navigate(m_WebAppInfo.url.c_str());

	m_wlLoadingBkgnd.Create(WS_VISIBLE, rcWnd.left, rcWnd.top, rcWnd.Width()
		, rcWnd.Height(), this, -1);
	m_wlLoadingBkgnd.Open(m_WebAppInfo.bkgnd_path.c_str());

	CPoint ptCenter = rcWnd.CenterPoint();
	if (!m_wlLoadingAnimation.Create(WS_VISIBLE | WLAnimation::AS_AUTOFITIMAGE | WLAnimation::AS_CMBD
		, ptCenter.x - 17, ptCenter.y - 17, 35, 35, this, -1))
		return FALSE;
	m_wlLoadingAnimation.Open(IDI_LOGWND_LOGGING_PNG, 17, 1);
	m_wlLoadingAnimation.Play();

	return TRUE;
}

BOOL WLWebAppShowWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	if(__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE;

	BOOL bRes = TRUE;
	switch(message)
	{
	case WM_DOCUMENT_COMPLETE:
		OnDocumentComplete();
		break;
	default:
		bRes= FALSE;
		break;
	}

	return bRes;
}

void WLWebAppShowWnd::OnDestroy()
{
	::SendMessage(::GetParent(GetHost()), WM_WEB_APP_WND_CLOSE, m_AppID, NULL);

	__super::OnDestroy();
}

void WLWebAppShowWnd::OnDocumentComplete()
{
	m_wlLoadingBkgnd.SetWndLessStyle(0);
	m_wlLoadingAnimation.Stop();
	m_wlLoadingAnimation.SetWndLessStyle(0);
	UpdateView();

	m_wndWebApp.ShowWindow(SW_SHOW);
}