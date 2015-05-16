#include "StdAfx.h"
#include "WLSkinPreview.h"

#include "Skin.h"
#include "SkinPreviewListener.h"

WLSkinPreview::WLSkinPreview(void)
:
m_skin(NULL)
, m_listener(NULL)
, m_dragging(FALSE)
{
	m_bkgnd_orgin_pos.x = 100 ;
	m_bkgnd_orgin_pos.y = 100 ;
	//m_bkgnd_mode = Single ;
}

WLSkinPreview::~WLSkinPreview(void)
{
}
/*
BOOL WLSkinPreview::LoadBkgnd(LPCTSTR lpszFilePath) 
{
	m_bkgnd = Shared_ImageEx::FromFile(lpszFilePath) ;
	if (m_bkgnd == NULL)
		return FALSE ;

	return TRUE ;
}
*/
CCanvasDC & WLSkinPreview::GetFace() 
{
	return m_face.GetFace() ;
}

void WLSkinPreview::SetSkin(CustomImageSkin * skin) 
{
	m_skin = skin ;
}

BOOL WLSkinPreview::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	if (!m_face.Create(WS_VISIBLE, m_bkgnd_orgin_pos.x, m_bkgnd_orgin_pos.y, 0, 0, this, (UINT)-1))
		return FALSE ;
	m_face.SetListener(this) ;

	if (!m_option.Create(WS_VISIBLE, 0, 0, 233, 159, this, (UINT)-1))
		return FALSE ;
	m_option.SetListener(this) ;

	RecalcLayout() ;
	DWORD activate_thread_id = ::GetWindowThreadProcessId(::GetForegroundWindow(), NULL) ;
	DWORD current_thread_id = ::GetCurrentThreadId() ;

	::AttachThreadInput(activate_thread_id, current_thread_id, TRUE) ;

    return __super::OnCreate(dwStyle, x, y, cx, cy, pParent) ;
}

void WLSkinPreview::OnDraw(HDC hdc, RECT const &rcUpdate) 
{
	m_skin->Draw(hdc, rcUpdate.left, rcUpdate.top, rcUpdate.right - rcUpdate.left, rcUpdate.bottom - rcUpdate.top) ;
}

void WLSkinPreview::OnSized(SIZE const &/*szOld*/, SIZE const &/*szNew*/) 
{
	RecalcLayout() ;
}

BOOL WLSkinPreview::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	*pResult = 0 ;
	BOOL consumed = FALSE ;

	switch (message)
	{
	case WM_LBUTTONDBLCLK :
		{
/*			if (m_listener != NULL)
			{
				// 获得主窗口位置
				POINT face_pos ;
				m_face.GetPos(&face_pos) ;

				POINT skin_pos = m_skin->GetOrigin() ;
				m_skin->SetOrigin(skin_pos.x - face_pos.x, skin_pos.y - face_pos.y) ;
				m_listener->OnOK(m_skin) ;
			}*/
			consumed = TRUE ;
		}
		break ;

	case WM_RBUTTONDOWN :
		{
/*			if (m_skin != NULL)
			{
				Skin::ImageMode mode = m_skin->GetImgMode() ;
				if (mode == Skin::Single)
					m_skin->SetImgMode(Skin::Tile) ;
				else
					m_skin->SetImgMode(Skin::Single) ;

				UpdateView() ;
			}*/
			consumed = TRUE ;
		}
		break ;

	case WM_KEYDOWN :
		{
			if (wParam == VK_ESCAPE)
			{
				m_listener->OnCancelUseSkin(m_skin) ;

				::DestroyWindow(GetHost()) ;

				consumed = TRUE ;
			}
		}
		break ;

	case WM_LBUTTONDOWN :
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
			BeginDrag(point) ;
		}
		break ;

	case WM_MOUSEMOVE :
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)} ;
			DoDrag(point) ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			EndDrag() ;
		}
		break ;
	}

	return consumed ;
}

//////////////////////////////////////////////////////////////////////////
// IFaceListener Interface

void WLSkinPreview::OnFaceMove() 
{
	RecalcLayout() ;
}

//////////////////////////////////////////////////////////////////////////
// IOptionListener Interface

void WLSkinPreview::OnSelectTileMode() 
{
	if (m_skin != NULL)
	{
		CustomImageSkin::ImageMode mode = m_skin->GetImgMode() ;
		if (mode == CustomImageSkin::Single)
		{
			m_skin->SetImgMode(CustomImageSkin::Tile) ;
			UpdateView() ;
		}
	}
}

