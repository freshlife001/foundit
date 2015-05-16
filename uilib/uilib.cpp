// uilib.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "uilib.h"

#include "include/uilib/common/utils/UIUtils.h"
#include "include/uilib/common/utils/ImageEx/Shared_ImageEx.h"
#include "include/uilib/common/utils/ImageEx/ImageEx.h"
#include "common/utils/Image2/TimerTaskPump.h"

#include "windowless/BaseWndlessWnd.h"
#include "windowless/ButtonAL/WLButtonALHelper.h"
#include "logex.h"

#if defined(_WIN32) && defined(_WINDLL)
LOG_IMPL_GLOBAL();
#endif



ULONG_PTR gdiplusToken;
bool InitUILib()
{
#ifdef _USRDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

 	GdiplusStartupInput gdiplusStartupInput;
	if (Ok == GdiplusStartup(&gdiplusToken,&gdiplusStartupInput,NULL))
	{
		TimerTaskPump::Initialize() ;
		WLButtonALHelper::Initialize();
		return true;
	}

	return false;
}
void UnInitUILib()
{
#ifdef _USRDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	
	ImageEx::ExitGifManager();
	WLButtonALHelper::UnInitialize() ;
	TimerTaskPump::Uninitialize() ;

	GdiplusShutdown(gdiplusToken);
}



class CUIlibApp : public CWinApp
{
public:
	CUIlibApp();

	// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CUIlibApp, CWinApp)
END_MESSAGE_MAP()


// CuiApp 构造

CUIlibApp::CUIlibApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

}

// 唯一的一个 CuiApp 对象

CUIlibApp theApp;

// CuiApp 初始化

BOOL CUIlibApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


