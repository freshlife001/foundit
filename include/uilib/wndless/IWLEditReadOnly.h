#pragma once

#include "IWndLess.h"


class IWLEditReadOnly : public IWndLess
{
public:

public:
	virtual ~IWLEditReadOnly(void){};

	// Operations
public:
	virtual void SetMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		)  = 0;
	virtual void GetMargin(LPRECT lprcMargin) const  = 0;

	virtual void SetSel(int nStartChar, int nEndChar)  = 0;
	virtual void LimitText(int nChars)  = 0;
	virtual BOOL IsEmpty() const  = 0;

	virtual Color SetBkgndColor(Color crBkgndColor)  = 0;

	virtual COLORREF SetTextNormalColor(COLORREF crTextNormal)  = 0;

	virtual void SetRowInterval(int nRowInterval)  = 0;

	virtual void Copy()  = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLEditReadOnly> >  WLEditReadOnlyImpBase;

template <class IT = IWLEditReadOnly, class IMPT = WLEditReadOnlyImpBase > 
class CWLEditReadOnlyShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLEditReadOnlyShell() {}
	virtual ~CWLEditReadOnlyShell(){}

	virtual void SetMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetMargin(nLeftMargin, nTopMargin, nRightMargin, nBottomMargin);
		}
	}
	virtual void GetMargin(LPRECT lprcMargin) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetMargin(lprcMargin);
		}
	}

	virtual void SetSel(int nStartChar, int nEndChar) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetSel(nStartChar, nEndChar);
		}
	}
	virtual void LimitText(int nChars)
	{
		if (m_wlimp)
		{
			return m_wlimp->LimitText(nChars);
		}
	}
	virtual BOOL IsEmpty() const
	{
		if (m_wlimp)
		{
			return m_wlimp->IsEmpty();
		}
		return TRUE;
	}

	virtual Color SetBkgndColor(Color crBkgndColor)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetBkgndColor(crBkgndColor);
		}
		return crBkgndColor;
	}

	virtual COLORREF SetTextNormalColor(COLORREF crTextNormal)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetTextNormalColor(crTextNormal);
		}
		return crTextNormal;
	}
	virtual void SetRowInterval(int nRowInterval)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetRowInterval(nRowInterval);
		}
	}

	virtual void Copy()
	{
		if (m_wlimp)
		{
			return m_wlimp->Copy();
		}
	}
	
};

typedef CWLEditReadOnlyShell<> WLEditReadOnly;



