#pragma once
#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLButtonAL.h"
#include "uilib/wndless/IWLEdit.h"
#include "uilib/wndless/IWLStatic.h"

#include "wlctrls/WLSearchInput.h"
#include "SearchHistoryList.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"

#define SBN_SUBMIT_SEARCH (NM_WLFIRST - 306U)

class CSearchBar :
	public CWndLess, public IWLSimpleList::EventListener
{
public:
	CSearchBar(void);
	virtual ~CSearchBar(void);

	enum
	{
		IDC_SEARCH_INPUT = 10,
		IDC_LIST_HISTORY = 11,
		IDC_BUTTON_HOME = 12,
	};

	//IWLSimpleList::EventListener
public:
	virtual void OnListItemSelectChanged(IWLSimpleList *, IWLSimpleListItem * /*item*/, bool /*gain_or_lose*/);
	virtual void OnListLeftClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListLeftDoubleClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListRightClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListRightDoubleClick(IWLSimpleList *, IWLSimpleListItem *);

protected:
	virtual BOOL OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	void Submit(const CString & text);

	void HandleArrow(bool upOrDown);



	WLSearchInput m_input;
	WLButtonAL m_btnHome;
	CSearchHistoryList m_history;

};
