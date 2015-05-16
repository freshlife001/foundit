#pragma once

#include "IWndLess.h"


class IWLButton :  public IWndLess
{
public:
	virtual ~IWLButton(void){};
	typedef struct _tagNMCDWLBTNINFO
	{
		NMWLHDR hdr ;
		HDC hDC ;
		RECT rc ;
		DWORD dwStatus ;
	} NMCDWLBTNINFO, *LPNMCDWLBTNINFO ;

	enum 
	{
		BS_NORMAL	= 0x00,
		BS_FOCUS	= 0x01,
		BS_HOT		= 0x02,
		BS_CHECK	= 0x04,
		BS_SELECTED	= 0x08,
		BS_DISABLED	= 0x10,
	};

	virtual int GetCheck() = 0 ;
	virtual void SetCheck(int nCheck) = 0 ;
	virtual BOOL DrawBySelf(HDC hDC, RECT const &rcUpdate, DWORD dwStatus) = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLButton> >  WLButtonImpBase;

template <class IT = IWLButton, class IMPT = WLButtonImpBase > 
class CWLButtonShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLButtonShell() {}
	virtual ~CWLButtonShell(){}
	virtual int GetCheck() 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCheck();
		}
		return 0 ;
	};
	virtual void SetCheck(int nCheck)	
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCheck(nCheck);
		}
		return;
	};

	virtual BOOL DrawBySelf(HDC hDC, RECT const &rcUpdate, DWORD dwStatus)
	{
		if (m_wlimp)
		{
			return m_wlimp->DrawBySelf(hDC, rcUpdate, dwStatus);
		}
		return FALSE;
	};
};

typedef CWLButtonShell<> WLButton;



