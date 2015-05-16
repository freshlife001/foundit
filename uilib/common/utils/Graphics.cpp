#include "stdafx.h"
#include "Graphics.h"
#include "CanvasDC.h"

namespace hdutils {

COLORREF crTextColor = RGB(0x33, 0x33, 0x33) ;

COLORREF GetTextColor()
{
	return crTextColor;
}

int s_arGlowMatrix[49] =
{
	1, 2, 3, 4, 3, 2, 1,
	2, 3, 4, 5, 4, 3, 2,
	3, 4, 5, 6, 5, 4, 3,
	4, 5, 6, 7, 6, 5, 4,
	3, 4, 5, 6, 5, 4, 3,
	2, 3, 4, 5, 4, 3, 2,
	1, 2, 3, 4, 3, 2, 1,
} ;

// 单方向拉伸绘制
void LineStretch(
				 HDC hdcDest,
				 int nXDest,
				 int nYDest,
				 int nWidthDest,
				 int nHeightDest,
				 Image *pImage,
				 int nXSrc,
				 int nYSrc,
				 int nWidthSrc,
				 int nHeightSrc,
				 const ImageAttributes* pImgAttr,
				 int nFixedFront,	// = -1		// 前端固定长度
				 int nFixedBack,	// = -1		// 后端固定长度
				 BOOL bHorz			// = TRUE	// 默认水平拉伸
				 ) 
{
	if (pImage == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
	{
		ASSERT(FALSE) ;
		return ;
	}

	Graphics gph(hdcDest) ;
	if (bHorz)
	{
		if (nFixedFront == -1)
			nFixedFront = nWidthSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nWidthSrc / 3 ;

		if (nFixedFront > 0)
		{
			gph.DrawImage(
				pImage,
				Rect(nXDest, nYDest, nFixedFront, nHeightDest),
				nXSrc, nYSrc, nFixedFront, nHeightSrc, UnitPixel, pImgAttr
				) ;
		}
		if (nWidthDest - nFixedFront - nFixedBack > 0)
		{
			// 修改说明[2011/06/22, LQL]: 这个修改失败了。。。
			// 修改说明[2011/06/21, LQL]: 有ia之后，再也不需要-1了
			//Gdiplus::ImageAttributes ia ;
			//ia.SetWrapMode(Gdiplus::WrapModeTile) ;
			gph.DrawImage(
				pImage,
				Rect(nXDest + nFixedFront, nYDest, nWidthDest - nFixedFront - nFixedBack, nHeightDest),
				nXSrc + nFixedFront, nYSrc, nWidthSrc - nFixedFront - nFixedBack - 1, nHeightSrc, UnitPixel, pImgAttr
				) ;
		}
		if (nFixedBack > 0)
		{
			gph.DrawImage(
				pImage,
				Rect(nXDest + nWidthDest - nFixedBack, nYDest, nFixedBack, nHeightDest),
				nXSrc + nWidthSrc - nFixedBack, nYSrc, nFixedBack, nHeightSrc, UnitPixel, pImgAttr
				) ;
		}
	}
	else
	{
		if (nFixedFront == -1)
			nFixedFront = nHeightSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nHeightSrc / 3 ;

		if (nFixedFront > 0)
		{
			gph.DrawImage(
				pImage,
				Rect(nXDest, nYDest, nWidthDest, nFixedFront),
				nXSrc, nYSrc, nWidthSrc, nFixedFront, UnitPixel, pImgAttr
				) ;
		}
		if (nHeightDest - nFixedFront - nFixedBack > 0)
		{
			// 修改说明[2011/06/22, LQL]: 这个修改失败了。。。
			// 修改说明[2011/06/21, LQL]: 有ia之后，再也不需要-1了
			//Gdiplus::ImageAttributes ia ;
			//ia.SetWrapMode(Gdiplus::WrapModeTile) ;
			gph.DrawImage(
				pImage,
				Rect(nXDest, nYDest + nFixedFront, nWidthDest, nHeightDest - nFixedFront - nFixedBack),
				nXSrc, nYSrc + nFixedFront, nWidthSrc, nHeightSrc - nFixedFront - nFixedBack - 1, UnitPixel, pImgAttr
				) ;
		}
		if (nFixedBack > 0)
		{
			gph.DrawImage(
				pImage,
				Rect(nXDest, nYDest + nHeightDest - nFixedBack, nWidthDest, nFixedBack),
				nXSrc, nYSrc + nHeightSrc - nFixedBack, nWidthSrc, nFixedBack, UnitPixel, pImgAttr
				) ;
		}
	}
}

// 面拉伸绘制
void SurfaceStretch(
					HDC hdcDest,
					int nXDest,
					int nYDest,
					int nWidthDest,
					int nHeightDest,
					Image *pImage,
					int nXSrc,
					int nYSrc,
					int nWidthSrc,
					int nHeightSrc,
					const ImageAttributes *imageAttributes,
					int nFixedLeft,		// = -1,	// 左端固定长度
					int nFixedRight,	// = -1,	// 右端固定长度
					int nFixedTop,		// = -1,	// 上端固定长度
					int nFixedBottom	// = -1		// 下端固定长度
					) 
{
	if (pImage == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (nFixedLeft == -1)
		nFixedLeft = nWidthSrc / 3 ;
	if (nFixedRight == -1)
		nFixedRight = nWidthSrc / 3 ;
	if (nFixedTop == -1)
		nFixedTop = nWidthSrc / 3 ;
	if (nFixedBottom == -1)
		nFixedBottom = nWidthSrc / 3 ;

	// 绘制上
	LineStretch(
		hdcDest,
		nXDest, nYDest, nWidthDest, nFixedTop,
		pImage,
		nXSrc, nYSrc, nWidthSrc, nFixedTop,
		imageAttributes,
		nFixedLeft, nFixedRight,
		TRUE
		) ;

	// 绘制下
	LineStretch(
		hdcDest,
		nXDest, nYDest + nHeightDest - nFixedBottom, nWidthDest, nFixedBottom,
		pImage,
		nXSrc, nYSrc + nHeightSrc - nFixedBottom, nWidthSrc, nFixedBottom,
		imageAttributes,
		nFixedLeft, nFixedRight,
		TRUE
		) ;

	Graphics gph(hdcDest) ;
	// 绘制左
	gph.DrawImage(
		pImage,
		Rect(nXDest, nYDest + nFixedTop, nFixedLeft, nHeightDest - nFixedTop - nFixedBottom),
		nXSrc, nYSrc + nFixedTop, nFixedLeft, nHeightSrc - nFixedTop - nFixedBottom, UnitPixel, imageAttributes
		) ;

	// 绘制中
	gph.DrawImage(
		pImage,
		Rect(nXDest + nFixedLeft, nYDest + nFixedTop, nWidthDest - nFixedLeft - nFixedRight, nHeightDest - nFixedTop - nFixedBottom),
		nXSrc, nYSrc + nFixedTop, nWidthSrc - nFixedLeft - nFixedRight, nHeightSrc - nFixedTop - nFixedBottom, UnitPixel, imageAttributes
		) ;

	// 绘制右
	gph.DrawImage(
		pImage,
		Rect(nXDest + nWidthDest - nFixedRight, nYDest + nFixedTop, nFixedRight, nHeightDest - nFixedTop - nFixedBottom),
		nXSrc + nWidthSrc - nFixedRight, nYSrc + nFixedTop, nFixedRight, nHeightSrc - nFixedTop - nFixedBottom, UnitPixel, imageAttributes
		) ;	
}

// 线平铺
void LineTile(
			  HDC hdcDest,
			  int nXDest,
			  int nYDest,
			  int nWidthDest,
			  int nHeightDest,
			  HDC hdcSrc,
			  int nXSrc,
			  int nYSrc,
			  int nWidthSrc,
			  int nHeightSrc,
			  int nFixedFront, //= -1,	// 前端固定长度
			  int nFixedBack, //= -1,	// 后端固定长度
			  BOOL bHorz //= TRUE		// 默认水平平铺
			  ) 
{
	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (bHorz)
	{
		if (nFixedFront == -1)
			nFixedFront = nWidthSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nWidthSrc / 3 ;

		if (nFixedFront > 0)
		{
			::BitBlt(
				hdcDest,
				nXDest, nYDest, nFixedFront, nHeightDest,
				hdcSrc,
				nXSrc, nYSrc, 
				SRCCOPY
				) ;
		}
		int nMiddleDest = nWidthDest - nFixedFront - nFixedBack ;
		int nMiddleSrc = nWidthSrc - nFixedFront - nFixedBack ;
		if (nMiddleDest > 0)
		{
			int nLeft = nXDest + nFixedFront ;
			int nRight = nXDest + nWidthDest - nFixedBack ;
			int nPiece = nMiddleSrc ;
			for ( ; ; )
			{
				if (nLeft >= nRight)
					break ;
				else if (nLeft + nMiddleSrc >= nRight)
					nPiece = nRight - nLeft ;

				::BitBlt(
					hdcDest,
					nLeft, nYDest, nPiece, nHeightDest,
					hdcSrc,
					nFixedFront, nYSrc,
					SRCCOPY
					) ;

				nLeft += nPiece ;
			}
		}
		if (nFixedBack > 0)
		{
			::BitBlt(
				hdcDest,
				nXDest + nWidthDest - nFixedBack, nYDest, nFixedBack, nHeightDest,
				hdcSrc,
				nXSrc + nWidthSrc - nFixedBack, nYSrc, 
				SRCCOPY
				) ;
		}
	}
	else
	{
		if (nFixedFront == -1)
			nFixedFront = nHeightSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nHeightSrc / 3 ;

		if (nFixedFront > 0)
		{
			::BitBlt(
				hdcDest,
				nXDest, nYDest, nWidthDest, nFixedFront,
				hdcSrc,
				nXSrc, nYSrc, 
				SRCCOPY
				) ;
		}
		int nMiddleDest = nHeightDest - nFixedFront - nFixedBack ;
		int nMiddleSrc = nHeightSrc - nFixedFront - nFixedBack ;
		if (nMiddleDest > 0)
		{
			int nTop = nYDest + nFixedFront ;
			int nBottom = nYDest + nHeightDest - nFixedBack ;
			int nPiece = nMiddleSrc ;
			for ( ; ; )
			{
				if (nTop >= nBottom)
					break ;
				else if (nTop + nMiddleSrc >= nBottom)
					nPiece = nBottom - nTop ;

				::BitBlt(
					hdcDest,
					nXDest, nTop, nWidthDest, nPiece,
					hdcSrc,
					nXSrc, nFixedFront,
					SRCCOPY
					) ;

				nTop += nPiece ;
			}
		}
		if (nFixedBack > 0)
		{
			::BitBlt(
				hdcDest,
				nXDest, nYDest + nHeightDest - nFixedBack, nWidthDest, nFixedBack,
				hdcSrc,
				nXSrc, nYSrc + nHeightSrc - nFixedBack, 
				SRCCOPY
				) ;
		}
	}
}

// 线平铺，带Alpha通道
void AlphaLineTile(
				   HDC hdcDest,
				   int nXDest,
				   int nYDest,
				   int nWidthDest,
				   int nHeightDest,
				   HDC hdcSrc,
				   int nXSrc,
				   int nYSrc,
				   int nWidthSrc,
				   int nHeightSrc,
				   int nFixedFront, //= -1,	// 前端固定长度
				   int nFixedBack, //= -1,	// 后端固定长度
				   BOOL bHorz //= TRUE		// 默认水平平铺
				   ) 
{
	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
	{
		ASSERT(FALSE) ;
		return ;
	}

	BLENDFUNCTION bfn ;
	bfn.BlendOp = AC_SRC_OVER ;
	bfn.BlendFlags = 0 ;
	bfn.SourceConstantAlpha = 255 ;
	bfn.AlphaFormat = AC_SRC_ALPHA ;

	if (bHorz)
	{
		if (nFixedFront == -1)
			nFixedFront = nWidthSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nWidthSrc / 3 ;

		if (nFixedFront > 0)
		{
			::AlphaBlend(
				hdcDest,
				nXDest, nYDest, nFixedFront, nHeightDest,
				hdcSrc,
				nXSrc, nYSrc, nFixedFront, nHeightDest,
				bfn
				) ;
		}
		int nMiddleDest = nWidthDest - nFixedFront - nFixedBack ;
		int nMiddleSrc = nWidthSrc - nFixedFront - nFixedBack ;
		if (nMiddleDest > 0)
		{
			int nLeft = nXDest + nFixedFront ;
			int nRight = nXDest + nWidthDest - nFixedBack ;
			int nPiece = nMiddleSrc ;
			for ( ; ; )
			{
				if (nLeft >= nRight)
					break ;
				else if (nLeft + nMiddleSrc >= nRight)
					nPiece = nRight - nLeft ;

				::AlphaBlend(
					hdcDest,
					nLeft, nYDest, nPiece, nHeightDest,
					hdcSrc,
					nXSrc + nFixedFront, nYSrc, nPiece, nHeightDest,
					bfn
					) ;

				nLeft += nPiece ;
			}
		}
		if (nFixedBack > 0)
		{
			::AlphaBlend(
				hdcDest,
				nXDest + nWidthDest - nFixedBack, nYDest, nFixedBack, nHeightDest,
				hdcSrc,
				nXSrc + nWidthSrc - nFixedBack, nYSrc, nFixedBack, nHeightDest,
				bfn
				) ;
		}
	}
	else
	{
		if (nFixedFront == -1)
			nFixedFront = nHeightSrc / 3 ;
		if (nFixedBack == -1)
			nFixedBack = nHeightSrc / 3 ;

		if (nFixedFront > 0)
		{
			::AlphaBlend(
				hdcDest,
				nXDest, nYDest, nWidthDest, nFixedFront,
				hdcSrc,
				nXSrc, nYSrc, nWidthDest, nFixedFront,
				bfn
				) ;
		}
		int nMiddleDest = nHeightDest - nFixedFront - nFixedBack ;
		int nMiddleSrc = nHeightSrc - nFixedFront - nFixedBack ;
		if (nMiddleDest > 0)
		{
			int nTop = nYDest + nFixedFront ;
			int nBottom = nYDest + nHeightDest - nFixedBack ;
			int nPiece = nMiddleSrc ;
			for ( ; ; )
			{
				if (nTop >= nBottom)
					break ;
				else if (nTop + nMiddleSrc >= nBottom)
					nPiece = nBottom - nTop ;

				::AlphaBlend(
					hdcDest,
					nXDest, nTop, nWidthDest, nPiece,
					hdcSrc,
					nXSrc, nYSrc + nFixedFront, nWidthDest, nPiece,
					bfn
					) ;

				nTop += nPiece ;
			}
		}
		if (nFixedBack > 0)
		{
			::AlphaBlend(
				hdcDest,
				nXDest, nYDest + nHeightDest - nFixedBack, nWidthDest, nFixedBack,
				hdcSrc,
				nXSrc, nYSrc + nHeightSrc - nFixedBack, nWidthDest, nFixedBack,
				bfn
				) ;
		}
	}
}

// 面平铺，带Alpha通道
void AlphaSurfaceTile(
					  HDC hdcDest,
					  int nXDest,
					  int nYDest,
					  int nWidthDest,
					  int nHeightDest,
					  HDC hdcSrc,
					  int nXSrc,
					  int nYSrc,
					  int nWidthSrc,
					  int nHeightSrc,
					  BOOL bBorderMode,	// = FALSE,		// 是否只绘制边框
					  int nFixedLeft,	// = -1,		// 左端固定长度
					  int nFixedRight,	// = -1,		// 右端固定长度
					  int nFixedTop,	// = -1,		// 上端固定长度
					  int nFixedBottom	// = -1			// 下端固定长度
					  ) 
{
	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
	{
		ASSERT(FALSE) ;
		return ;
	}

	if (nFixedLeft == -1)
		nFixedLeft = nWidthSrc / 3 ;
	if (nFixedRight == -1)
		nFixedRight = nWidthSrc / 3 ;
	if (nFixedTop == -1)
		nFixedTop = nWidthSrc / 3 ;
	if (nFixedBottom == -1)
		nFixedBottom = nWidthSrc / 3 ;

	// 绘制上
	AlphaLineTile(
		hdcDest,
		nXDest, nYDest, nWidthDest, nFixedTop,
		hdcSrc,
		nXSrc, nYSrc, nWidthSrc, nFixedTop,
		nFixedLeft, nFixedRight,
		TRUE
		) ;

	// 绘制下
	AlphaLineTile(
		hdcDest,
		nXDest, nYDest + nHeightDest - nFixedBottom, nWidthDest, nFixedBottom,
		hdcSrc,
		nXSrc, nYSrc + nHeightSrc - nFixedBottom, nWidthSrc, nFixedBottom,
		nFixedLeft, nFixedRight,
		TRUE
		) ;

	// 绘制左
	AlphaLineTile(
		hdcDest,
		nXDest, nYDest + nFixedTop, nFixedLeft, nHeightDest - nFixedTop - nFixedBottom,
		hdcSrc,
		nXSrc, nYSrc + nFixedTop, nFixedLeft, nHeightSrc - nFixedTop - nFixedBottom,
		0, 0, FALSE
		) ;

	if (!bBorderMode)
	{
		// 绘制中
		int nMiddleHorz = nWidthSrc - nFixedLeft - nFixedRight ;
		int nMiddleVert = nHeightSrc - nFixedTop - nFixedBottom ;

		int nLeftSrc = nXSrc + nFixedLeft ;
		int nTopSrc = nYSrc + nFixedTop ;

		int nLeftDest = nXDest + nFixedLeft ;
		int nTopDest = nYDest + nFixedTop ;
		int nBottomDest = nYDest + nHeightDest - nFixedBottom ;

		int nLineHeight = nMiddleVert ;
		for ( ; nTopDest < nBottomDest ; )
		{
			if (nTopDest + nLineHeight > nBottomDest)
				nLineHeight = nBottomDest - nTopDest ;

			AlphaLineTile(
				hdcDest,
				nLeftDest, nTopDest, nWidthDest - nFixedLeft - nFixedRight, nLineHeight,
				hdcSrc,
				nLeftSrc, nTopSrc, nWidthSrc - nFixedLeft - nFixedRight, nLineHeight,
				0, 0, TRUE
				) ;

			nTopDest += nLineHeight ;
		}
	}

	// 绘制右
	AlphaLineTile(
		hdcDest,
		nXDest + nWidthDest - nFixedRight, nYDest + nFixedTop, nFixedRight, nHeightDest - nFixedTop - nFixedBottom,
		hdcSrc,
		nXSrc + nWidthSrc - nFixedRight, nYSrc + nFixedTop, nFixedRight, nHeightSrc - nFixedTop - nFixedBottom,
		0, 0, FALSE
		) ;
}

// 模糊化位图
void Blur(HBITMAP hBitmap) 
{
	BITMAP bi ;
	::GetObject(hBitmap, sizeof(bi), &bi) ;

	// 只处理16位色和32为色位图
	if (bi.bmBitsPixel != 16 && bi.bmBitsPixel != 32)
		return ;

	DWORD dwCount = bi.bmWidthBytes * bi.bmHeight ;
	BYTE * pData = new BYTE[dwCount] ;
	BYTE * pNewData = new BYTE[dwCount] ;

	::GetBitmapBits(hBitmap, dwCount, pData) ;
	//bmp.GetBitmapBits(dwCount, pData) ;

	static int s_nSide = 3 ;
	static int s_arGlowMatrix[49] = 
	{
		1, 2, 3, 4, 3, 2, 1,
		2, 3, 4, 5, 4, 3, 2,
		3, 4, 5, 6, 5, 4, 3,
		4, 5, 6, 7, 6, 5, 4,
		3, 4, 5, 6, 5, 4, 3,
		2, 3, 4, 5, 4, 3, 2,
		1, 2, 3, 4, 3, 2, 1,
	} ;
	static int nMatrixLength = 49 ;
	int nTotalWeight = 0 ;
	int k ;
	for (k = 0 ; k < nMatrixLength ; ++ k)
	{
		nTotalWeight += s_arGlowMatrix[k] ;
	}

	// 开始处理
	if (bi.bmBitsPixel == 32)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				BYTE * pDst = pNewData + j * bi.bmWidthBytes + i * 4 ;

				DWORD dwRed = 0 ;
				DWORD dwGreen = 0 ;
				DWORD dwBlue = 0 ;
				DWORD dwGray = 0 ;
				int nWeight = 0 ;

				int m, n ;
				for (n = j - s_nSide ; n < j + s_nSide + 1 ; ++ n)
				{
					for (m = i - s_nSide ; m < i + s_nSide + 1 ; ++ m)
					{
						if (n < 0 || n >= bi.bmHeight || m < 0 || m >= bi.bmWidth)
							continue ;

						BYTE * pSrc = pData + n * bi.bmWidthBytes + m * 4 ;

						int w = s_arGlowMatrix[(n - j + s_nSide) * 7 + (m - i + s_nSide)] ;
						nWeight += w ;
						dwBlue += pSrc[0] * w ;
						dwGreen += pSrc[1] * w ;
						dwRed += pSrc[2] * w ;
					}
				}

				pDst[0] = BYTE(dwBlue / nWeight) ;
				pDst[1] = BYTE(dwGreen / nWeight) ;
				pDst[2] = BYTE(dwRed / nWeight) ;
				pDst[3] = 0 ;
			}
		}
	}
	else if (bi.bmBitsPixel == 16)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				unsigned short * pDst = (unsigned short *)(pNewData + j * bi.bmWidthBytes + i * 2) ;

