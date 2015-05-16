// test.cpp : 定义控制台应用程序的入口点。

//

#include "stdafx.h"
#include <string>
#include <windows.h>
#include <Sddl.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/shared_ptr.hpp>

#include "netlib/NetEngine.h"
#include "netlib/IInvoke.h"
#include "utils/strutils.h"
#include "utils/file_ex.h"
#include "utils/IProfile.h"

#include "core.h"
#include "logex.h"


bool spawn(std::wstring& path)
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hToken = NULL, hTokenNew = NULL;
	PSID plntegritySicl = NULL;
	TOKEN_MANDATORY_LABEL tml = {0};
	PROCESS_INFORMATION procInfo = {0};
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb= sizeof(StartupInfo);

	std::wstring cmd_str =  path;
	
	LPWSTR szCmd = _tcsdup(cmd_str.c_str());
	if (!ImpersonateSelf(SecurityImpersonation)) 
	{
		return false ;
	}
	// 指定低权限：
	if (!ConvertStringSidToSid(SDDL_ML_HIGH, &plntegritySicl)) 
	{
		return false;
	}

	bool result = false;

	BOOL bWait = TRUE;
	if (OpenProcessToken(hProcess, MAXIMUM_ALLOWED, &hToken)) 
	{
		if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hTokenNew)) 
		{
			tml.Label.Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED;
			tml.Label.Sid = plntegritySicl;
			if (SetTokenInformation(hTokenNew, TokenIntegrityLevel, &tml, sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(plntegritySicl))) 
			{
				if (CreateProcessAsUser(hTokenNew, NULL, szCmd, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &procInfo)) 
				{
					printf("CreateProcessAsUser succeed!");
				    result = true;
					if (bWait) 
					{
						WaitForSingleObject(procInfo.hProcess, 1 * 1000);
					}
				} else {
						printf("error :%d", GetLastError() );
	
				}
				
			}
			CloseHandle(hTokenNew);
		}
		CloseHandle(hToken);
	}
	return result;
} 






#if defined(_WIN32) && defined(_WINDLL)
LOG_IMPL_GLOBAL();
#endif

boost::shared_ptr<NetEngine> g_netEngine;
IInvoke *g_invoker = NULL;


NetEngine *GetCoreNetEngine()
{
	if (g_netEngine==NULL)
	{
		g_netEngine = CreateASIONetEngine();
		g_netEngine->start();
	}
	return g_netEngine.get();
}


IInvoke *GetCoreInvoker()
{
	if (g_invoker == NULL)
	{
		if (g_netEngine)
		{
			g_invoker = dynamic_cast<IInvoke*>(g_netEngine.get());
		}
	}
	return g_invoker;
}


void ShutDown()
{
	if (g_netEngine)
	{
		g_netEngine->stop();
		if (g_invoker==dynamic_cast<IInvoke*>(g_netEngine.get()))
		{
			static DummyInvoke s_dummyInvoker;
			g_invoker = &s_dummyInvoker;
			g_netEngine.reset(new DummyNetEngine);
		}
		
	}
}




std::wstring & GetGlobalString(GlobalStringType type)
{
	static std::vector<std::wstring> s_gStrings;
	if (s_gStrings.empty())
	{
		s_gStrings.resize(MAX_STRING_SIZE);
	}
	return s_gStrings[(unsigned int)type];
}
