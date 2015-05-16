//////////////////////////////////////////////////////////////////////////
//
//	描述:   图像处理通用函数
//
//	
//	日期:   2011/02/25
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "uilib_exports.h"

namespace hdutils {

extern COLORREF crTextColor ;

UILIB_EXPORT COLORREF GetTextColor();

// 单方向拉伸绘制
UILIB_EXPORT void LineStretch(
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
				 const ImageAttributes* pImgAttr = NULL,
				 int nFixedFront = -1,	// 前端固定长度
				 int nFixedBack = -1,	// 后端固定长度
				 BOOL bHorz = TRUE		// 默认水平拉伸
				 ) ;

// 面拉伸绘制
UILIB_EXPORT void SurfaceStretch(
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
					const ImageAttributes *imageAttributes = NULL,
					int nFixedLeft = -1,	// 左端固定长度
					int nFixedRight = -1,	// 右端固定长度
					int nFixedTop = -1,		// 上端固定长度
					int nFixedBottom = -1	// 下端固定长度
					) ;

// 线平铺
UILIB_EXPORT void LineTile(
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
			  int nFixedFront = -1,	// 前端固定长度
			  int nFixedBack = -1,	// 后端固定长度
			  BOOL bHorz = TRUE		// 默认水平平铺
			  ) ;

// 线平铺，带Alpha通道
UILIB_EXPORT void AlphaLineTile(
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
				   int nFixedFront = -1,	// 前端固定长度
				   int nFixedBack = -1,		// 后端固定长度
				   BOOL bHorz = TRUE		// 默认水平平铺
				   ) ;

// 面平铺，带Alpha通道
UILIB_EXPORT void AlphaSurfaceTile(
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
					  BOOL bBorderMode = FALSE,		// 是否只绘制边框
					  int nFixedLeft = -1,			// 左端固定长度
					  int nFixedRight = -1,			// 右端固定长度
					  int nFixedTop = -1,			// 上端固定长度
					  int nFixedBottom = -1			// 下端固定长度
					  ) ;

// 模糊化位图
UILIB_EXPORT void Blur(HBITMAP hBitmap) ;

// 创建32位设备无关位图
UILIB_EXPORT HBITMAP Create32Bitmap(int nWidth, int nHeight) ;

// 创建尽量兼容的32位位图
UILIB_EXPORT HBITMAP CreateCompatible32Bitmap(HDC hDC, int nWidth, int nHeight) ;

// 将Image转换成位图
UILIB_EXPORT HBITMAP ConvertImage2Bitmap(HDC hDC, Image * pImage) ;

// 将Image转换成画布DC
// BOOL ConvertImage2CanvasDC(CCanvasDC &dcDest, HDC hDC, Image * pImage) ;

// 无变形绘制图片
UILIB_EXPORT void DrawImageIndeformable(
						   Graphics &gph,
						   Image * pImage,
						   CRect const &rcDest,
                           ImageAttributes * ia = NULL
						   ) ;
UILIB_EXPORT void DrawImageIndeformable(
						   Graphics &gph,
						   Image * pImage,
						   CRect const &rcDest,
                           int nLeft, int nTop, int nWidth, int nHeight,
                           ImageAttributes * ia = NULL
						   ) ;
//绘制带光晕效果的文字
UILIB_EXPORT int DrawText( HDC hdc,
			 LPCTSTR lpchText,
			 int cchText,
			 LPRECT lprc,
			 UINT format,
			 COLORREF shadowColor
			 );

// 使32位位图不透明化
UILIB_EXPORT BOOL Opaque32Bitmap(HBITMAP hBmp, COLORREF crInit = RGB(0, 0, 0)) ;

// 获得背景平均灰度
UILIB_EXPORT BYTE GetRoughGrayScale(HDC hdc, LPRECT lprc) ;

// 绘制发光文字，能自动根据背景灰度
UILIB_EXPORT void DrawGlowText(HDC hdc, LPCTSTR lpszText, int cchText, LPRECT lprc, UINT format) ;

// 绘制发光文字
UILIB_EXPORT void DrawGlowText(HDC hdc, LPCTSTR lpszText, int cchText, LPRECT lprc, UINT format, COLORREF crHalo, COLORREF crBkgnd) ;

UILIB_EXPORT BOOL RenderBitmap(HBITMAP bmp, COLORREF color);

UILIB_EXPORT BOOL SaveDIB2BmpFile(HGLOBAL dib, LPCTSTR file_path) ;

UILIB_EXPORT HGLOBAL ConvertDDB2DIB(HBITMAP ddb) ;

} ;