				DWORD dwRed = 0 ;
				DWORD dwGreen = 0 ;
				DWORD dwBlue = 0 ;
				DWORD dwGray = 0 ;
				int nWeight = 0 ;

				int m, n ;
				for (n = j - s_nSide ; n < j + s_nSide + 1 ; ++ n)
				{
					for (m = i - s_nSide ; m < i + s_nSide + 1 ; ++ m)
					{
						if (n < 0 || n >= bi.bmHeight || m < 0 || m >= bi.bmWidth)
							continue ;

						BYTE * pSrc = pData + n * bi.bmWidthBytes + m * 2 ;
						unsigned short usPixel = *(unsigned short*)pSrc ;
						BYTE cbRed = (BYTE)((usPixel & 0xF800) >> 8) ;
						BYTE cbGreen = (BYTE)((usPixel & 0x7E0) >> 3) ;
						BYTE cbBlue = (BYTE)((usPixel & 0x1F) << 3) ;

						int w = s_arGlowMatrix[(n - j + s_nSide) * 7 + (m - i + s_nSide)] ;
						nWeight += w ;
						dwBlue += cbBlue * w ;
						dwGreen += cbGreen * w ;
						dwRed += cbRed * w ;
					}
				}

				unsigned short usBlue = unsigned short(dwBlue / nWeight) ;
				unsigned short usGreen = unsigned short(dwGreen / nWeight) ;
				unsigned short usRed = unsigned short(dwRed / nWeight) ;
				*pDst = ((usRed & 0xF8) << 8) | ((usGreen & 0xFC) << 3) | ((usBlue >> 3)) ;
			}
		}
	}
	SetBitmapBits(hBitmap, dwCount, pNewData) ;

	delete [] pNewData ;
	delete [] pData ;
}

