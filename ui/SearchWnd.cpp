#include "StdAfx.h"
#include "SearchWnd.h"

const unsigned int BAR_HEIGHT = 30;
const unsigned int MARGIN = 5;

CSearchWnd::CSearchWnd(void)
{
}

CSearchWnd::~CSearchWnd(void)
{
}


BOOL CSearchWnd::OnCreate(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess* pParent)
{
	if(!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
		return FALSE;

	m_searchBar.Create(WS_VISIBLE, MARGIN, 0, cx - 2 * MARGIN, cy, this, IDC_SEARCH_BAR);
	m_resultWnd.Create(WS_VISIBLE, MARGIN, BAR_HEIGHT + MARGIN, cx - 2 * MARGIN, cy - BAR_HEIGHT - 2 * MARGIN, this, IDC_SEARCH_RESULT);
	
	
	return TRUE;
}
BOOL CSearchWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE;
	BOOL bRes = TRUE;
	switch(message)
	{
	case WM_COMMAND:
		{
			bRes = OnCommand(wParam, lParam, pResult);
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

void CSearchWnd::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	m_resultWnd.SetWndLessPos(0, 0, szNew.cx - 2 * MARGIN, szNew.cy - BAR_HEIGHT - 2 * MARGIN, SWLP_CX|SWLP_CY);
}
BOOL CSearchWnd::OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
	WORD wCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);

	BOOL bRes = TRUE;
	switch(wCode)
	{
	case BN_CLICKED:
		{
			
		}
		break;
	case WM_MOUSEHOVER:

		break;
	case WM_MOUSELEAVE:

		break;
	case WM_RBUTTONUP:

		break;
	default:
		bRes = FALSE;
	}

	return bRes;
}



BOOL CSearchWnd::OnNotify(WPARAM /*wParam*/, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
	switch ( pNMWLHDR->code )
	{
	case SBN_SUBMIT_SEARCH :
		{

			LPNMSEARCHINFO psi =  (LPNMSEARCHINFO)lParam;
			CString key = psi->sKeyWord ;
			m_resultWnd.DoSearch(key);
			bRes = TRUE;
		}
		break ;
	
	}
	return bRes ;
}
