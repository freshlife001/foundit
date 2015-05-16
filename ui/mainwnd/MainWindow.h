#pragma once
#include <boost/shared_ptr.hpp>
#include "WndMsgOnly.h"

#define MSG_EXE_FULL_SCREEN		WM_USER + 300

class CSearchWndContainer;

class CMainWindow :
	public CWndMsgOnly
{
	DECLARE_DYNAMIC(CMainWindow)
public:
	CMainWindow(void);
	virtual ~CMainWindow(void);

	void ShowAppCenterWnd(BOOL bShow = TRUE);
	void ShowAppNotificationBar(BOOL bShow = TRUE);
	
	CSearchWndContainer * GetNewSearchWndContainer( bool activate, bool bNeedLayout );
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	void InitFullScreenChecker();
	BOOL IsValidFullscreenProc();
	LRESULT OnCheckFullScreen(WPARAM wParam, LPARAM lParam);

private:
	void RelayoutContainer( CSearchWndContainer* pContainer );

private:
	HICON m_hIcon;
	std::vector<CSearchWndContainer*> m_containers;

};
