// foundit.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <shellapi.h>
#include <Shlobj.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"


#include "include/core.h"
#include "include/ui/ui.h"
#include "uilib/common/utils/UIUtils.h"

#include "utils/strutils.h"
#include "utils/file_ex.h"
#include "utils/utils.h"
#include "utils/IProfile.h"

#include "netlib/NetEngine.h"
#include "netlib/IInvoke.h"

#include "IEtApi.h"
#include "logex.h"


LOG_IMPL_GLOBAL();

#ifdef _DEBUG
	#include <crtdbg.h>
	class ExitWorker
	{
	public:

		~ExitWorker()
		{
			//_CrtMemDumpAllObjectsSince(&curState);
		}

		_CrtMemState curState;
	};

	ExitWorker woker;

	void SetLeakCheckPoint()
	{
		_CrtMemCheckpoint(&woker.curState);  //save the memory state here
	};
#else
//隐藏控制台
	#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"wmainCRTStartup\"")
#endif


#include "breakpad/exception_handler.h"
#include "breakpad/crash_report_sender.h"
#include "breakpad/minidump_generator.h"


using namespace std;
using namespace google_breakpad;
namespace fs = boost::filesystem;

static __int64 gs_version = 0;
static bool filtercb( void* /*context*/, EXCEPTION_POINTERS* /*exinfo*/, MDRawAssertionInfo* /*assertion*/)
{	
	return true;
}


static bool callback(const wchar_t* /*dump_path*/, const wchar_t *id, void* /*context*/, EXCEPTION_POINTERS* /*exinfo*/, MDRawAssertionInfo* /*assertion*/, bool succeeded) 
{
	std::wstring dmp_id = GetGlobalString(DUMP_ID);
	if (dmp_id.empty())
	{
		dmp_id = _T("unknown");
	}
	std::wstring msnlite_ver = GetGlobalString(CORE_VER);

	wchar_t buf[256];
	swprintf_s(buf, 256, _T("foundit-%d-%d-%d-%d(%s) %s %s"), (WORD)(gs_version >> 48), (WORD)(gs_version >> 32), (WORD)(gs_version >> 16), (WORD)(gs_version),msnlite_ver.c_str(), dmp_id.c_str(), id);
	

	int iRes = (int)ShellExecute(NULL, L"open", L"ErrorReport.exe", buf, NULL, SW_HIDE); //启动错误报告程序
	if (iRes <= 32)
	{
		ShellExecute(NULL, L"open", L"..\\..\\ErrorReport.exe", buf, NULL, SW_HIDE); //启动错误报告程序
	}

	return succeeded;   
}


static int kCustomInfoCount = 2;
static CustomInfoEntry kCustomInfoEntries[] = {
	CustomInfoEntry(L"prod", L"Foundit"),
	CustomInfoEntry(L"ver", L"1.0"),
};

ExceptionHandler *g_phandle = NULL;

bool Init()
{

	//gs_version = LibIM::GetMyVersion();

	CustomClientInfo custom_info = {kCustomInfoEntries, kCustomInfoCount};
	TCHAR tcTmpPath[MAX_PATH] = { 0 };
	GetTempPath( MAX_PATH, tcTmpPath);
	g_phandle = new ExceptionHandler( 
		tcTmpPath, 
		filtercb, 
		callback, 
		NULL, 
		ExceptionHandler::HANDLER_ALL, 
		MiniDumpNormal,
		TEXT( "\\\\.\\pipe\\FoundItBreakPadFromGoogleCrashService\\DumpServer"),
		&custom_info
		);  


	//设置当前目录为exe目录
	TCHAR tcFilePath[MAX_PATH*2] = { 0 };
	GetModuleFileName( NULL, tcFilePath, MAX_PATH*2);
	fs::wpath path(tcFilePath);
	try
	{
		fs::current_path(path.parent_path());
	}
	catch(boost::system::system_error &e)
	{
		(e);
		SetCurrentDirectory(path.parent_path().wstring().c_str());
	}


	LOG_FUNC("foundit version " << (WORD)(gs_version >> 48) << "-" << (WORD)(gs_version >> 32) << "-" << (WORD)(gs_version >> 16) << "-" << (WORD)(gs_version));



	//try use absolute path
	std::wstring local_dbstore_path = path.parent_path().wstring() + L"\\Config";
	std::wstring appdata_dbstore_path;
	//force to use appdata to store db, due to win7 virtual store!!
	{
		TCHAR szAppData[MAX_PATH];
		HRESULT ret = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
		if (S_OK == ret)
		{
			appdata_dbstore_path = szAppData;
			appdata_dbstore_path.append(L"\\FoundIt\\Config");
		}
		else
		{
			if(hdutils::GetWindowsDir(CSIDL_APPDATA, appdata_dbstore_path))
			{
				appdata_dbstore_path.append(L"\\FoundIt\\Config");
			}
		}
		LOG_INFO("appdata_dbstore_path path: " << appdata_dbstore_path );
	}

	if (file_ex::exists_w(appdata_dbstore_path + L"\\global.dat"))
	{
		//优先选择appdata
		SetDBStorePath(appdata_dbstore_path);
		LOG_INFO("Using Appdata DBStorePath: " << appdata_dbstore_path);
	}
	else if (file_ex::exists_w(local_dbstore_path + L"\\global.dat"))
	{
		//其次选择本地
		SetDBStorePath(local_dbstore_path);
		LOG_INFO("Using Local DBStorePath: " << local_dbstore_path);
	}
	else
	{
		std::wstring dbstore_path = appdata_dbstore_path;
		if (dbstore_path.empty())
		{
			dbstore_path = local_dbstore_path;
		}
		SetDBStorePath(dbstore_path);
		
		LOG_INFO("Using DBStorePath: " << dbstore_path);
	}

	std::wstring dbstore_path = GetDBStorePath();
	try
	{
		boost::filesystem::create_directories(dbstore_path);
	}
	catch(boost::system::system_error &e)
	{
		e;
		LOG_ERROR(e.code() << e.what());
	}

	return true;
}

std::wstring ParsePipe(int argc, _TCHAR* argv[])
{
	for (int i = 0; i<argc; i++)
	{
		std::wstring arg = argv[i];
		if (arg == L"-p")
		{
			i++;
			if (i < argc)
			{
				std::wstring pipe_name = argv[i];
				return pipe_name;
			}
		}
	}

	return L"";

}

extern void UnitTestGo();
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	SetLeakCheckPoint();
#endif
	LOG_INIT();
	LOG_FUNC_SCOPE();


	HANDLE hSemaphore = CreateSemaphore(NULL, 1, 1000, L"FOUND_IT");

	DWORD errorCode = GetLastError();
	if (ERROR_ALREADY_EXISTS == errorCode)
	{
		LONG preViousCount;
		BOOL ret = ReleaseSemaphore(hSemaphore, 1, &preViousCount);
		if (ret)
		{
		}
	}

	Init();

	std::wstring pipe = ParsePipe(argc, argv);

	NetEngine * engin = GetCoreNetEngine();
	IInvoke* invoker = GetCoreInvoker();

	IEtApi * etapi = GetCoreEtApi();

	//提前先初始化界面

	etapi->Init();
	

	UIInit();


#ifdef GTEST
	UnitTestGo();
#endif

	

	RunMsgLoop();


	delete etapi;
	etapi = NULL;

	
	ShutDown();

	if (hSemaphore)
	{
		WaitForSingleObject(hSemaphore, 1000);
	}
	

	
	return 0;
}





