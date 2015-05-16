#pragma once

#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLScrollBar.h"
#include "uilib/wndless/IWLButtonAL.h"
#include "uilib/wndless/IWLStatic.h"
#include "uilib/wndless/IWLProgress.h"
#include "SearchResultList.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"

class COverViewWnd :
	public CWndLess
{
public:
	COverViewWnd(void);
	virtual ~COverViewWnd(void);
};