// 创建32位设备无关位图
HBITMAP Create32Bitmap(int nWidth, int nHeight) 
{
	BYTE                      * pBits ;   
	BITMAPINFOHEADER          bmih;   
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER));   

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ;   
	bmih.biWidth                = nWidth;   
	bmih.biHeight               = nHeight;   
	bmih.biPlanes               = 1 ;   
	bmih.biBitCount             = 32;        //这里一定要是32   
	bmih.biCompression          = BI_RGB ;   
	bmih.biSizeImage            = 0 ;   
	bmih.biXPelsPerMeter        = 0 ;   
	bmih.biYPelsPerMeter        = 0 ;   
	bmih.biClrUsed              = 0 ;   
	bmih.biClrImportant         = 0 ;   

	return CreateDIBSection(
		NULL,
		(BITMAPINFO *)&bmih,
		DIB_RGB_COLORS,
		(VOID**)&pBits,
		NULL,
		0
		) ;
}

// 创建尽量兼容的32位位图
HBITMAP CreateCompatible32Bitmap(HDC hDC, int nWidth, int nHeight) 
{
	if ((GetDeviceCaps(hDC, BITSPIXEL) == 32))
	{
		return CreateCompatibleBitmap(hDC, nWidth, nHeight) ;
	}
	else
	{
		return Create32Bitmap(nWidth, nHeight) ;
	}
}

