#pragma once
#include "BaseWndlessWnd.h"
#include "BaseWndLessRoot.h"
#include "DHtmlWnd.h"
#include "wndless/IWLAnimation.h"
#include "include/IAppMgr.h"

//////////////////////////////////////////////////////////////////////////
// WebAppShowWnd

class WebAppShowWnd :
	public BaseWndlessWnd
{
	DECLARE_DYNAMIC(WebAppShowWnd)

public:
	WebAppShowWnd(void);
	virtual ~WebAppShowWnd(void);

public:
	BOOL CreateWnd(HWND hParent, unsigned int app_id, const IAppMgr::WebAppInfo& info);
};

//////////////////////////////////////////////////////////////////////////
// WLWebAppShowWnd

#define WM_WEB_APP_WND_CLOSE	WM_USER + 200

class WLWebAppShowWnd :
	public BaseWndLessRoot
{
public:
	WLWebAppShowWnd(unsigned int app_id, const IAppMgr::WebAppInfo& info);
	virtual ~WLWebAppShowWnd(void);
	
protected:
	virtual BOOL OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	virtual void OnDestroy();

protected:
	void OnDocumentComplete();

private:
	hdui::CDHtmlWnd m_wndWebApp;
	WLAnimation m_wlLoadingBkgnd;
	WLAnimation m_wlLoadingAnimation;

	int m_AppID;
	IAppMgr::WebAppInfo m_WebAppInfo;
};
