#pragma once
#include "..\windowless\wledit.h"

#define EN_ENDEDIT		0x0800
#define EN_CANCELEDIT	0x0801

class WLTempEdit :
	public WLEdit
{
public:
	WLTempEdit(void);
	virtual ~WLTempEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	void SetOwner(HWL hOwner) { m_hOwner = hOwner ; }
	void BeginEdit() ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

protected:
	HWL m_hOwner ;
	BOOL m_editing ;		// 标记当前是否在编辑中

};