// 将Image转换成位图
HBITMAP ConvertImage2Bitmap(HDC hDC, Image * pImage) 
{
	if (pImage == NULL && pImage->GetLastStatus() != Ok)
		return NULL ;

	int nWidth = pImage->GetWidth() ;
	int nHeight = pImage->GetHeight() ;

	HDC hMemDC = ::CreateCompatibleDC(hDC) ;
	HBITMAP hMemBmp = CreateCompatible32Bitmap(hDC, nWidth, nHeight) ;
	HGDIOBJ hMemOldBmp = ::SelectObject(hMemDC, hMemBmp) ;

	{
		Graphics gph(hMemDC) ;
		gph.DrawImage(
			pImage,
			Rect(0, 0, nWidth, nHeight),
			0, 0, nWidth, nHeight, UnitPixel, NULL
			) ;
	}

	::SelectObject(hMemDC, hMemOldBmp) ;
	::DeleteDC(hMemDC) ;

	return hMemBmp ;
}

// 无变形绘制图片
void DrawImageIndeformable(Graphics &gph, Image * pImage, CRect const &rcDest, ImageAttributes * ia /* = NULL */) 
{
	if (pImage == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}
	DrawImageIndeformable(gph, pImage, rcDest, 0, 0, pImage->GetWidth(), pImage->GetHeight(), ia) ;
}

void DrawImageIndeformable(
						   Graphics &gph,
						   Image * pImage,
						   CRect const &rcDest,
						   int nLeft, int nTop, int nWidth, int nHeight,
                           ImageAttributes * ia /* = NULL */
						   ) 
{
	// 图像实际宽高
	int nImageWidth = nWidth ;
	int nImageHeight = nHeight ;

	// 预览区域宽高
	int nPreViewWidth = rcDest.Width() ;
	int nPreViewHeight = rcDest.Height() ;

	// 图像预览宽高
	int nImagePreViewWidth ;
	int nImagePreViewHeight ;

	// 判断预览控件是否容纳得下表情图片
	if (nImageWidth <= nPreViewWidth && nImageHeight <= nPreViewHeight)
	{
		nImagePreViewWidth = nImageWidth ;
		nImagePreViewHeight = nImageHeight ;
	}
	else
	{
		if (nImageWidth < nImageHeight)
		{
			nImagePreViewWidth = nImageWidth * nPreViewHeight / nImageHeight ;
			nImagePreViewHeight = nPreViewHeight ;
		}
		else
		{
			nImagePreViewWidth = nPreViewWidth ;
			nImagePreViewHeight = nImageHeight * nPreViewWidth / nImageWidth ;
		}
	}

	int nImagePreViewPosX = (nPreViewWidth - nImagePreViewWidth) / 2 ;
	int nImagePreViewPosY = (nPreViewHeight - nImagePreViewHeight) / 2 ;

	gph.DrawImage(
		pImage,
		Rect(rcDest.left + nImagePreViewPosX, rcDest.top + nImagePreViewPosY, nImagePreViewWidth, nImagePreViewHeight),
		nLeft, nTop, nImageWidth, nImageHeight, UnitPixel, ia
		);
}

//绘制带光晕的文字
int DrawText( HDC hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT format, COLORREF shadowColor )
	{
		int width =  lprc->right - lprc->left + 8;
		int height = lprc->bottom - lprc->top + 8;

		HDC hMemDC = ::CreateCompatibleDC(hdc) ;
		HBITMAP hBmp = CreateCompatibleBitmap(hdc, width, height);

		HGDIOBJ hOldMemBmp = SelectObject(hMemDC, hBmp);
		int nOldBkMode = ::SetBkMode(hMemDC, TRANSPARENT) ;
		COLORREF color = ::SetTextColor(hMemDC, RGB(255,255,255));			

		HFONT hFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		::SelectObject(hMemDC, hFont);


		BITMAP bi ;
		::GetObject(hBmp, sizeof(bi), &bi) ;
		DWORD dwCount = width * height * 4;
		BYTE * pData = new BYTE[dwCount] ;

		if(bi.bmBitsPixel == 32)
		{
			CRect rect(4, 4, width-4, height-4);
			::DrawText(hMemDC, lpchText, cchText, rect, format) ;
			::SetTextColor(hMemDC, color);
			Blur(hBmp);

			::GetBitmapBits(hBmp, dwCount, pData) ;
			int i, j ;
			for (j = 0 ; j < height ; ++ j)
			{
				for (i = 0 ; i < width ; ++ i)
				{
					BYTE * pDst = pData + j * bi.bmWidthBytes + i * 4 ;
					pDst[3] = (pDst[0] + pDst[1] + pDst[2])/3 == 0 ? 1 : (pDst[0] + pDst[1] + pDst[2])/4;

					pDst[0] = GetRValue(shadowColor) * pDst[3] / 255 ;
					pDst[1] = GetGValue(shadowColor) * pDst[3] / 255 ;
					pDst[2] = GetBValue(shadowColor) * pDst[3] / 255 ;
				}
			}
			::SetBitmapBits(hBmp, dwCount, pData) ;


			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER; 
			bf.BlendFlags = 0;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.SourceConstantAlpha = 255; 
			::AlphaBlend(
				hdc,
				lprc->left-4, lprc->top-4, width, height,
				hMemDC,
				0, 0, width, height,
				bf);
		}

		::SetBkMode(hMemDC, nOldBkMode) ;
		::SelectObject(hMemDC, hOldMemBmp) ;
		::DeleteObject(hBmp);
		::DeleteDC(hMemDC) ;

		delete pData;

		return ::DrawText(hdc, lpchText, cchText, lprc, format) ;
	}


