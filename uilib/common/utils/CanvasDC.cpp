#include "StdAfx.h"
#include "CanvasDC.h"
#include "graphics.h"
#include "common/MemoryLeakChecker.h"

CCanvasDC::CCanvasDC()
:
m_hDC(NULL)
, m_hOldBmp(NULL)
, m_hOldFont(NULL)
, m_nCX(0)
, m_nCY(0)
, m_bHollow(TRUE)
{
}

CCanvasDC::~CCanvasDC(void)
{
	Clear() ;
}

// init
BOOL CCanvasDC::CreateCompatibleDC(HDC hDC, int cx, int cy) 
{
	if (IsAvailable())
	{
		ASSERT(FALSE) ;
		return FALSE ;
	}

	if (cx == 0 || cy == 0)
	{
		return FALSE ;
	}

	m_hDC = ::CreateCompatibleDC(hDC) ;
	HBITMAP hNewBmp = ::CreateCompatibleBitmap(hDC, cx, cy) ;
	m_hOldBmp = ::SelectObject(m_hDC, hNewBmp) ;

	m_bHollow = FALSE ;

	m_nCX = cx ;
	m_nCY = cy ;

	return TRUE ;
}

BOOL CCanvasDC::Create32BitDC(HDC hDC, int cx, int cy) 
{
	if (IsAvailable())
	{
		ASSERT(FALSE) ;
		return FALSE ;
	}

	if (cx == 0 || cy == 0)
	{
		return FALSE ;
	}

	HDC desktop_dc = ::GetDC(NULL) ;
	m_hDC = ::CreateCompatibleDC(desktop_dc) ;
	::ReleaseDC(NULL, desktop_dc) ;

	m_bRequest32 = TRUE ;
	HBITMAP hNewBmp = hdutils::Create32Bitmap(cx, cy) ;
	m_hOldBmp = ::SelectObject(m_hDC, hNewBmp) ;

	m_bHollow = FALSE ;

	m_nCX = cx ;
	m_nCY = cy ;

	return TRUE ;
}

BOOL CCanvasDC::CreateFromImage(HDC hDC, Image * pImage, BOOL bRequest32) 
{
	if (pImage == NULL || pImage->GetLastStatus() != Ok)
		return FALSE ;

	if (IsAvailable())
	{
		ASSERT(FALSE) ;
		return FALSE ;
	}

	int nWidth = pImage->GetWidth() ;
	int nHeight = pImage->GetHeight() ;

	if (bRequest32)
	{
		if (!Create32BitDC(hDC, nWidth, nHeight))
			return FALSE ;
	}
	else
	{
		if (!CreateCompatibleDC(hDC, nWidth, nHeight))
			return FALSE ;
	}

	{
		Gdiplus::ImageAttributes ia ;
		ia.SetWrapMode(Gdiplus::WrapModeTile) ;

		Graphics gph(m_hDC) ;
		gph.DrawImage(
			pImage,
			Rect(0, 0, nWidth, nHeight),
			0, 0, nWidth, nHeight, UnitPixel, &ia
			) ;
	}

	return TRUE ;
}

// uninit
void CCanvasDC::Clear() 
{
	if (IsAvailable())
	{
		if (!m_bHollow)
			::DeleteObject(::SelectObject(m_hDC, m_hOldBmp)) ;

		if (m_hOldFont != NULL)
			::SelectObject(m_hDC, m_hOldFont) ;

		::DeleteDC(m_hDC) ;

		m_hDC = NULL ;
		m_hOldBmp = NULL ;
		m_hOldFont = NULL ;
		m_nCX = 0 ;
		m_nCY = 0 ;
		m_bHollow = TRUE ;
	}
}

// operation
BOOL CCanvasDC::ChangeSize(int cx, int cy) 
{
	if (!IsAvailable() || m_bHollow)
	{
		return FALSE ;
	}

	if (m_nCX == cx && m_nCY == cy)
		return TRUE ;

	// 修正宽度或者高度为0时导致m_hDC无效的BUG
	if (cx == 0 || cy == 0)
		return FALSE ;

	HBITMAP hNewBmp = CreateDesiredBitmap(m_hDC, cx, cy, m_bRequest32) ;
	::DeleteObject(::SelectObject(m_hDC, hNewBmp)) ;
	m_nCX = cx ;
	m_nCY = cy ;

	return TRUE ;
}

// hFont由调用方保存
BOOL CCanvasDC::SelectFont(HFONT hFont) 
{
	if (!IsAvailable())
		return FALSE ;

	HGDIOBJ hOldFont = ::SelectObject(m_hDC, hFont) ;
	if (m_hOldFont == NULL)
	{
		// 保留最早的字体句柄
		m_hOldFont = hOldFont ;
	}

	return TRUE ;
}

HBITMAP CCanvasDC::GetBitmap() 
{
	if (!IsAvailable() || m_bHollow)
		return NULL ;

	m_bHollow = TRUE ;
	return (HBITMAP)::SelectObject(m_hDC, m_hOldBmp) ;
}

BOOL CCanvasDC::ReleaseBitmap(HBITMAP bmp) 
{
	if (!IsAvailable() || !m_bHollow)
		return FALSE ;

	m_bHollow = FALSE ;
	m_hOldBmp = ::SelectObject(m_hDC, bmp) ;
	return TRUE ;
}

// attribute
HBITMAP CCanvasDC::CreateDesiredBitmap(HDC hDC, int cx, int cy, BOOL bRequest32) 
{
	if (bRequest32)
	{
		return Create32Bitmap(hDC, cx, cy) ;
	}
	else
	{
		return ::CreateCompatibleBitmap(hDC, cx, cy) ;
	}
}

HBITMAP CCanvasDC::Create32Bitmap(HDC hDC, int cx, int cy) 
{
	if ((GetDeviceCaps(hDC, BITSPIXEL) == 32))
	{
		return ::CreateCompatibleBitmap(hDC, cx, cy) ;
	}
	else
	{
		return hdutils::Create32Bitmap(cx, cy) ;
	}
}
