#include "StdAfx.h"
#include "WLSkinBox.h"

#include "include/ISkin.h"

#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"

const int WLSkinBox::kImageSkinWidth = 62 ;
const int WLSkinBox::kImageSkinHeight = 82 ;
const int WLSkinBox::kImageSkinBorderWidth = 6 ;

const int WLSkinBox::kColorSkinWidth = 23 ;
const int WLSkinBox::kColorSkinHeight = 24 ;
const int WLSkinBox::kColorSkinBorderWidth = 3 ;

WLSkinBox::WLSkinBox(void)
:
m_skin_box_type(ImageSkin)
, m_skin(NULL)
, m_selected(FALSE)
, m_listener(NULL)
{
}

WLSkinBox::~WLSkinBox(void)
{
}

void WLSkinBox::SetSkin(ISkin * skin) 
{
	if (m_skin == skin)
		return ;

	m_skin = skin ;
	UpdateView() ;
}

// 选择
void WLSkinBox::Select(BOOL select) 
{
	if (m_skin == NULL || m_selected == select)
		return ;

	m_selected = select ;
	UpdateView() ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess interface

BOOL WLSkinBox::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (m_skin_box_type == ImageSkin)
	{
		m_img_bkgnd = Shared_ImageEx::FromId(IDI_IMAGE_SKIN_BKGND_PNG) ;
		m_img_border = Shared_ImageEx::FromId(IDI_IMAGE_SKIN_BORDER_PNG) ;

		m_border_width = kImageSkinBorderWidth ;
		m_box_width = kImageSkinWidth ;
		m_box_height = kImageSkinHeight ;
	}
	else if (m_skin_box_type == ColorSkin)
	{
		m_img_bkgnd = Shared_ImageEx::FromId(IDI_COLOR_SKIN_BKGND_PNG) ;
		m_img_border = Shared_ImageEx::FromId(IDI_COLOR_SKIN_BORDER_PNG) ;

		m_border_width = kColorSkinBorderWidth ;
		m_box_width = kColorSkinWidth ;
		m_box_height = kColorSkinHeight ;
	}
	else
		return FALSE ;

	SetWndLessPos(
		x - m_border_width,
		y - m_border_width,
		m_box_width,
		m_box_height,
		SWLP_X | SWLP_Y | SWLP_CX | SWLP_CY
		) ;

    return __super::OnCreate(dwStyle, x, y, cx, cy, pParent);
}

BOOL WLSkinBox::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	switch (message)
	{
	case WM_LBUTTONDOWN :
		{
			if (m_skin != NULL && !IsSelected())
			{
				Select(TRUE) ;

				if (m_listener != NULL)
				{
					m_listener->OnSelectedSkinBox(this) ;
				}

				consumed = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONDBLCLK :
		{
			if (m_skin != NULL)
			{
				Select(TRUE) ;

				if (m_listener != NULL)
				{
					m_listener->OnSelectedSkinBox(this) ;
					m_listener->OnDblClickSkinBox(this) ;
				}

				consumed = TRUE ;
			}
		}
		break ;
	}

	return consumed ;
}

void WLSkinBox::OnDraw(HDC hDC, RECT const &/*rcUpdate*/) 
{
	RECT bound ;
	GetRectInHost(&bound) ;

	if (m_skin != NULL)
	{
		m_skin->DrawThumbnail(
			hDC,
			bound.left + m_border_width, bound.top + m_border_width,
			bound.right - bound.left - (m_border_width << 1),
			bound.bottom - bound.top - (m_border_width << 1)
			) ;

		// 选中时绘制边框
		if (m_selected)
		{
			Graphics gph(hDC) ;
			gph.DrawImage(
				m_img_border,
				Rect(bound.left, bound.top, m_box_width, m_box_height),
				0, 0, m_img_border->GetWidth(), m_img_border->GetHeight(), UnitPixel, NULL
				) ;
		}
	}
	else
	{
		Graphics gph(hDC) ;
		gph.DrawImage(
			m_img_bkgnd,
			Rect(bound.left + m_border_width, bound.top + m_border_width, m_box_width - (m_border_width << 1), m_box_height - (m_border_width << 1)),
			0, 0, m_img_bkgnd->GetWidth(), m_img_bkgnd->GetHeight(), UnitPixel, NULL
			) ;
	}
}