// 使位图不透明化
BOOL Opaque32Bitmap(HBITMAP hBmp, COLORREF crInit /*= RGB(0, 0, 0)*/)  
{
	BITMAP bi ;
	if (0 == ::GetObject(hBmp, sizeof(bi), &bi))
		return FALSE ;

	// 使每个像素的alpha值为255
	DWORD dwCount = bi.bmWidthBytes * bi.bmHeight ;
	BYTE * pData = new BYTE[dwCount] ;

	if (0 == ::GetBitmapBits(hBmp, dwCount, pData))
	{
		delete [] pData ;
		return FALSE ;
	}

	// 	memset(pData, 255, dwCount) ;
	BYTE cbRed = GetRValue(crInit) ;
	BYTE cbGreen = GetGValue(crInit) ;
	BYTE cbBlue = GetBValue(crInit) ;

	int i, j ;
	for (j = 0 ; j < bi.bmHeight ; ++ j)
	{
		for (i = 0 ; i < bi.bmWidth ; ++ i)
		{
			pData[j * bi.bmWidthBytes + i * 4 + 0] = cbBlue ;
			pData[j * bi.bmWidthBytes + i * 4 + 1] = cbGreen ;
			pData[j * bi.bmWidthBytes + i * 4 + 2] = cbRed ;
			pData[j * bi.bmWidthBytes + i * 4 + 3] = 255 ;
		}
	}

	::SetBitmapBits(hBmp, dwCount, pData) ;

	delete [] pData ;
	return TRUE ;
}

BYTE GetRoughGrayScale(HDC hdc, LPRECT lprc)
{
	RECT rcdc ;
	::GetClipBox(hdc, &rcdc) ;
	if (lprc == NULL)
	{
		ASSERT(FALSE) ;
		return 0 ;
	}

	int nWidth = lprc->right - lprc->left ;
	int nHeight = lprc->bottom - lprc->top ;

	if (nWidth > rcdc.right - rcdc.left)
		nWidth = rcdc.right - rcdc.left ;
	if (nHeight > rcdc.bottom - rcdc.top)
		nHeight = rcdc.bottom - rcdc.top ;

	if (nWidth <= 0 || nHeight <= 0)
	{
		return 0 ;
	}

	HDC dcMem = ::CreateCompatibleDC(hdc) ;
	HBITMAP bmpMem = ::CreateCompatibleBitmap(hdc, nWidth, nHeight) ;
	HGDIOBJ bmpOld = ::SelectObject(dcMem, bmpMem) ;
	BitBlt(dcMem, 0, 0, nWidth, nHeight, hdc, lprc->left, lprc->top, SRCCOPY) ;
	::SelectObject(dcMem, bmpOld) ;
	::DeleteDC(dcMem) ;

	BITMAP bi ;
	::GetObject(bmpMem, sizeof(bi), &bi) ;
	DWORD cntPixel = bi.bmHeight * bi.bmWidthBytes ;
	BYTE * pData = new BYTE[cntPixel] ;
	::GetBitmapBits(bmpMem, cntPixel, pData) ;

	DWORD dwRes = 0 ;
	DWORD cntTruePixel = 0 ;

	if (bi.bmBitsPixel == 32)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth; ++ i)
			{
				BYTE * pTemp = pData + j * bi.bmWidthBytes + i * 4 ;
				dwRes += DWORD(0.11 * pTemp[0]) ;	// blue
				dwRes += DWORD(0.59 * pTemp[1]) ;	// green
				dwRes += DWORD(0.30 * pTemp[2]) ;	// red

				++ cntTruePixel ;
			}
		}
	}
	else if (bi.bmBitsPixel == 16)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth; ++ i)
			{
				unsigned short * pTemp = (unsigned short *)(pData + j * bi.bmWidthBytes + i * 2) ;
				unsigned short usPixel = *(unsigned short*)pTemp ;
				BYTE cbRed = (BYTE)((usPixel & 0xF800) >> 8) ;
				BYTE cbGreen = (BYTE)((usPixel & 0x7E0) >> 3) ;
				BYTE cbBlue = (BYTE)((usPixel & 0x1F) << 3) ;
				dwRes += DWORD(0.11 * cbRed) ;
				dwRes += DWORD(0.59 * cbGreen) ;
				dwRes += DWORD(0.30 * cbBlue) ;

				++ cntTruePixel ;
			}
		}
	}
	delete [] pData ;

	::DeleteObject(bmpMem) ;

	return cntTruePixel == 0 ? 0 : (BYTE)(dwRes / (/*3 * */cntTruePixel)) ;
}

