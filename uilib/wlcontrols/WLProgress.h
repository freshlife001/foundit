#pragma once
#include "IWLProgress.h"

class WLProgressImp :
	public WLProgressImpBase
{
public:
	WLProgressImp(void);
	virtual ~WLProgressImp(void);
public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	void LoadImage(Image* pImgChannel, Image* pImgChannelActive);

	virtual void SetRange(int nMin, int nMax);

	virtual int GetRangeMin() const;
	virtual int GetRangeMax() const;

	virtual void SetPos(int nPos, BOOL bRedraw = TRUE);
	virtual int GetCurPos() const;

	virtual void SetStep(int nStep);
	virtual int GetStep() const;
	virtual void StepIt();

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
	int  m_nMin;
	int  m_nMax;
	int  m_nPos;
	int  m_nStep;

	Image* m_pChannel;
	Image* m_pChannelActive;
	CSize  m_szImage;
};
