#pragma once
#include "IWndLess.h"
class IWLScrollWndPlate : public IWndLess
{
public:
	virtual ~IWLScrollWndPlate(void){};
	virtual void OnViewportWidthChanged(int /*old_width*/, int /*new_width*/) = 0;
	virtual void OnViewportHeightChanged(int /*old_height*/, int /*new_height*/) = 0;
};



typedef CWndLessImpBase< CWndLessShell<IWLScrollWndPlate> >  WLScrollWndPlateImpBase;

template <class IT = IWLScrollWndPlate, class IMPT = WLScrollWndPlateImpBase > 
class CWLScrollWndPlateShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLScrollWndPlateShell() {}
	virtual ~CWLScrollWndPlateShell(){}
	virtual void OnViewportWidthChanged(int old_width, int new_width) 
	{
		if (m_wlimp)
		{
			return m_wlimp->OnViewportWidthChanged(old_width, new_width);
		}
		return ;

	}
	virtual void OnViewportHeightChanged(int old_height, int new_height)
	{
		if (m_wlimp)
		{
			return m_wlimp->OnViewportHeightChanged(old_height, new_height);
		}
		return ;

	}
};

typedef CWLScrollWndPlateShell<> WLScrollWndPlate;