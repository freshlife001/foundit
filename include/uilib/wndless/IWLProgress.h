#pragma once
#include "IWndLess.h"

class IWLProgress : public IWndLess
{
public:
	virtual ~IWLProgress(void){};

public:
	virtual void LoadImage(Image* pImgChannel, Image* pImgChannelActive) = 0;
	virtual void SetRange(int nMin, int nMax) = 0;

	virtual int GetRangeMin() const = 0;
	virtual int GetRangeMax() const = 0;

	virtual void SetPos(int nPos, BOOL bRedraw = TRUE) = 0;
	virtual int GetCurPos() const = 0;

	virtual void SetStep(int nStep) = 0;
	virtual int GetStep() const = 0;
	virtual void StepIt() = 0;
};

typedef CWndLessImpBase< CWndLessShell<IWLProgress> >  WLProgressImpBase;

template <class IT = IWLProgress, class IMPT = WLProgressImpBase > 
class CWLProgressShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLProgressShell() {}
	virtual ~CWLProgressShell(){}

public:
	virtual void LoadImage(Image* pImgChannel, Image* pImgChannelActive)
	{
		if(m_wlimp)
		{
			return m_wlimp->LoadImage(pImgChannel, pImgChannelActive);
		}
		return;
	}

	virtual void SetRange(int nMin, int nMax)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetRange(nMin, nMax);
		}
		return;
	}

	virtual int GetRangeMin() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRangeMin();
		}
		return 0;
	}

	virtual int GetRangeMax() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetRangeMax();
		}
		return 0;
	}

	virtual void SetPos(int nPos, BOOL bRedraw = TRUE)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetPos(nPos, bRedraw);
		}
		return;
	}

	virtual int GetCurPos() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCurPos();
		}
		return 0;
	}

	virtual void SetStep(int nStep)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetStep(nStep);
		}
		return;
	}

	virtual int GetStep() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetStep();
		}
		return 0;
	}

	virtual void StepIt()
	{
		if (m_wlimp)
		{
			return m_wlimp->StepIt();
		}
		return;
	}
};

typedef CWLProgressShell<> WLProgress;
