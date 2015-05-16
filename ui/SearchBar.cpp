#include "StdAfx.h"
#include "SearchBar.h"
#include "uilib/common/utils/ImageEx/ImageEx.h"
CSearchBar::CSearchBar(void)
{
}

CSearchBar::~CSearchBar(void)
{
}


BOOL CSearchBar::OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent)
{
	if(!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
		return FALSE;

	if (!m_btnHome.Create(WS_VISIBLE | WS_TABSTOP, 0, y, 24, 24, this, IDC_BUTTON_HOME))
		return FALSE;
	WLButtonALProcess( m_btnHome, _T("DefPushButton"));


	m_input.SetCueBanner(_T("找到你想要的")) ;
	if (!m_input.Create( WS_VISIBLE | WS_TABSTOP, 20, y, 300, 24, this, IDC_SEARCH_INPUT ))
		return FALSE ;
	m_input.SetBubbleImage(IDI_SEARCH_INPUT_FRAME_PNG) ;
	m_input.SetSearchIcon(IDI_SEARCH_INPUT_MAGNIFIER_PNG) ;
	m_input.SetMargin(6, 6, 6, 6) ;
	m_input.SetOwner( this );
	
	
	m_history.Init();
	m_history.Create(0, 20, y + 30, 300, 100, this, IDC_LIST_HISTORY);
	m_history.SetEventListener(this);
	m_history.SetZorder(NULL);

	return TRUE;
}

BOOL CSearchBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE;
	BOOL bRes = TRUE;
	switch(message)
	{
	case WM_NOTIFY :
		{
			bRes = OnNotify(wParam, lParam, pResult) ;
		}
		break;
	case WM_KILLFOCUS :
		{
			HWL hwlFocus = (HWL)wParam ;
			if (hwlFocus == NULL || !m_history.IsChild(hwlFocus, TRUE))
			{
				m_history.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE);
			}
		}
		break;
	case WM_LDOWNHOST :
		{
			m_history.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE);
		}
		break;
	default:
		bRes = FALSE;
		break;
	}

	return bRes;

}


BOOL CSearchBar::OnNotify(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
	switch ( pNMWLHDR->code )
	{
	case SBN_BEGINSEARCH :
	case SBN_UPDATESEARCH :
		{
			if (!m_history.IsWndLessVisible() && m_history.GetItemCount()>0)
			{
				m_history.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW);
			}
			CString text;
			m_input.GetWindowText(text);
			m_history.SearchItem(text);

			bRes = TRUE;
		}
		break ;
	case SBN_UP :
		{
			
			HandleArrow(true);
			bRes = TRUE;
		}
		break ;
	case SBN_DOWN :
		{
			HandleArrow(false);
			bRes = TRUE;
		}
		break ;
	case SBN_ENDSEARCH :
		{
			Submit(_T(""));
			bRes = TRUE;
		}
		break ;
	case SBN_RETURN :
		{
			CString key;
			m_input.GetWindowText(key);
			if (key.GetLength() > 0)
			{
				int index = m_history.SearchItem(key.GetString());
				if (index == -1)
				{
					m_history.Insert(key.GetString());
				}
			}

			Submit(key);
			
			bRes = TRUE;
		}
		break ;
	}
	return bRes ;
}

void CSearchBar::HandleArrow(bool upOrDown)
{

	if (!m_history.IsWndLessVisible())
	{
		m_history.SetWndLessPos(0, 0, 0, 0, SWLP_SHOW);
	}
	IWLSelectableItem * curr_item = m_history.GetSelectManager()->GetSelectedItem();
	if (curr_item)
	{
		int index = curr_item->GetIndex();
		if (upOrDown)
		{
			index --;
		}
		else
		{
			index ++;
		}

		if (index >= 0 && index < m_history.GetItemCount())
		{
			m_history.GetSelectManager()->SelectItem(m_history.GetItem(index), 0);
		}		
	}
	else
	{
		if (0 < m_history.GetItemCount())
		{
			m_history.GetSelectManager()->SelectItem(m_history.GetItem(0), 0);
		}

	}

}

void CSearchBar::Submit(const CString & text)
{
	if (text.GetLength() > 0)
	{
		CString tip;
		tip.Format(_T("正在找 “%s”"), text);
		m_input.SetCueBanner(tip) ;
	}
	else
	{
		m_input.SetCueBanner(_T("找到你想要的...")) ;
	}

	m_input.EndSearch();
	m_history.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE);

	NMSEARCHINFO si ;
	si.hdr.hWlFrom = this ;
	si.hdr.idFrom = GetID() ;
	si.hdr.code = SBN_SUBMIT_SEARCH ;
	si.sKeyWord = text;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)-1, (LPARAM)&si) ;

}

void CSearchBar::OnListItemSelectChanged(IWLSimpleList *, IWLSimpleListItem * /*item*/, bool /*gain_or_lose*/)
{


}
void CSearchBar::OnListLeftClick(IWLSimpleList *, IWLSimpleListItem *item)
{
	if (item)
	{
		CString text = m_history.GetItemText(item->GetIndex());
		Submit(text); 
	}

}
void CSearchBar::OnListLeftDoubleClick(IWLSimpleList *, IWLSimpleListItem *)
{

}
void CSearchBar::OnListRightClick(IWLSimpleList *, IWLSimpleListItem *)
{

}
void CSearchBar::OnListRightDoubleClick(IWLSimpleList *, IWLSimpleListItem *)
{

}