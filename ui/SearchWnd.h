#pragma once
#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLScrollBar.h"
#include "uilib/wndless/IWLButtonAL.h"
#include "uilib/wndless/IWLStatic.h"
#include "uilib/wndless/IWLProgress.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"


#include "SearchBar.h"
#include "SearchResultWnd.h"
#include "OverViewWnd.h"


class CSearchWnd : public CWndLess
{
public:
	CSearchWnd(void);
	virtual ~CSearchWnd(void);
	enum
	{
		IDC_SEARCH_BAR = 10,
		IDC_SEARCH_RESULT = 20,
		IDC_OVER_VIEW = 30,
	};

protected:
	virtual BOOL OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
private:
	BOOL OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	BOOL OnNotify(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult);

	COverViewWnd m_overViewWnd;
	CSearchBar m_searchBar;
	CSearchResultWnd m_resultWnd;
};
