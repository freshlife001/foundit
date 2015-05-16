//////////////////////////////////////////////////////////////////////////
//
//	描述:   画布dc，专门用于保存窗口背景
//
//	
//	日期:   2011/02/19
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "uilib/uilib_exports.h"

class UILIB_EXPORT CCanvasDC
{
public:
	CCanvasDC(/*BOOL bRequest32 = FALSE*/);
	~CCanvasDC(void);

public:
	// init
	BOOL CreateCompatibleDC(HDC hDC, int cx, int cy) ;
	BOOL Create32BitDC(HDC hDC, int cx, int cy) ;
	BOOL CreateFromImage(HDC hDC, Image * pImage, BOOL bRequest32) ;

	// uninit
	void Clear() ;

public:
	// operation
	BOOL ChangeSize(int cx, int cy) ;
	BOOL SelectFont(HFONT hFont) ;

	HBITMAP GetBitmap() ;
	BOOL ReleaseBitmap(HBITMAP bmp) ;

	// attribute
	BOOL IsAvailable() const
	{
		return m_hDC != NULL ;
	}

	BOOL IsRequest32() const { return m_bRequest32 ; }
	int GetWidth() const { return m_nCX ; }
	int GetHeight() const { return m_nCY ; }

	operator HDC () const
	{
		return m_hDC ;
	}
	HDC GetDC() const { return m_hDC ; }

protected:
	HBITMAP CreateDesiredBitmap(HDC hDC, int cx, int cy, BOOL bRequest32) ;
	HBITMAP Create32Bitmap(HDC hDC, int cx, int cy) ;

protected:
	HDC m_hDC ;
	HGDIOBJ m_hOldBmp ;
	HGDIOBJ m_hOldFont ;

	BOOL m_bRequest32 ;

	int m_nCX ;
	int m_nCY ;

	BOOL m_bHollow ;		// 是否空心(没有bmp)，调用GetBitmap后为FALSE

};