void DrawGlowText(HDC hdc, LPCTSTR lpszText, int cchText, LPRECT lprc, UINT format)
{
	if (lpszText == NULL || lprc == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}
	if (cchText == 0)
		return ;

	UINT uFormatPos = (format & 0xf) ;
	UINT uFormatNoPos = (format & ~0xf) ;

	// 计算文字所占范围并校正其位置
	RECT rcText = {lprc->left, lprc->top, lprc->right, lprc->bottom} ;
	::DrawText(hdc, lpszText, cchText, &rcText, uFormatNoPos | DT_CALCRECT) ;

	int nTextWidth = rcText.right - rcText.left ;
	int nTextHeight = rcText.bottom - rcText.top ;

	if (uFormatPos & DT_CENTER)
	{
		rcText.left = (lprc->left + lprc->right - nTextWidth) / 2 ;
		rcText.right = rcText.left + nTextWidth ;
	}
	else if (uFormatPos & DT_RIGHT)
	{
		rcText.left = lprc->right - nTextWidth ;
		rcText.right = lprc->right ;
	}

	if (uFormatPos & DT_VCENTER)
	{
		rcText.top = (lprc->top + lprc->bottom - nTextHeight) / 2 ;
		rcText.bottom = rcText.top + nTextHeight ;
	}
	else if (uFormatPos & DT_BOTTOM)
	{
		rcText.top = lprc->bottom - nTextHeight ;
		rcText.bottom = lprc->bottom ;
	}

	int nHaloWidth = 3 ;	// 光晕宽度
	rcText.left -= nHaloWidth ;
	rcText.top -= nHaloWidth ;
	rcText.right += nHaloWidth ;
	rcText.bottom += nHaloWidth ;

	BYTE cbBkgnd = 0 ;
	cbBkgnd = GetRoughGrayScale(hdc, &rcText) ;

	// 根据当前背景颜色确定文字颜色
	COLORREF crText ;
	BYTE cbGlow = 0 ;
	if (cbBkgnd < 128)
	{
		cbGlow = 80 ;
		crText = RGB(255, 255, 255) ;
	}
	else
	{
		cbGlow = 233 ;
		crText = RGB(0, 0, 0) ;
	}

	COLORREF crBkgnd = RGB(cbBkgnd, cbBkgnd, cbBkgnd) ;

	// 创建兼容dc和32位位图
	HDC dcMem = ::CreateCompatibleDC(hdc) ;
	HBITMAP bmpMem = Create32Bitmap(rcText.right - rcText.left, rcText.bottom - rcText.top) ;
	if (!Opaque32Bitmap(bmpMem, crBkgnd))
	{
		::DeleteObject(bmpMem) ;
		::DeleteDC(dcMem) ;

		return ;
	}

	HGDIOBJ bmpMemOld = ::SelectObject(dcMem, bmpMem) ;

	// 字体和字体颜色与hdc保持一致
	HGDIOBJ ft = ::GetCurrentObject(hdc, OBJ_FONT) ;
	HGDIOBJ ftMemOld = ::SelectObject(dcMem, ft) ;

	//COLORREF crText = ::GetTextColor(hdc) ;
	::SetTextColor(dcMem, crText) ;
	::SetBkMode(dcMem, TRANSPARENT) ;

	// 绘制文字
	RECT rcMemText = {nHaloWidth, nHaloWidth, rcText.right - rcText.left - nHaloWidth, rcText.bottom - rcText.top - nHaloWidth} ;
	::DrawText(dcMem, lpszText, cchText, &rcMemText, uFormatNoPos) ;

	//////////////////////////////////////////////////////////////////////////
	// 开始处理
	::SelectObject(dcMem, bmpMemOld) ;
	{
		BITMAP bi ;
		if (0 == ::GetObject(bmpMem, sizeof(bi), &bi))
			goto EXIT0 ;

		DWORD dwPixelCount = bi.bmWidthBytes * bi.bmHeight ;
		BYTE * pSrcData = new BYTE[dwPixelCount] ;
		if (0 == ::GetBitmapBits(bmpMem, dwPixelCount, pSrcData))
		{
			delete [] pSrcData ;
			goto EXIT0 ;
		}

		BYTE * pDstData = new BYTE[dwPixelCount] ;

		int nTotalWeight = 0 ;
		int k ;
		for (k = 0 ; k < 49 ; ++ k)
		{
			nTotalWeight += s_arGlowMatrix[k] ;
		}

		// 开始处理
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				BYTE * pDstTemp = pDstData + j * bi.bmWidthBytes + i * 4 ;
				BYTE * pSrcTemp = pSrcData + j * bi.bmWidthBytes + i * 4 ;
// 				if (pSrcTemp[3] == 0)	// 文字区域
// 				{
// 					// 复制文字颜色并使之不透明
// 					pDstTemp[0] = pSrcTemp[0] ;
// 					pDstTemp[1] = pSrcTemp[1] ;
// 					pDstTemp[2] = pSrcTemp[2] ;
// 					pDstTemp[3] = 255 ;
// 				}
// 				else
				{
					int nWeight = 0 ;		// 权重
					int m, n ;
					for (n = j - nHaloWidth ; n < j + nHaloWidth + 1 ; ++ n)
					{
						for (m = i - nHaloWidth ; m < i + nHaloWidth + 1 ; ++ m)
						{
							int nPiexlWeight = 0 ;
							if (n < 0 || n >= bi.bmHeight || m < 0 || m >= bi.bmWidth)
							{
								nPiexlWeight = 255 ;
							}
							else
							{
								BYTE * pSrcTemp2 = pSrcData + n * bi.bmWidthBytes + m * 4 ;
								nPiexlWeight = pSrcTemp2[3] ;
							}

							nWeight += s_arGlowMatrix[(n - j + nHaloWidth) * 7 + (m - i + nHaloWidth)] * nPiexlWeight ;
						}
					}
					nWeight /= nTotalWeight ;

					BYTE cbAlpha = 255 - (BYTE)nWeight ;
					pDstTemp[0] = cbAlpha * cbGlow / 255 ;
					pDstTemp[1] = cbAlpha * cbGlow / 255 ;
					pDstTemp[2] = cbAlpha * cbGlow / 255 ;
// 					pDstTemp[0] = cbAlpha * pSrcTemp[0] / 255 ;
// 					pDstTemp[1] = cbAlpha * pSrcTemp[1] / 255 ;
// 					pDstTemp[2] = cbAlpha * pSrcTemp[2] / 255 ;
					pDstTemp[3] = cbAlpha ;
				}
			}
		}
		::SetBitmapBits(bmpMem, dwPixelCount, pDstData) ;
		delete [] pDstData ;
		delete [] pSrcData ;

		bmpMemOld = ::SelectObject(dcMem, bmpMem) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// 校正位置
	BLENDFUNCTION bf ;
	bf.BlendOp = AC_SRC_OVER ; 
	bf.BlendFlags = 0 ;
	bf.AlphaFormat = AC_SRC_ALPHA ;
	bf.SourceConstantAlpha = 255 ; 
	AlphaBlend(
		hdc,
		rcText.left, rcText.top, rcText.right - rcText.left, rcText.bottom - rcText.top,
		dcMem,
		0, 0, rcText.right - rcText.left, rcText.bottom - rcText.top,
		bf
		) ;

	//////////////////////////////////////////////////////////////////////////
	// 还原dcMem(颜色无需还原)
EXIT0:
	::SelectObject(dcMem, ftMemOld) ;
	::SelectObject(dcMem, bmpMemOld) ;
	::DeleteObject(bmpMem) ;
	::DeleteDC(dcMem) ;

	COLORREF crOldText = ::SetTextColor(hdc, crText) ;
	int nOldMode = ::SetBkMode(hdc, TRANSPARENT) ;
	::DrawText(hdc, lpszText, cchText, lprc, format) ;
	::SetBkMode(hdc, nOldMode) ;
	::SetTextColor(hdc, crOldText) ;
}

