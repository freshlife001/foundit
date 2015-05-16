#include "StdAfx.h"
#include "WLStatic.h"
#include "common/utils/Graphics.h"
#include "common/MemoryLeakChecker.h"

WLStaticImp::WLStaticImp(void)
:
m_crTextColor(hdutils::crTextColor)
, m_hFont(NULL)
{
}

WLStaticImp::~WLStaticImp(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

COLORREF WLStaticImp::SetTextColor(COLORREF crTextColor) 
{
	if (m_crTextColor != crTextColor)
	{
		COLORREF crOld = m_crTextColor ;
		m_crTextColor = crTextColor ;
	}

	return m_crTextColor ;
}

void WLStaticImp::SetFont(HFONT hFont) 
{
	m_hFont = hFont ;
}

void WLStaticImp::SizeToContent() 
{
	WLDC dc(this) ;

	CString text;
	GetWindowText(text);

	CSize szContent = dc.GetTextExtent(text) ;
	SetWndLessPos(0, 0, szContent.cx, szContent.cy, SWLP_CX | SWLP_CY) ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

void WLStaticImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost) ;

	int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT) ;
	COLORREF crOldTextColor = ::SetTextColor(hDC, m_crTextColor) ;
	HGDIOBJ hOldFont = NULL ;
	if (m_hFont != NULL)
	{
		hOldFont = ::SelectObject(hDC, m_hFont) ;
	}

// 	::ExtTextOut(
// 		hDC, 
// 		rcInHost.left, rcInHost.top,
// 		ETO_CLIPPED,
// 		rcInHost,
// 		m_sText, m_sText.GetLength(),
// 		NULL
// 		) ;

	UINT uFormat = 0 ;

	DWORD style = GetStyle();
	if (style & SS_ALIGN_CENTER)
		uFormat |= DT_CENTER ;
	else if (style & SS_ALIGN_RIGHT)
		uFormat |= DT_RIGHT ;
	if (style & SS_ALIGN_VCENTER)
		uFormat |= DT_VCENTER ;
	else if (style & SS_ALIGN_BOTTOM)
		uFormat |= DT_BOTTOM ;

	CString text;
	GetWindowText(text);
	::DrawText(hDC, text, text.GetLength(), &rcInHost, uFormat | DT_SINGLELINE) ;

	if (m_hFont != NULL)
	{
		::SelectObject(hDC, hOldFont) ;
	}
	::SetTextColor(hDC, crOldTextColor) ;
	::SetBkMode(hDC, nOldBkMode) ;
}

BOOL WLStaticImp::SetWindowText(LPCTSTR lpszText) 
{
	if (!__super::SetWindowText(lpszText))
		return FALSE ;

	if (GetStyle() & SS_AUTOFIT)
		SizeToContent() ;

	return TRUE ;
}


template<>
UILIB_EXPORT WLStaticImpBase *CreateDefaultImp< WLStaticImpBase >(IWLStatic * wrapper)

{
	WLStaticImp *ret = new WLStaticImp();
	ret->SetWrapper(wrapper);
	return ret;
};
