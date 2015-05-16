#include "StdAfx.h"
#include "WLSkinOption.h"

#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"
#include "common/utils/Graphics.h"

WLSkinOption::WLSkinOption(void)
: m_single_or_tile(TRUE)
, m_listener(NULL)
{
}

WLSkinOption::~WLSkinOption(void)
{
}

BOOL WLSkinOption::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
    if (!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
    {
        return FALSE;
    }
	InitBkgnd() ;
	InitImage() ;

	int width = m_img_selected_border->GetWidth() ;
	int gap = (cx - width * 2) / 3 ;

	if (!m_btn_tile_mode.Create(WS_VISIBLE | BS_AUTORADIOBUTTON, gap, 37, m_img_tile_mode->GetWidth(), m_img_tile_mode->GetHeight(), this, Id_Btn_TileMode))
		return FALSE ;

	if (!m_btn_single_mode.Create(WS_VISIBLE | BS_AUTORADIOBUTTON, gap + width + gap, 37, m_img_single_mode->GetWidth(), m_img_single_mode->GetHeight(), this, Id_Btn_SingleMode))
		return FALSE ;

	int btn_width = 51, btn_height = m_img_push_btn->GetHeight() ;

	if (!m_btn_ok.Create(WS_VISIBLE, cx - 10 - btn_width * 2 - 10, cy - 10 - btn_height, btn_width, btn_height, this, Id_Btn_OK))
		return FALSE ;
	m_btn_ok.SetWindowText(_T("确定")) ;

	if (!m_btn_cancel.Create(WS_VISIBLE, cx - 10 - btn_width, cy - 10 - btn_height, btn_width, btn_height, this, Id_Btn_Cancel))
		return FALSE ;
	m_btn_cancel.SetWindowText(_T("取消")) ;

	if (m_single_or_tile)
	{
		m_btn_single_mode.SetCheck(1) ;
		m_btn_tile_mode.SetCheck(0) ;
	}

	return TRUE ;
}

void WLSkinOption::InitBkgnd() 
{
	SIZE size ;
	GetSize(&size) ;

	Shared_ImageEx imgBorder = Shared_ImageEx::FromId(IDI_WND_FRAME_PNG) ;

	WLDC dc(this) ;
	CCanvasDC temp ;
	temp.CreateFromImage(dc, imgBorder, TRUE) ;

	m_bkgnd_dc.Create32BitDC(dc, size.cx, size.cy) ;

	// 填充背景
	{
		int title_height = 25 ;

		// 填充标题栏
		Graphics gph(m_bkgnd_dc) ;
		LinearGradientBrush lgb(Point(0, 3), Point(0, 3 + title_height), Color(144, 144, 144, 144), Color(191, 191, 191, 191)) ;
		gph.FillRectangle(&lgb, 3, 3, size.cx - 6, 3 + title_height) ;

		// 填充下背景
		SolidBrush sb(Color(204, 204, 204, 204)) ;
		gph.FillRectangle(&sb, 3, 3 + title_height, size.cx - 6, size.cy - 6 - title_height) ;
	}

	// 绘制边框
	hdutils::AlphaSurfaceTile(
		m_bkgnd_dc,
		0, 0, size.cx, size.cy,
		temp,
		0, 0, temp.GetWidth(), temp.GetHeight(),
		TRUE
		) ;

	// 绘制标题和提示文字
	{
		Graphics gph(m_bkgnd_dc) ;
		Gdiplus::FontFamily  fontFamily(L"宋体") ;
		Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel) ;
		SolidBrush sb(Color(254, 0, 0, 0)) ;

		CString title = _T("选择填充背景方式:") ;
		gph.DrawString(title, title.GetLength(), &font, PointF(8.0, 8.0), &sb) ;

		CString tip = _T("拖动主面板和底图试试") ;
		gph.DrawString(tip, tip.GetLength(), &font, PointF(10.0, 107.0), &sb) ;
	}
}

void WLSkinOption::InitImage() 
{
	m_img_tile_mode = Shared_ImageEx::FromId(IDI_SKIN_PREVIEW_TILE_MODE_PNG) ;
	m_img_single_mode = Shared_ImageEx::FromId(IDI_SKIN_PREVIEW_SINGLE_MODE_PNG) ;
	m_img_selected_border = Shared_ImageEx::FromId(IDI_SKIN_PREVIEW_SELECTED_BORDER_PNG) ;
	m_img_push_btn = Shared_ImageEx::FromId(IDI_PUSHBUTTON_PNG) ;
}

void WLSkinOption::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	BLENDFUNCTION bft ;
	bft.BlendOp = AC_SRC_OVER ;
	bft.BlendFlags = 0 ;
	bft.SourceConstantAlpha = 255 ;
	bft.AlphaFormat = AC_SRC_ALPHA ;

	RECT update_in_this ;
	::CopyRect(&update_in_this, &rcUpdate) ;
	HostToClient(&update_in_this) ;

	::AlphaBlend(
		hDC,
		rcUpdate.left, rcUpdate.top, rcUpdate.right - rcUpdate.left, rcUpdate.bottom - rcUpdate.top,
		m_bkgnd_dc,
		update_in_this.left, update_in_this.top, update_in_this.right - update_in_this.left, update_in_this.bottom - update_in_this.top,
		bft
		) ;
}

BOOL WLSkinOption::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
		consumed = TRUE ;
		break ;

	case WM_NOTIFY :
		{
			consumed = OnNotify(wParam, lParam, pResult) ;
		}
		break ;

	case WM_COMMAND :
		{
			consumed = OnCommand(wParam, lParam, pResult) ;
		}
		break ;
	}

	return consumed ;
}