void DrawGlowText(HDC hdc, LPCTSTR lpszText, int cchText, LPRECT lprc, UINT format, COLORREF crHalo, COLORREF crBkgnd)
{
	if (lpszText == NULL || lprc == NULL)
	{
		ASSERT(FALSE) ;
		return ;
	}
	if (cchText == 0)
		return ;

	UINT uFormatPos = (format & 0xf) ;
	UINT uFormatNoPos = (format & ~0xf) ;

	// 计算文字所占范围并校正其位置
	RECT rcText = {lprc->left, lprc->top, lprc->right, lprc->bottom} ;
	::DrawText(hdc, lpszText, cchText, &rcText, uFormatNoPos | DT_CALCRECT) ;

	int nTextWidth = rcText.right - rcText.left ;
	int nTextHeight = rcText.bottom - rcText.top ;

	if (uFormatPos & DT_CENTER)
	{
		rcText.left = (lprc->left + lprc->right - nTextWidth) / 2 ;
		rcText.right = rcText.left + nTextWidth ;
	}
	else if (uFormatPos & DT_RIGHT)
	{
		rcText.left = lprc->right - nTextWidth ;
		rcText.right = lprc->right ;
	}

	if (uFormatPos & DT_VCENTER)
	{
		rcText.top = (lprc->top + lprc->bottom - nTextHeight) / 2 ;
		rcText.bottom = rcText.top + nTextHeight ;
	}
	else if (uFormatPos & DT_BOTTOM)
	{
		rcText.top = lprc->bottom - nTextHeight ;
		rcText.bottom = lprc->bottom ;
	}

	int nHaloWidth = 3 ;	// 光晕宽度
	rcText.left -= nHaloWidth ;
	rcText.top -= nHaloWidth ;
	rcText.right += nHaloWidth ;
	rcText.bottom += nHaloWidth ;

	//COLORREF crBkgnd = crHalo ;
	//crBkgnd = crHalo ;

	BOOL black_bkgnd = (GetRValue(crBkgnd) < 128) ;
/*	if (black_bkgnd)
	{
		crBkgnd = RGB(0, 0, 0) ;
	}
	else
	{
		crBkgnd = RGB(255, 255, 255) ;
	}*/

	// 创建兼容dc和32位位图
	HDC dcMem = ::CreateCompatibleDC(hdc) ;
	HBITMAP bmpMem = Create32Bitmap(rcText.right - rcText.left, rcText.bottom - rcText.top) ;
	if (!Opaque32Bitmap(bmpMem, crBkgnd))
	{
		::DeleteObject(bmpMem) ;
		::DeleteDC(dcMem) ;

		return ;
	}

	HGDIOBJ bmpMemOld = ::SelectObject(dcMem, bmpMem) ;

	// 字体和字体颜色与hdc保持一致
	HGDIOBJ ft = ::GetCurrentObject(hdc, OBJ_FONT) ;
	HGDIOBJ ftMemOld = ::SelectObject(dcMem, ft) ;

	COLORREF crText = ::GetTextColor(hdc) ;
	//COLORREF crText = black_bkgnd ? RGB(255, 255, 255) : RGB(0, 0, 0) ;
	::SetTextColor(dcMem, crText) ;
	::SetBkMode(dcMem, TRANSPARENT) ;

	// 绘制文字
	RECT rcMemText = {nHaloWidth, nHaloWidth, rcText.right - rcText.left - nHaloWidth, rcText.bottom - rcText.top - nHaloWidth} ;
	::DrawText(dcMem, lpszText, cchText, &rcMemText, uFormatNoPos) ;

	//////////////////////////////////////////////////////////////////////////
	// 开始处理
	::SelectObject(dcMem, bmpMemOld) ;
	{
		BITMAP bi ;
		if (0 == ::GetObject(bmpMem, sizeof(bi), &bi))
			goto EXIT0 ;

		DWORD dwPixelCount = bi.bmWidthBytes * bi.bmHeight ;
		BYTE * pSrcData = new BYTE[dwPixelCount] ;
		if (0 == ::GetBitmapBits(bmpMem, dwPixelCount, pSrcData))
		{
			delete [] pSrcData ;
			goto EXIT0 ;
		}

		BYTE * pDstData = new BYTE[dwPixelCount] ;

		int nTotalWeight = 0 ;
		int k ;
		for (k = 0 ; k < 49 ; ++ k)
		{
			nTotalWeight += s_arGlowMatrix[k] ;
		}

		// 开始处理
		int i, j ;
		BYTE cbRed = GetRValue(crHalo) ;
		BYTE cbGreen = GetGValue(crHalo) ;
		BYTE cbBlue = GetBValue(crHalo) ;

		BYTE text_red = GetRValue(crText) ;
		BYTE text_green = GetGValue(crText) ;
		BYTE text_blue = GetBValue(crText) ;

		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				BYTE * pDstTemp = pDstData + j * bi.bmWidthBytes + i * 4 ;
				BYTE * pSrcTemp = pSrcData + j * bi.bmWidthBytes + i * 4 ;
/*				if (pSrcTemp[3] == 0)	// 文字区域
				{
					// 复制文字颜色并使之不透明
// 					pDstTemp[0] = (pSrcTemp[0] + text_blue + cbBlue) / 3 ;
// 					pDstTemp[1] = (pSrcTemp[1] + text_green + cbGreen) / 3 ;
// 					pDstTemp[2] = (pSrcTemp[2] + text_red + cbRed) / 3 ;

// 					pDstTemp[0] = (pSrcTemp[0] + text_blue) / 2 ;
// 					pDstTemp[1] = (pSrcTemp[1] + text_green) / 2 ;
// 					pDstTemp[2] = (pSrcTemp[2] + text_red) / 2 ;
//					pDstTemp[3] = 255 ;

					pDstTemp[0] = pSrcTemp[0] ;
					pDstTemp[1] = pSrcTemp[1] ;
					pDstTemp[2] = pSrcTemp[2] ;

					pDstTemp[3] = (pDstTemp[0] + pDstTemp[1] + pDstTemp[2]) * 255 / (text_blue + text_green + text_red) ;
				}
				else*/
				{
					int nWeight = 0 ;		// 权重
					int m, n ;
					for (n = j - nHaloWidth ; n < j + nHaloWidth + 1 ; ++ n)
					{
						for (m = i - nHaloWidth ; m < i + nHaloWidth + 1 ; ++ m)
						{
							int nPiexlWeight = 0 ;
							if (n < 0 || n >= bi.bmHeight || m < 0 || m >= bi.bmWidth)
							{
								nPiexlWeight = 255 ;
							}
							else
							{
								BYTE * pSrcTemp2 = pSrcData + n * bi.bmWidthBytes + m * 4 ;
								nPiexlWeight = pSrcTemp2[3] ;
							}

							nWeight += s_arGlowMatrix[(n - j + nHaloWidth) * 7 + (m - i + nHaloWidth)] * nPiexlWeight ;
						}
					}
					nWeight /= nTotalWeight ;

					BYTE cbAlpha = 255 - (BYTE)nWeight ;

/*					if (pSrcTemp[3] == 0)
					{
// 						pDstTemp[0] = cbAlpha * pSrcTemp[0] / 255 ;
// 						pDstTemp[1] = cbAlpha * pSrcTemp[1] / 255 ;
// 						pDstTemp[2] = cbAlpha * pSrcTemp[2] / 255 ;
// 						pDstTemp[3] = cbAlpha ;

						if (black_bkgnd)
						{
							BYTE max = pSrcTemp[0] ;
							if (max < pSrcTemp[1]) max = pSrcTemp[i + 1] ;
							if (max < pSrcTemp[2]) max = pSrcTemp[i + 2] ;

							BYTE alpha = max ;

							pDstTemp[0] = pSrcTemp[0] ;
							pDstTemp[1] = pSrcTemp[1] ;
							pDstTemp[2] = pSrcTemp[2] ;
							pDstTemp[3] = alpha ;
						}
						else
						{
							BYTE min = pSrcTemp[0] ;
							if (min > pSrcTemp[1]) min = pSrcTemp[1] ;
							if (min > pSrcTemp[2]) min = pSrcTemp[2] ;

							BYTE alpha = 255 - min ;

							pDstTemp[0] = pSrcTemp[0] - min ;
							pDstTemp[1] = pSrcTemp[1] - min ;
							pDstTemp[2] = pSrcTemp[2] - min ;
							pDstTemp[3] = alpha ;
						}
					}
					else*/
					{
						pDstTemp[0] = cbAlpha * cbBlue / 255 ;
						pDstTemp[1] = cbAlpha * cbGreen / 255 ;
						pDstTemp[2] = cbAlpha * cbRed / 255 ;
						pDstTemp[3] = cbAlpha ;
					}
				}
			}
		}
		::SetBitmapBits(bmpMem, dwPixelCount, pDstData) ;
		delete [] pDstData ;
		delete [] pSrcData ;

		bmpMemOld = ::SelectObject(dcMem, bmpMem) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// 校正位置
	BLENDFUNCTION bf ;
	bf.BlendOp = AC_SRC_OVER ; 
	bf.BlendFlags = 0 ;
	bf.AlphaFormat = AC_SRC_ALPHA ;
	bf.SourceConstantAlpha = 255 ; 
	AlphaBlend(
		hdc,
		rcText.left, rcText.top, rcText.right - rcText.left, rcText.bottom - rcText.top,
		dcMem,
		0, 0, rcText.right - rcText.left, rcText.bottom - rcText.top,
		bf
		) ;

	//////////////////////////////////////////////////////////////////////////
	// 还原dcMem(颜色无需还原)