void WLSkinPreview::OnSelectSingleMode() 
{
	if (m_skin != NULL)
	{
		CustomImageSkin::ImageMode mode = m_skin->GetImgMode() ;
		if (mode == CustomImageSkin::Tile)
		{
			m_skin->SetImgMode(CustomImageSkin::Single) ;
			UpdateView() ;
		}
	}
}

void WLSkinPreview::OnOK() 
{
	if (m_listener != NULL)
	{
		// 获得主窗口位置大小
		RECT face_bound ;
		m_face.GetRectInParent(&face_bound) ;

		POINT skin_pos = m_skin->GetOrigin() ;
		m_skin->SetOrigin(skin_pos.x - face_bound.left, skin_pos.y - face_bound.top) ;
		m_skin->SetThumbnailSize(face_bound.right - face_bound.left, face_bound.bottom - face_bound.top) ;
		m_skin->SaveImageFile() ;

		m_listener->OnConfirmUseSkin(m_skin) ;
	}

	::DestroyWindow(GetHost()) ;
}

void WLSkinPreview::OnCancel() 
{
	m_listener->OnCancelUseSkin(m_skin) ;

	::DestroyWindow(GetHost()) ;
}

void WLSkinPreview::BeginDrag(POINT const &point) 
{
	if (m_dragging)
		return ;

	m_dragging = TRUE ;
	SetCapture() ;

	m_drag_point.x = point.x ;
	m_drag_point.y = point.y ;
}

void WLSkinPreview::DoDrag(POINT const &point) 
{
	if (!m_dragging)
		return ;

	int x_offset = point.x - m_drag_point.x ;
	int y_offset = point.y - m_drag_point.y ;

	SIZE preview_size ;
	GetSize(&preview_size) ;

	if (m_skin->GetImgMode() == CustomImageSkin::Single)
	{
		int new_left = m_skin->GetLeft() + x_offset ;
		int new_top = m_skin->GetTop() + y_offset ;

		if (new_left > preview_size.cx - 100)
			new_left = preview_size.cx - 100 ;
		else if (new_left < 100 - m_skin->GetWidth())
			new_left = 100 - m_skin->GetWidth() ;
		if (new_top > preview_size.cy - 100)
			new_top = preview_size.cy - 100 ;
		else if (new_top < 100 - m_skin->GetHeight())
			new_top = 100 - m_skin->GetHeight() ;

		m_skin->SetLeft(new_left) ;
		m_skin->SetTop(new_top) ;
	}
	else
	{
		int new_left = m_skin->GetLeft() + x_offset ;
		new_left %= m_skin->GetWidth() ;
		if (new_left < 0 && new_left + m_skin->GetWidth() < preview_size.cx / 2)
			new_left += m_skin->GetWidth() ;

		int new_top = m_skin->GetTop() + y_offset ;
		new_top %= m_skin->GetHeight() ;
		if (new_top < 0 && new_top + m_skin->GetHeight() < preview_size.cy / 2)
			new_top += m_skin->GetHeight() ;

		m_skin->SetLeft(new_left) ;
		m_skin->SetTop(new_top) ;
	}
	UpdateView() ;

	m_drag_point.x = point.x ;
	m_drag_point.y = point.y ;
}

void WLSkinPreview::EndDrag() 
{
	if (m_dragging)
	{
		m_dragging = FALSE ;
		ReleaseCapture() ;
	}
}

// 调整选项面板的位置
void WLSkinPreview::RecalcLayout() 
{
	RECT face_bound ;
	m_face.GetRectInParent(&face_bound) ;

	SIZE option_size ;
	m_option.GetSize(&option_size) ;

	SIZE preview_size ;
	GetSize(&preview_size) ;

	int option_offset_face = 82 ;
	POINT option_pos ;		// 待求

	// x坐标
	if (face_bound.left >= option_size.cx)
	{
		option_pos.x = face_bound.left - option_size.cx ;
	}
	else if (preview_size.cx - face_bound.right >= option_size.cx)
	{
		option_pos.x = face_bound.right ;
	}
	else
	{
		option_pos.x = face_bound.left ;
	}

	// y坐标
	option_pos.y = face_bound.top + option_offset_face ;
	if (option_pos.y < 0)
		option_pos.y = 0 ;
	else if (option_pos.y > preview_size.cy - option_size.cy)
		option_pos.y = preview_size.cy - option_size.cy ;

	m_option.SetWndLessPos(option_pos.x, option_pos.y, 0, 0, SWLP_X | SWLP_Y) ;
}
