#pragma once
#include "afxwin.h"

class CSearchWndContainerRoot;
class CSearchWndContainer :
	public CWnd
{
public:
	CSearchWndContainer(void);
	virtual ~CSearchWndContainer(void);

	virtual BOOL Create( CWnd* pParent ); 

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg) ;


private:
	CSearchWndContainerRoot *m_pRoot;
};
