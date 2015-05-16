// WndMsgOnly.cpp : 实现文件
//

#include "stdafx.h"
#include "WndMsgOnly.h"


// CWndMsgOnly

IMPLEMENT_DYNAMIC(CWndMsgOnly, CWnd)

CString CWndMsgOnly::s_sClassName = _T("") ;

CWndMsgOnly::CWndMsgOnly()
{
	if (s_sClassName.IsEmpty())
	{
		s_sClassName = AfxRegisterWndClass(0) ;
	}
}

CWndMsgOnly::~CWndMsgOnly()
{
}

BOOL CWndMsgOnly::Create() 
{
	return CreateEx(0, s_sClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0) ;
}

BEGIN_MESSAGE_MAP(CWndMsgOnly, CWnd)
END_MESSAGE_MAP()



// CWndMsgOnly 消息处理程序

