#pragma once
#include "../wledit.h"

class WLCmbEditImp :
	public WLEditImp
{
public:
	WLCmbEditImp(void);
	virtual ~WLCmbEditImp(void);

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

};


typedef CWLEditShell<IWLEdit, WLCmbEditImp> WLCmbEdit;