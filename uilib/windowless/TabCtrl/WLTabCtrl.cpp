// TabCtrlST.cpp : implementation file
//

#include "stdafx.h"
#include "WLTabCtrl.h"
#include "WLTabItem.h"
#include "logex.h"

namespace uilib_global
{
	const int TOOL_TIP_INIT_TIME = 300;			//光标停留 TOOL_TIP_INIT_TIME毫秒之后 显示tip
	const int TOOL_TIP_RESHOW_TIME = 100;			//获取当指针从一个工具移动到另一个工具时，后续工具提示窗口显示所需要的时间长度
	const int TOOL_TIP_AUTOPOP_TIME = 5000;			//光标 一直停留在提示去 tip持续时间 AUTOPOP 遵从MFC的命名
};

template<>
UILIB_EXPORT WLTabCtrlImpBase *CreateDefaultImp< WLTabCtrlImpBase >(IWLTabCtrl * wrapper)

{
	WLTabCtrlImp<> *ret = new WLTabCtrlImp<>(FALSE);
	ret->SetWrapper(wrapper);
	return ret;
};

