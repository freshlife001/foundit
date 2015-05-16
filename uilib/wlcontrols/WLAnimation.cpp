#include "StdAfx.h"
#include "WLAnimation.h"
#include "ImageEx/ImageEx.h"
#include "common/utils/Graphics.h"
#include <boost/bind.hpp>
#include "common/MemoryLeakChecker.h"
WLAnimation::WLAnimation(void)
:
m_uImageColCount(1)
, m_uImageRowCount(1)
, m_uPlayElapse(80)
, m_uFrameIndex(0)
, m_uFrameWidth(0)
, m_uFrameHeight(0)
, m_make_gray(FALSE)
{
}

WLAnimation::~WLAnimation(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

BOOL WLAnimation::Open(LPCTSTR lpszPath) 
{
	if (m_dwStyle & AS_CMBD)
		return FALSE ;

	Shared_ImageEx img = Shared_ImageEx::FromFile(lpszPath) ;
	if (img == NULL)
		return FALSE ;

	return Open(img) ;
}

BOOL WLAnimation::Open(UINT nID) 
{
	if (m_dwStyle & AS_CMBD)
		return FALSE ;

	Shared_ImageEx img = Shared_ImageEx::FromId(nID) ;
	if (img == NULL)
		return FALSE ;

	return Open(img) ;
}

BOOL WLAnimation::Open(Shared_ImageEx &imgAnimation) 
{
	m_imgAnimation = imgAnimation ;
	m_imgAnimation.SetCallBackFunction(boost::bind(&WLAnimation::PlayCallbackFucntion, this)) ;
	m_imgAnimation->Stop() ;
	UpdateView() ;

	if (m_dwStyle & AS_AUTOFITIMAGE)
	{
		int nWidth = m_imgAnimation->GetWidth() ;
		int nHeight = m_imgAnimation->GetHeight() ;
		SetWndLessPos(0, 0, nWidth, nHeight, SWLP_CX | SWLP_CY) ;
	}

	return TRUE ;
}

BOOL WLAnimation::Open(LPCTSTR lpszPath, UINT uCol, UINT uRow) 
{
	if (!(m_dwStyle & AS_CMBD))
		return FALSE ;

	Shared_ImageEx img = Shared_ImageEx::FromFile(lpszPath) ;
	if (img == NULL)
		return FALSE ;

	return Open(img, uCol, uRow) ;
}

BOOL WLAnimation::Open(UINT nID, UINT uCol, UINT uRow) 
{
	if (!(m_dwStyle & AS_CMBD))
		return FALSE ;

	Shared_ImageEx img = Shared_ImageEx::FromId(nID) ;
	if (img == NULL)
		return FALSE ;

	return Open(img, uCol, uRow) ;
}

BOOL WLAnimation::Open(Shared_ImageEx &imgAnimation, UINT uCol, UINT uRow) 
{
	m_imgAnimation = imgAnimation ;

	m_uImageColCount = uCol ;
	m_uImageRowCount = uRow ;
	m_uFrameWidth = m_imgAnimation->GetWidth() / uCol ;
	m_uFrameHeight = m_imgAnimation->GetHeight() / uRow ;

	if (m_dwStyle & AS_AUTOFITIMAGE)
	{
		SetWndLessPos(0, 0, m_uFrameWidth, m_uFrameHeight, SWLP_CX | SWLP_CY) ;
	}

	return TRUE ;
}

void WLAnimation::Close() 
{
	m_imgAnimation.Reset() ;
}

UINT WLAnimation::SetPlayElapse(UINT uPlayElapse) 
{
	UINT uTemp = m_uPlayElapse ;
	m_uPlayElapse = uPlayElapse ;

	return uTemp ;
}

void WLAnimation::Play() 
{
	if (m_dwStyle & AS_CMBD)
	{
		m_uFrameIndex = 0 ;
		SetTimer(TIMERID_ANIMATION_PLAY, m_uPlayElapse, NULL) ;
	}
	else
		m_imgAnimation->Play() ;
}

void WLAnimation::Stop() 
{
	if (m_dwStyle & AS_CMBD)
	{
		KillTimer(TIMERID_ANIMATION_PLAY) ;
		m_uFrameIndex = 0 ;
		UpdateView() ;
	}
	else
		m_imgAnimation->Stop() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLAnimation::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_TIMER :
		{
			OnTimer((UINT_PTR)wParam) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLAnimation::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	if (m_imgAnimation != NULL)
	{
		CRect rcInHost ;
		GetRectInHost(rcInHost) ;

		Graphics gph(hDC) ;

        ImageAttributes ia ;
        if (m_make_gray)
        {
            Gdiplus::ColorMatrix cm_gray =
            {
                (Gdiplus::REAL)0.30, (Gdiplus::REAL)0.30, (Gdiplus::REAL)0.30, 0.00, 0.00,
                (Gdiplus::REAL)0.59, (Gdiplus::REAL)0.59, (Gdiplus::REAL)0.59, 0.00, 0.00,
                (Gdiplus::REAL)0.11, (Gdiplus::REAL)0.11, (Gdiplus::REAL)0.11, 0.00, 0.00,
                (Gdiplus::REAL)0.00, 0.00, 0.00, 1.00, 0.00,
                (Gdiplus::REAL)0.00, 0.00, 0.00, 0.00, 1.00
            };
            ia.SetColorMatrix(&cm_gray) ;
        }

		if (m_dwStyle & AS_CMBD)
		{
			int nLeft = m_uFrameIndex % m_uImageColCount * m_uFrameWidth ;
			int nTop = m_uFrameIndex / m_uImageColCount * m_uFrameHeight ;
// 			gph.DrawImage(
// 				m_imgAnimation,
// 				Rect(rcInHost.left, rcInHost.top, rcInHost.Width(), rcInHost.Height()),
// 				nLeft, nTop, m_uFrameWidth, m_uFrameHeight, UnitPixel, NULL
// 				) ;
			hdutils::DrawImageIndeformable(
				gph,
				m_imgAnimation,
				rcInHost,
				nLeft, nTop, m_uFrameWidth, m_uFrameHeight,
                &ia
				) ;
		}
		else
		{
// 			gph.DrawImage(
// 				m_imgAnimation,
// 				Rect(rcInHost.left, rcInHost.top, rcInHost.Width(), rcInHost.Height()),
// 				0, 0, m_imgAnimation->GetWidth(), m_imgAnimation->GetHeight(), UnitPixel, NULL
// 				) ;
			hdutils::DrawImageIndeformable(
				gph,
				m_imgAnimation,
                rcInHost,
                &ia
				) ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Handler

void WLAnimation::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == TIMERID_ANIMATION_PLAY)
	{
		if (!(m_dwStyle & AS_CMBD))
			return ;

		++ m_uFrameIndex ;
		m_uFrameIndex %= (m_uImageColCount * m_uImageRowCount) ;
		UpdateView() ;
	}
}

//////////////////////////////////////////////////////////////////////////
// Inner Function

void WLAnimation::PlayCallbackFucntion() 
{
	UpdateView() ;
}