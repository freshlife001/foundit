// MainWindow.cpp : ÊµÏÖÎÄ¼þ
//

#include "stdafx.h"
#include <psapi.h>

#include "include/IEtApi.h"
#include "include/core.h"
#include "utils/IProfile.h"

#include "uilib/uilib.h"
#include "uilib/wndless/IWndLess.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"

#include "FileInfoLoader.h"
#include "SignalHandler.h"
#include "MainWindow.h"
#include "SearchWndContainer.h"
#include "Resource.h"
#include "logex.h"




// CMainWindow

IMPLEMENT_DYNAMIC(CMainWindow, CWndMsgOnly)

CMainWindow::CMainWindow(void)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMainWindow::~CMainWindow(void)
{
}


BEGIN_MESSAGE_MAP(CMainWindow, CWndMsgOnly)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(MSG_EXE_FULL_SCREEN, &CMainWindow::OnCheckFullScreen)
END_MESSAGE_MAP()


class DummyListener : public IEtApi::SearchListener
{
		public:
			virtual void HandleResult(const std::wstring &key, unsigned int total, unsigned int offset, const std::vector<std::wstring>& fullPaths) 
			{
			};

			virtual void HandleError(unsigned int errorCode) 
			{
			};
};

int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	this->SetIcon(m_hIcon, TRUE);
	this->SetIcon(m_hIcon, FALSE);

	InitFullScreenChecker();

	CFileInfoLoader::Instance().Init();

	//test
	ShowAppNotificationBar();
	ShowAppCenterWnd();


	IEtApi::SearchListenerPtr lptr;
	lptr.reset(new DummyListener);
	GetCoreEtApi()->Search(L"msnlite-setup-3.0.0.4045.exe",0, lptr );

	return TRUE;
}

void CMainWindow::OnDestroy()
{

	CFileInfoLoader::Instance().UnInit();

	CWndMsgOnly::OnDestroy();
}

void CMainWindow::ShowAppCenterWnd(BOOL bShow/* = TRUE*/)
{
	GetNewSearchWndContainer(true, true);

}

void CMainWindow::ShowAppNotificationBar(BOOL bShow/* = TRUE*/)
{

}


void CMainWindow::InitFullScreenChecker()
{
	APPBARDATA abd;
	memset(&abd, 0, sizeof(abd));
	abd.cbSize = sizeof(APPBARDATA);   
	abd.hWnd = this->GetSafeHwnd();
	abd.uCallbackMessage = MSG_EXE_FULL_SCREEN;
	::SHAppBarMessage(ABM_NEW, &abd);
}

BOOL CMainWindow::IsValidFullscreenProc()
{
	DWORD dwProcessId = 0;
	CWnd* pCornerWnd = WindowFromPoint( CPoint( 0, 0 ));
	GetWindowThreadProcessId( pCornerWnd->GetSafeHwnd(), &dwProcessId );
	LOG_TRACE( L"FullScreen_APP: processId " << dwProcessId );

	int curProcessId = GetCurrentProcessId();
	if ( dwProcessId == curProcessId )
	{
		return FALSE;
	}
	else
	{
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId );
		TCHAR chProcessName[MAX_PATH];
		chProcessName[MAX_PATH-1] = L'\0';
		if ( hProcess )
		{
			HMODULE hModule;
			DWORD dwCbNeeded;
			if ( EnumProcessModules( hProcess, &hModule, sizeof( hModule ), &dwCbNeeded ))
			{
				GetModuleBaseName( hProcess, hModule, chProcessName, MAX_PATH );
				chProcessName[MAX_PATH-1] = L'\0';
				LOG_TRACE(L"FullScreen_APP: processName" << chProcessName );
			}
		}
		else
		{
			CloseHandle( hProcess );
			return TRUE;
		}
		CloseHandle( hProcess );
		if ( _tcsicmp( chProcessName, L"Explorer.exe") == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

LRESULT CMainWindow::OnCheckFullScreen(WPARAM wParam, LPARAM lParam)
{
	static BOOL bFullScreen = FALSE;

	if((UINT)wParam == ABN_FULLSCREENAPP)
	{
		if (bFullScreen != static_cast<BOOL>(lParam))
		{
			BOOL bValidProc = TRUE;
			if (lParam)
			{
				bValidProc = IsValidFullscreenProc();
			}
			if (bValidProc)
			{
				if (lParam)
				{
					bFullScreen = TRUE;
				}
				else
				{
					bFullScreen = FALSE;
				}
			}
		}
	}

	return 0;
}


CSearchWndContainer * CMainWindow::GetNewSearchWndContainer( bool activate, bool bNeedLayout )
{
	CSearchWndContainer *pContainer = new CSearchWndContainer();

	if(!::IsWindow( pContainer->GetSafeHwnd() ))
	{
		pContainer->Create( GetDesktopWindow() );
	}
	if ( m_containers.end() == std::find( m_containers.begin(), m_containers.end(), pContainer ))
	{
		if ( bNeedLayout )
		{
			RelayoutContainer( pContainer );
		}
		m_containers.push_back(pContainer);
		if (!activate)
		{
			pContainer->ShowWindow(SW_SHOWMINNOACTIVE);
		}
		else
		{
			pContainer->ShowWindow(SW_SHOWNORMAL);
		}
	}
	return pContainer;
}


void CMainWindow::RelayoutContainer( CSearchWndContainer* pContainer )
{
	ASSERT( pContainer );
	CRect rcWorkArea ;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) ;
	//const int xMaxScreen = GetSystemMetrics( SM_CXSCREEN ), yMaxScreen = GetSystemMetrics( SM_CYSCREEN );
	WINDOWPLACEMENT winPlacement;
	winPlacement.length = sizeof( WINDOWPLACEMENT);
	const RECT& rctDlg = winPlacement.rcNormalPosition;

	CSearchWndContainer* pLastContainer = NULL;
	std::vector<CSearchWndContainer*>::reverse_iterator itContainer = m_containers.rbegin();
	for ( ; itContainer != m_containers.rend(); ++itContainer )
	{
		if ( *itContainer && ::IsWindow( (*itContainer)->GetSafeHwnd()))
		{
			pLastContainer = *itContainer;
			break;
		}
	}
	if ( pLastContainer )
	{
		pLastContainer->GetWindowPlacement( &winPlacement );
		LONG chatWidth = rctDlg.right - rctDlg.left, chatHeight = rctDlg.bottom - rctDlg.top, leftMoveTo = rctDlg.left + 30, topMoveTo = rctDlg.top + 30;

		if ( leftMoveTo <= rcWorkArea.left + 30 || leftMoveTo >= rcWorkArea.right - chatWidth - 30 || topMoveTo <= rcWorkArea.left + 30 || topMoveTo >= rcWorkArea.bottom - chatHeight - 30 )
		{
			leftMoveTo = rcWorkArea.left + 30;
			topMoveTo = rcWorkArea.top + 30;
		}
		pContainer->SetWindowPos( NULL, leftMoveTo + 15, topMoveTo + 15, NULL, NULL, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE );
	}
	else
	{
		pContainer->GetWindowPlacement( &winPlacement);
		const int chatWidth = rctDlg.right - rctDlg.left, chatHeight = rctDlg.bottom - rctDlg.top;
		pContainer->SetWindowPos( NULL,  rcWorkArea.left + ( rcWorkArea.Width() - chatWidth) / 2 , rcWorkArea.top + ( rcWorkArea.Height() - chatHeight) / 2,
			NULL, NULL, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}
}
