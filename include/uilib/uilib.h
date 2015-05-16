// uilib.h : uilib DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif


#include "uilib_exports.h"

#define ST_HDBASEWINDOW	_T("St.HDBaseWindow")


UILIB_EXPORT bool InitUILib();
UILIB_EXPORT void UnInitUILib();


