#pragma once
#include <vector>
#include "boost/shared_ptr.hpp"
#include "uilib/wndless/IWndLess.h"
#include "uilib/wndless/IWLScrollBar.h"
#include "uilib/wndless/IWLButtonAL.h"
#include "uilib/wndless/IWLStatic.h"
#include "uilib/wndless/IWLProgress.h"
#include "SearchResultList.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"
#include <vector>

namespace boost
{
	namespace signals2
	{
		class connection;
	}
}

class CSearchHelper;
struct _FileInfo;

class CSearchResultWnd : public CWndLess, public IWLSimpleList::EventListener
{
public:
	CSearchResultWnd(void);
	virtual ~CSearchResultWnd(void);
	enum
	{
		IDC_LIST_SEARCH_RESULT = 20,
		IDC_BUTTON_BACK = 21,
		IDC_BUTTON_REFRESH = 22,
		IDC_BUTTON_SORT_BYNAME = 30,
		IDC_BUTTON_SORT_BYSIZE = 31,
		IDC_BUTTON_SORT_BYDATE = 32,
	};

	
public:
	//IWLSimpleList::EventListener
	virtual void OnListItemSelectChanged(IWLSimpleList *, IWLSimpleListItem * /*item*/, bool /*gain_or_lose*/);
	virtual void OnListLeftClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListLeftDoubleClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListRightClick(IWLSimpleList *, IWLSimpleListItem *);
	virtual void OnListRightDoubleClick(IWLSimpleList *, IWLSimpleListItem *);



	void DoSearch(LPCTSTR text);

protected:
	virtual BOOL OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
private:
	BOOL OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	BOOL OnNotify(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult);

	void OnSearchResult(const std::wstring &key, unsigned int total, unsigned int offset, const std::vector<std::wstring>& fullPaths);

	void OnSearchError(unsigned int errorCode);

	void OnFileInfoLoaded(_FileInfo &);


	CSearchResultList m_searchResult;

	boost::shared_ptr<CSearchHelper> m_searchHelper;

	CString m_key;

	std::vector<boost::signals2::connection> m_conns;

	std::wstring m_currKey;
	unsigned int m_totalResultCount;

	std::set<unsigned int> m_offsetRequested;
};