BOOL WLSkinOption::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;
	switch (wParam)
	{
	case Id_Btn_TileMode :
		{
			if (pNMWLHDR->code == NM_CUSTOMDRAW)
			{
				WLButton::LPNMCDWLBTNINFO pInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

				{
					Graphics gph(pInfo->hDC) ;
					gph.DrawImage(
						m_img_tile_mode,
						Rect(pInfo->rc.left, pInfo->rc.top, pInfo->rc.right - pInfo->rc.left, pInfo->rc.bottom - pInfo->rc.top),
						0, 0, m_img_tile_mode->GetWidth(), m_img_tile_mode->GetHeight(), UnitPixel, NULL
						) ;

					if (pInfo->dwStatus & WLButton::BS_CHECK)
					{
						gph.DrawImage(
							m_img_selected_border,
							Rect(pInfo->rc.left, pInfo->rc.top, pInfo->rc.right - pInfo->rc.left, pInfo->rc.bottom - pInfo->rc.top),
							0, 0, m_img_tile_mode->GetWidth(), m_img_tile_mode->GetHeight(), UnitPixel, NULL
							) ;
					}
				}

				*pResult = 1 ;
				consumed = TRUE ;
			}
		}
		break ;

	case Id_Btn_SingleMode :
		{
			if (pNMWLHDR->code == NM_CUSTOMDRAW)
			{
				WLButton::LPNMCDWLBTNINFO pInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

				{
					Graphics gph(pInfo->hDC) ;
					gph.DrawImage(
						m_img_single_mode,
						Rect(pInfo->rc.left, pInfo->rc.top, pInfo->rc.right - pInfo->rc.left, pInfo->rc.bottom - pInfo->rc.top),
						0, 0, m_img_tile_mode->GetWidth(), m_img_tile_mode->GetHeight(), UnitPixel, NULL
						) ;

					if (pInfo->dwStatus & WLButton::BS_CHECK)
					{
						gph.DrawImage(
							m_img_selected_border,
							Rect(pInfo->rc.left, pInfo->rc.top, pInfo->rc.right - pInfo->rc.left, pInfo->rc.bottom - pInfo->rc.top),
							0, 0, m_img_tile_mode->GetWidth(), m_img_tile_mode->GetHeight(), UnitPixel, NULL
							) ;
					}
				}

				*pResult = 1 ;
				consumed = TRUE ;
			}
		}
		break ;

	case Id_Btn_OK :
	case Id_Btn_Cancel :
		{
			if (pNMWLHDR->code == NM_CUSTOMDRAW)
			{
				WLButton::LPNMCDWLBTNINFO pInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

				int img_unit_index = 0 ;
				if (pInfo->dwStatus & WLButton::BS_SELECTED)
				{
					img_unit_index = 2 ;
				}
				else if (pInfo->dwStatus & WLButton::BS_HOT)
				{
					img_unit_index = 1 ;
				}

				int nUnitWidth = m_img_push_btn->GetWidth() / 5 ;
				int nUnitHeight = m_img_push_btn->GetHeight() ;
				RECT &rc = pInfo->rc ;
				hdutils::LineStretch(
					pInfo->hDC,
					rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
					m_img_push_btn,
					img_unit_index * nUnitWidth, 0, nUnitWidth, nUnitHeight, NULL, 3, 3
					) ;

				{
					Graphics gph(pInfo->hDC) ;
					CString title ;
					pInfo->hdr.hWlFrom->GetWindowText(title) ;

					Gdiplus::FontFamily  fontFamily(L"宋体") ;
					Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel) ;
					RectF layout(
						(Gdiplus::REAL)pInfo->rc.left,
						(Gdiplus::REAL)pInfo->rc.top + 2, // UNDONE - Q，不知道为什么要这么微调才能垂直居中
						(Gdiplus::REAL)(pInfo->rc.right - pInfo->rc.left),
						(Gdiplus::REAL)(pInfo->rc.bottom - pInfo->rc.top)
						) ;
					StringFormat sf ;
					sf.SetAlignment( StringAlignmentCenter );
					sf.SetLineAlignment( StringAlignmentCenter );
					SolidBrush sb(Color(254, 0, 0, 0)) ;

					gph.DrawString(title, title.GetLength(), &font, layout, &sf, &sb) ;
				}

				*pResult = 1 ;
				consumed = TRUE ;
			}
		}
		break ;
	}

	return consumed ;
}

BOOL WLSkinOption::OnCommand(WPARAM wParam, LPARAM /*lParam*/, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	WORD id = LOWORD(wParam) ;
	WORD code = HIWORD(wParam) ;

	switch (id)
	{
	case Id_Btn_TileMode :
		{
			if (code == BN_CLICKED)
			{
				m_btn_tile_mode.SetCheck(1) ;
				m_btn_single_mode.SetCheck(0) ;

				m_listener->OnSelectTileMode() ;
				consumed = TRUE ;
			}
		}
		break ;

	case Id_Btn_SingleMode :
		{
			if (code == BN_CLICKED)
			{
				m_btn_tile_mode.SetCheck(0) ;
				m_btn_single_mode.SetCheck(1) ;

				m_listener->OnSelectSingleMode() ;
				consumed = TRUE ;
			}
		}
		break ;

	case Id_Btn_OK :
		m_listener->OnOK() ;
		break ;

	case Id_Btn_Cancel :
		m_listener->OnCancel() ;
		break ;
	}

	return consumed ;
}