EXIT0:
	::SelectObject(dcMem, ftMemOld) ;
	::SelectObject(dcMem, bmpMemOld) ;
	::DeleteObject(bmpMem) ;
	::DeleteDC(dcMem) ;

	COLORREF crOldText = ::SetTextColor(hdc, crText) ;
	int nOldMode = ::SetBkMode(hdc, TRANSPARENT) ;
	::DrawText(hdc, lpszText, cchText, lprc, format) ;
	::SetBkMode(hdc, nOldMode) ;
	::SetTextColor(hdc, crOldText) ;
}

// 粉刷位图
BOOL RenderBitmap(HBITMAP bmp, COLORREF color) 
{
	BITMAP bi ;
	if (0 == ::GetObject(bmp, sizeof(bi), &bi))
		return FALSE ;

	if (bi.bmBitsPixel != 16 && bi.bmBitsPixel != 32)
		return FALSE ;

	// 使每个像素的alpha值为255
	DWORD dwCount = bi.bmWidthBytes * bi.bmHeight ;
	BYTE * pData = new BYTE[dwCount] ;

	if (0 == ::GetBitmapBits(bmp, dwCount, pData))
	{
		delete [] pData ;
		return FALSE ;
	}

	BYTE cbRed = GetRValue(color) ;
	BYTE cbGreen = GetGValue(color) ;
	BYTE cbBlue = GetBValue(color) ;

	if (bi.bmBitsPixel == 16)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				unsigned short * pTemp = (unsigned short *)(pData + j * bi.bmWidthBytes + i * 2) ;
				* pTemp = ((cbRed >> 3) << 11) | ((cbGreen >> 2) << 5) | (cbBlue >> 3) ;
			}
		}
	}
	else if (bi.bmBitsPixel == 32)
	{
		int i, j ;
		for (j = 0 ; j < bi.bmHeight ; ++ j)
		{
			for (i = 0 ; i < bi.bmWidth ; ++ i)
			{
				pData[j * bi.bmWidthBytes + i * 4 + 0] = cbBlue ;
				pData[j * bi.bmWidthBytes + i * 4 + 1] = cbGreen ;
				pData[j * bi.bmWidthBytes + i * 4 + 2] = cbRed ;
				pData[j * bi.bmWidthBytes + i * 4 + 3] = 255 ;
			}
		}
	}

	::SetBitmapBits(bmp, dwCount, pData) ;

	delete [] pData ;
	return TRUE ;
}

BOOL SaveDIB2BmpFile(HGLOBAL dib, LPCTSTR file_path) 
{
    if (NULL == dib || NULL == file_path)
        return FALSE ;

    LPBITMAPINFOHEADER bih = (LPBITMAPINFOHEADER)::GlobalLock(dib) ;
    if (NULL == bih)
        return FALSE ;

    if (bih->biSize != sizeof(BITMAPINFOHEADER))
        goto ERROREXIT ;

    // 创建文件
    HANDLE file = ::CreateFile(
        file_path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        ) ;

    if (NULL == file)
        goto ERROREXIT ;

    BITMAPFILEHEADER bfh ;
    bfh.bfType = 'MB' ;
    bfh.bfSize ;
    bfh.bfReserved1 = 0 ;
    bfh.bfReserved2 = 0 ;
    bfh.bfOffBits ;

    // 计算颜色表的大小
    size_t color_table_size = 0 ;
    if (bih->biBitCount == 16 || bih->biBitCount == 32)
    {
        if (bih->biCompression == BI_BITFIELDS)
            color_table_size = 3 * sizeof(DWORD) ;
    }
    else 
    {
        if (bih->biBitCount < 1)
            bih->biBitCount = 1 ;
        else if (bih->biBitCount < 4)
            bih->biBitCount = 4 ;
        else if (bih->biBitCount < 8)
            bih->biBitCount = 8 ;
        else
            bih->biBitCount = 24 ;

        if (bih->biBitCount != 24)
            color_table_size = 1 << bih->biBitCount ;
    }

    // 计算图片文件的大小和图像数据偏移
    bfh.bfSize = sizeof(bfh) + bih->biSize + color_table_size + bih->biSizeImage ;
    bfh.bfOffBits = sizeof(bfh) + bih->biSize + color_table_size ;

    DWORD written = 0 ;
    WriteFile(file, &bfh, sizeof(bfh), &written, NULL) ;

    WriteFile(file, bih, bfh.bfSize - sizeof(bfh), &written, NULL) ;

    ::CloseHandle(file) ;
    ::GlobalUnlock(dib) ;
    return TRUE ;

ERROREXIT:
    ::GlobalUnlock(dib) ;
    return FALSE ;
}

HGLOBAL ConvertDDB2DIB(HBITMAP ddb) 
{
    if (NULL == ddb)
        return NULL ;

    HDC dc = ::GetDC(NULL) ;

    BITMAPINFO bi ;
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader) ;
    bi.bmiHeader.biBitCount = 0 ;
    if (ERROR_INVALID_PARAMETER == GetDIBits(dc, ddb, 0, 0, NULL, &bi, DIB_RGB_COLORS))
        return NULL ;

    // 根据当前获得的位图信息计算颜色表长度和数据长度
    size_t color_table_size = 0 ;
    if (bi.bmiHeader.biBitCount == 16 || bi.bmiHeader.biBitCount == 32)
    {
        if (bi.bmiHeader.biCompression == BI_BITFIELDS)
            color_table_size = 3 * sizeof(DWORD) ;
    }
    else 
    {
        if (bi.bmiHeader.biBitCount < 1)
            bi.bmiHeader.biBitCount = 1 ;
        else if (bi.bmiHeader.biBitCount < 4)
            bi.bmiHeader.biBitCount = 4 ;
        else if (bi.bmiHeader.biBitCount < 8)
            bi.bmiHeader.biBitCount = 8 ;
        else
            bi.bmiHeader.biBitCount = 24 ;

        if (bi.bmiHeader.biBitCount != 24)
            color_table_size = 1 << bi.bmiHeader.biBitCount ;
    }

    size_t dib_size = bi.bmiHeader.biSize + color_table_size + bi.bmiHeader.biSizeImage ; 
    HGLOBAL memory = ::GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, dib_size) ;
    BYTE * dib = (BYTE*)::GlobalLock(memory) ;

    memcpy(dib, &bi, sizeof(bi)) ;
    GetDIBits(dc, ddb, 0, bi.bmiHeader.biHeight, dib + bi.bmiHeader.biSize + color_table_size, (LPBITMAPINFO)dib, DIB_RGB_COLORS) ;
    ::ReleaseDC(NULL, dc) ;

    ::GlobalUnlock(memory) ;

    return memory ;
}

} ;
