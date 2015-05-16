#pragma once

#include "IWndLess.h"


class IWLStatic : public IWndLess
{
public:
	enum STYLE
	{
		SS_ALIGN_LEFT			= 0x00000000,
		SS_ALIGN_CENTER			= 0x00000001,
		SS_ALIGN_RIGHT			= 0x00000002,
		SS_ALIGN_TOP			= 0x00000000,
		SS_ALIGN_VCENTER		= 0x00000004,
		SS_ALIGN_BOTTOM			= 0x00000008,
		SS_AUTOFIT				= 0x00001000,		// 自适应文字大小
	};
public:
	virtual ~IWLStatic(void){};
	
	virtual COLORREF SetTextColor(COLORREF crTextColor) = 0 ;
	virtual void SetFont(HFONT hFont)  = 0 ;

	// 必须在创建后使用
	virtual void SizeToContent()  = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLStatic> >  WLStaticImpBase;

template <class IT = IWLStatic, class IMPT = WLStaticImpBase > 
class CWLStaticShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLStaticShell() {}
	virtual ~CWLStaticShell(){}
	virtual COLORREF SetTextColor(COLORREF crTextColor) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetTextColor(crTextColor);
		}
		return 0 ;
	};
	virtual void SetFont(HFONT hFont)	
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFont(hFont);
		}
		return;
	};

	virtual void SizeToContent()	
	{
		if (m_wlimp)
		{
			return m_wlimp->SizeToContent();
		}
		return;
	};
};

typedef CWLStaticShell<> WLStatic;



