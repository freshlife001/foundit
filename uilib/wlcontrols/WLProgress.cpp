#include "StdAfx.h"
#include "WLProgress.h"
#include "common/utils/Graphics.h"
#include "common/MemoryLeakChecker.h"

WLProgressImp::WLProgressImp(void)
	: m_nMin(0), m_nMax(0), m_nPos(0), m_nStep(0)
	, m_pChannel(NULL), m_pChannelActive(NULL)
{
	m_szImage.SetSize(0, 0);
}

WLProgressImp::~WLProgressImp(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

void WLProgressImp::LoadImage(Image* pImgChannel, Image* pImgChannelActive)
{
	m_pChannel = pImgChannel;
	m_pChannelActive = pImgChannelActive;
	ASSERT(m_pChannel);
	ASSERT(m_pChannelActive);

	if(m_pChannel)
	{
		m_szImage.cx = m_pChannel->GetWidth();
		m_szImage.cy = m_pChannel->GetHeight();
	}
	else if(m_pChannelActive)
	{
		m_szImage.cx = m_pChannelActive->GetWidth();
		m_szImage.cy = m_pChannelActive->GetHeight();
	}
}

void WLProgressImp::SetRange(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}

int WLProgressImp::GetRangeMin() const
{
	return m_nMin;
}

int WLProgressImp::GetRangeMax() const
{
	return m_nMax;
}

void WLProgressImp::SetPos(int nPos, BOOL bRedraw/* = TRUE*/)
{
	m_nPos = min(max(m_nMin, nPos), m_nMax);

	if(bRedraw)
		UpdateView();
}

int WLProgressImp::GetCurPos() const
{
	return m_nPos;
}

void WLProgressImp::SetStep(int nStep)
{
	m_nStep = nStep;
}

int WLProgressImp::GetStep() const
{
	return m_nStep;
}

void WLProgressImp::StepIt()
{
	int pos = m_nPos + m_nStep;
	SetPos(pos);
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

void WLProgressImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	CRect rcInHost ;
	GetRectInHost(rcInHost);

	double percent = (double)(m_nPos - m_nMin)/(double)(m_nMax - m_nMin);
	int des_pixels = (int)(percent * rcInHost.Width());
	int src_pixels = (int)(percent * m_szImage.cx);

	Graphics g(hDC);
	g.DrawImage(m_pChannel, Rect(rcInHost.left, rcInHost.top, rcInHost.Width(), rcInHost.Height()));
	if(des_pixels)
	{
		g.DrawImage(m_pChannelActive, Rect(rcInHost.left, rcInHost.top, des_pixels, rcInHost.Height()),
			0, 0, src_pixels, m_szImage.cy, UnitPixel);
	}
}

template<>
UILIB_EXPORT WLProgressImpBase *CreateDefaultImp< WLProgressImpBase >(IWLProgress * wrapper)
{
	WLProgressImp *ret = new WLProgressImp();
	ret->SetWrapper(wrapper);
	return ret;
};
