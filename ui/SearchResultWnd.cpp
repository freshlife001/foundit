#include "StdAfx.h"
#include "boost/signals2/signal.hpp"
#include "boost/foreach.hpp"
#include "include/IEtApi.h"
#include "include/core.h"

#include "uilib/common/utils/UIUtils.h"
#include "SignalHandler.h"
#include "FileInfoLoader.h"
#include "SearchResultWnd.h"

class CSearchHelper : public CSignalHandler, public IEtApi::SearchListener
{
public:
	//fullPaths : dir ends with /
	void HandleResult(const std::wstring &key, unsigned int total, unsigned int offset, const std::vector<std::wstring>& fullPaths)
	{
		GotResult(key, total, offset, fullPaths);
	}

	void HandleError(unsigned int errorCode)
	{
		GotError(errorCode);
	}

//events
public:
	boost::signals2::signal<void (const std::wstring &, unsigned int , unsigned int , const std::vector<std::wstring>&)> GotResult;
	boost::signals2::signal<void (unsigned int)> GotError;



};

CSearchResultWnd::CSearchResultWnd(void) : m_totalResultCount(0)
{
	m_searchHelper.reset(new CSearchHelper());
	m_conns.push_back( m_searchHelper->GotResult.connect(m_searchHelper->GuiWrapper(boost::bind(&CSearchResultWnd::OnSearchResult, this, _1, _2, _3, _4))) );
	m_conns.push_back( m_searchHelper->GotError.connect(m_searchHelper->GuiWrapper(boost::bind(&CSearchResultWnd::OnSearchError, this, _1))) );

	m_conns.push_back( CFileInfoLoader::Instance().GotFileInfo.connect(CFileInfoLoader::Instance().GuiWrapper(boost::bind(&CSearchResultWnd::OnFileInfoLoaded, this, _1))) );
}

CSearchResultWnd::~CSearchResultWnd(void)
{
	boost::signals2::connection conn;
	BOOST_FOREACH(conn, m_conns)
	{
		conn.disconnect();
	}

	m_searchHelper->Stop();
	m_searchHelper->DestroyWindow();
	m_searchHelper.reset();


}


BOOL CSearchResultWnd::OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent)
{
	if(!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
		return FALSE;

	
	m_searchResult.Init();
	m_searchResult.Create(WS_VISIBLE, 0, 0, cx, cy, this, IDC_LIST_SEARCH_RESULT);
	m_searchResult.SetEventListener(this);

	return TRUE;
}

void CSearchResultWnd::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	m_searchResult.SetWndLessPos(0, 0, szNew.cx, szNew.cy, SWLP_CX|SWLP_CY);

}
BOOL CSearchResultWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE;
	BOOL bRes = TRUE;
	switch(message)
	{
	case WM_COMMAND :
		{
			bRes = OnCommand(wParam, lParam, pResult) ;
		}
		break;
	case WM_NOTIFY :
		{
			bRes = OnNotify(wParam, lParam, pResult) ;
		}
		break;
	default:
		bRes = FALSE;
		break;
	}

	return bRes;

}

BOOL CSearchResultWnd::OnNotify(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE;

	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
	switch (pNMWLHDR->idFrom) 
	{
	case IDC_LIST_SEARCH_RESULT :
		{
			switch (pNMWLHDR->code)
			{
			case SRLN_VISIBLE_ITEM_CHANGE :
				{
					LPNMVISIBLERANGE vr = (LPNMVISIBLERANGE)pNMWLHDR;

					for (unsigned int index = vr->fromIndex; index <= vr->toIndex; index++)
					{
						LPCTSTR path = m_searchResult.GetItemText(index);

						if (path != L"")
						{
							FileInfo info;

							CFileInfoLoader::Instance().LoadFileInfo(path, -1, info);
						}

					}

					//try to load more

					unsigned int curr_count = m_searchResult.GetItemCount();

					if (vr->toIndex == (curr_count-1) && curr_count < m_totalResultCount && m_totalResultCount>0 && !m_offsetRequested.count(curr_count))
					{
						GetCoreEtApi()->Search(m_currKey, curr_count, m_searchHelper);
						m_offsetRequested.insert(curr_count);
					}
					
				}
				break ;
			}
		}
		break ;
	}

	return bRes ;
}



BOOL CSearchResultWnd::OnCommand(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
// 	switch ( pNMWLHDR->code )
// 	{
// 	default:
// 		bRes = FALSE;
// 		break;
// 	}
	return bRes ;
}




void CSearchResultWnd::OnListItemSelectChanged(IWLSimpleList *, IWLSimpleListItem * /*item*/, bool /*gain_or_lose*/)
{


}
void CSearchResultWnd::OnListLeftClick(IWLSimpleList *, IWLSimpleListItem *item)
{

	hdutils::OpenFolder(item->GetText());

}
void CSearchResultWnd::OnListLeftDoubleClick(IWLSimpleList *, IWLSimpleListItem *)
{

}
void CSearchResultWnd::OnListRightClick(IWLSimpleList *, IWLSimpleListItem *)
{

}
void CSearchResultWnd::OnListRightDoubleClick(IWLSimpleList *, IWLSimpleListItem *)
{

}


void CSearchResultWnd::DoSearch(LPCTSTR text)
{
	m_currKey  = text;
	GetCoreEtApi()->Search(m_currKey, 0, m_searchHelper);
	m_searchResult.DeleteAll();
	m_totalResultCount = 0;
	m_offsetRequested.clear();
	m_offsetRequested.insert(0);
}


void CSearchResultWnd::OnSearchResult(const std::wstring &key, unsigned int total, unsigned int offset, const std::vector<std::wstring>& fullPaths)
{
	m_totalResultCount = total;

	if (fullPaths.size() > 0)
	{
		std::vector<std::wstring>::const_iterator it;
		for (it = fullPaths.begin(); it != fullPaths.end(); it++)
		{
			m_searchResult.Insert(it->c_str());
			if (offset == 0)
			{
				FileInfo info;
				CFileInfoLoader::Instance().LoadFileInfo(it->c_str(), -1, info);
			}

		}

		m_searchResult.GetSelectManager()->SelectItem(m_searchResult.GetItem(0), 0);


	}

}

void CSearchResultWnd::OnSearchError(unsigned int errorCode)
{

}
void CSearchResultWnd::OnFileInfoLoaded(FileInfo & info)
{
	CSearchResultItem * item = m_searchResult.SearchVisibleItem(info.fullpath);
	if (item && info.imgIcon)
	{
		item->SetIcon(info.imgIcon);
	}
}
