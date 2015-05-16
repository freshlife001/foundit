#pragma once

#include "IWndLess.h"


class IWLEdit : public IWndLess
{
public:
	enum STYLE
	{
		WLES_NOHIDESEL	= 0x00000001,
	};
public:
	virtual ~IWLEdit(void){};

	// Operations
public:
	virtual UINT SetFrameWidth(UINT uNewFrameWidth)  = 0;
	virtual void SetMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		)  = 0;
	virtual void GetMargin(LPRECT lprcMargin) const  = 0;

	virtual void SetCueBanner(LPCTSTR lpszText)  = 0;
	virtual CString GetCueBanner() const  = 0;

	virtual void ReplaceSel(LPCTSTR lpszNewText)  = 0;
	virtual void SetSel(int nStartChar, int nEndChar)  = 0;
	virtual void LimitText(int nChars)  = 0;
	virtual BOOL CanAutoCompleteWord() const   = 0;
	virtual BOOL IsEmpty() const   = 0;

	virtual COLORREF SetFrameColor(COLORREF crFrame)  = 0;
	virtual Color SetBkgndColor(Color crBkgndColor)  = 0;

	virtual COLORREF SetTextNormalColor(COLORREF crTextNormal)  = 0;

	virtual void Undo()  = 0;
	virtual void Cut()  = 0;
	virtual void Copy()  = 0;
	virtual void Paste()  = 0;
	virtual void Clear()  = 0;

	// 获得刚好存放文字时的大小
	virtual void CalcWrapContentSize(LPSIZE lpSize)  = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLEdit> >  WLEditImpBase;

template <class IT = IWLEdit, class IMPT = WLEditImpBase > 
class CWLEditShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLEditShell() {}
	virtual ~CWLEditShell(){}
	virtual UINT SetFrameWidth(UINT uNewFrameWidth)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFrameWidth(uNewFrameWidth);
		}
		return 0;
	}
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

	virtual void SetCueBanner(LPCTSTR lpszText)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCueBanner(lpszText);
		}
	}
	virtual CString GetCueBanner() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCueBanner();
		}
		return _T("");
	}

	virtual void ReplaceSel(LPCTSTR lpszNewText)
	{
		if (m_wlimp)
		{
			return m_wlimp->ReplaceSel(lpszNewText);
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
	virtual BOOL CanAutoCompleteWord() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->CanAutoCompleteWord();
		}
		return FALSE;
	}
	virtual BOOL IsEmpty() const
	{
		if (m_wlimp)
		{
			return m_wlimp->IsEmpty();
		}
		return TRUE;
	}

	virtual COLORREF SetFrameColor(COLORREF crFrame)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFrameColor(crFrame);
		}
		return 0;
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

	virtual void Undo()
	{
		if (m_wlimp)
		{
			return m_wlimp->Undo();
		}
	}
	virtual void Cut()
	{
		if (m_wlimp)
		{
			return m_wlimp->Cut();
		}
	}
	virtual void Copy()
	{
		if (m_wlimp)
		{
			return m_wlimp->Copy();
		}
	}
	virtual void Paste()
	{
		if (m_wlimp)
		{
			return m_wlimp->Paste();
		}
	}
	virtual void Clear()
	{
		if (m_wlimp)
		{
			return m_wlimp->Clear();
		}
	}

	// 获得刚好存放文字时的大小
	virtual void CalcWrapContentSize(LPSIZE lpSize)
	{
		if (m_wlimp)
		{
			return m_wlimp->CalcWrapContentSize(lpSize);
		}
	}
};

typedef CWLEditShell<> WLEdit;



