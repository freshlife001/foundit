#pragma once

#include "IWndLess.h"


typedef struct _tagSCROLLMSGINFO
{
	UINT nSBCode ;
	int nPos ;
}SCROLLMSGINFO, *LPSCROLLMSGINFO ;


class IWLScrollBar : public IWndLess
{
public:
	enum
	{
		STYLE_HORZ				= 0x00000000L,
		STYLE_VERT				= 0x00000001L,
		STYLE_TOPALIGN			= 0x00000000L,
		STYLE_LEFTALIGN			= 0x00000000L,
		STYLE_BOTTOMALIGN		= 0x00000002L,
		STYLE_RIGHTALIGN		= 0x00000002L,
		STYLE_NOARROW			= 0x00000004L,
		//STYLE_DISABLENOSCROLL	= 0x00000008L,
	};

public:
	virtual ~IWLScrollBar(void){};
	//////////////////////////////////////////////////////////////////////////
	// Operation
	virtual BOOL SetImage(UINT uID) = 0;
	virtual int GetScrollWidth() const = 0;

	virtual int SetScrollInfo(LPCSCROLLINFO lpsi) = 0;
	virtual BOOL GetScrollInfo(
		LPSCROLLINFO lpScrollInfo,
		UINT nMask = SIF_ALL 
		) = 0;
	
	

};

typedef CWndLessImpBase< CWndLessShell<IWLScrollBar> >  WLScrollBarImpBase;

template <class IT = IWLScrollBar, class IMPT = WLScrollBarImpBase > 
class CWLScrollBarShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLScrollBarShell() {}
	virtual ~CWLScrollBarShell(){}

	BOOL SetImage(UINT uID)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetImage(uID);
		}
		return FALSE;
	};
	int GetScrollWidth() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetScrollWidth();
		}
		return 0;
	};

	int SetScrollInfo(LPCSCROLLINFO lpsi) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetScrollInfo(lpsi);
		}
		return 0;
	};

	BOOL GetScrollInfo(
		LPSCROLLINFO lpScrollInfo,
		UINT nMask
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->GetScrollInfo(lpScrollInfo, nMask);
		}
		return FALSE;
	};

};

typedef CWLScrollBarShell<> WLScrollBar;



