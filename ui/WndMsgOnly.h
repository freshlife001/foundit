//////////////////////////////////////////////////////////////////////////
//
//	描述:   只接收窗口消息的窗口基类
//	
//	日期:   2010/09/23
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "uilib/uilib_exports.h"

// CWndMsgOnly

class CWndMsgOnly : public CWnd 
{
	DECLARE_DYNAMIC(CWndMsgOnly)

public:
	CWndMsgOnly();
	virtual ~CWndMsgOnly();

	BOOL Create() ;

// Data
private:
	static CString s_sClassName ;

protected:
	DECLARE_MESSAGE_MAP()

};


