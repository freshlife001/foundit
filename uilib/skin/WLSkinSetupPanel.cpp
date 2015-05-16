#include "StdAfx.h"

#include "WLSkinSetupPanel.h"

#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"
#include "common/utils/UIUtils.h"
#include "common/utils/Graphics.h"
#include "common/windowless/ButtonAL/WLButtonALHelper.h"

#include "frame/frame.h"
#include "frame/mainwnd/skin/Skin.h"
#include "frame/mainwnd/WLMainWindowRoot.h"

#include "libim/IAddressInfo.h"

COLORREF const WLSkinSetupPanel::kSystemColorList[kSystemColorSkinBoxCount] = 
{
	RGB(0x7e, 0x00, 0x00), RGB(0x80, 0x2d, 0x00), RGB(0x84, 0x4d, 0x00), 
	RGB(0x8a, 0x7e, 0x00), RGB(0x48, 0x6a, 0x00), RGB(0x00, 0x5e, 0x13),
	RGB(0x00, 0x56, 0x1e), RGB(0x00, 0x58, 0x52), RGB(0x00, 0x58, 0x82),
	RGB(0x00, 0x32, 0x66), RGB(0x00, 0x1a, 0x58), RGB(0x01, 0x00, 0x4c),
	RGB(0x30, 0x00, 0x4a), RGB(0x50, 0x00, 0x43), RGB(0x66, 0x01, 0x44),
	RGB(0x70, 0x01, 0x38), RGB(0x53, 0x53, 0x53), RGB(0xe6, 0x00, 0x0a),
	RGB(0xeb, 0x5e, 0x00), RGB(0xf4, 0x95, 0x00), RGB(0xff, 0xf0, 0x01),
	RGB(0x8e, 0xc2, 0x1f), RGB(0x23, 0xac, 0x39), RGB(0x00, 0x9a, 0x44),
	RGB(0x00, 0x9e, 0x94), RGB(0x00, 0x9f, 0xea), RGB(0x00, 0x67, 0xb8),
	RGB(0x00, 0x47, 0x9f), RGB(0x1d, 0x1f, 0x88), RGB(0x5e, 0x19, 0x86),
	RGB(0x94, 0x07, 0x7e), RGB(0xe4, 0x00, 0x76), RGB(0xe6, 0x00, 0x4b),
	RGB(0x95, 0x95, 0x95),

	RGB(0xf2,0x9b,0x75), RGB(0xf5,0xb1,0x80), RGB(0xf9,0xcb,0x8a), RGB(0xff,0xf6,0x99), 
	RGB(0xcb,0xe1,0x98), RGB(0xac,0xd5,0x98), RGB(0x88,0xc9,0x98), RGB(0x85,0xcc,0xc8),
	RGB(0x7d,0xcb,0xf4), RGB(0x89,0xab,0xda), RGB(0x8d,0x97,0xca), RGB(0x8e,0x82,0xbb),
	RGB(0xaa,0x89,0xbc), RGB(0xc4,0x90,0xbd), RGB(0xf1,0x9e,0xbf), RGB(0xf1,0x9c,0x9c), RGB(0xc9,0xc9,0xc9),
} ;

WLSkinSetupPanel::WLSkinSetupPanel(void)
:
m_old_skin(NULL)
, m_change_skin(FALSE)
, m_main_window_root(NULL)
{
}

WLSkinSetupPanel::~WLSkinSetupPanel(void)
{
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Interface

BOOL WLSkinSetupPanel::OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) 
{
	// 复制一份旧皮肤
	m_old_skin = GetFrame()->GetSkin().LightDuplicate() ;
	if (m_old_skin == NULL)
		return FALSE ;

	if (!__super::OnCreate(dwStyle, x, y, cx, cy, pParent))
		return FALSE ;

	GetFrame()->RegisterSkinWindow(GetHost()) ;

	m_selected_skin_box = NULL ;

	InitImage() ;

	int left = 261 ;
	int top = 27 ;
	int width = 24 ;
	int height ;
	int horz_space = 5 ;
	int vert_space ;

	if (!m_btn_gain_skin.Create(WS_VISIBLE, left, top, width, 0, this, ID_BTN_GAIN_SKIN))
		return FALSE ;
	m_btn_gain_skin.SetStyle(WLButtonAL::FITFRGNDSIZE) ;
	m_btn_gain_skin.SetFrgnd(m_img_btn_frgnd, 3, WLButtonAL::FITX) ;
	m_btn_gain_skin.SetIcon(m_img_gain_skin) ;
	m_btn_gain_skin.AddTooltip(_T("获取和分享皮肤")) ;

	left += (width + horz_space) ;
	if (!m_btn_add_skin.Create(WS_VISIBLE, left, top, width, 0, this, ID_BTN_ADD_SKIN))
		return FALSE ;
	m_btn_add_skin.SetStyle(WLButtonAL::FITFRGNDSIZE) ;
	m_btn_add_skin.SetFrgnd(m_img_btn_frgnd, 3, WLButtonAL::FITX) ;
	m_btn_add_skin.SetIcon(m_img_add_skin) ;
	m_btn_add_skin.AddTooltip(_T("添加图片皮肤")) ;

	left += (width + horz_space) ;
	if (!m_btn_delete_skin.Create(WS_VISIBLE, left, top, width, 0, this, ID_BTN_DELETE_SKIN))
		return FALSE ;
	m_btn_delete_skin.SetStyle(WLButtonAL::FITFRGNDSIZE) ;
	m_btn_delete_skin.SetFrgnd(m_img_btn_frgnd, 3, WLButtonAL::FITX) ;
	m_btn_delete_skin.SetIcon(m_img_delete_skin) ;
	m_btn_delete_skin.AddTooltip(_T("删除皮肤")) ;

	if (!m_btn_add_color.Create(WS_VISIBLE, 247, 274, 0, 0, this, ID_BTN_ADD_COLOR))
		return FALSE ;
	m_btn_add_color.SetWindowText(_T("添加自定义色")) ;
	m_btn_add_color.SetStyle(WLButtonAL::FITFRGNDSIZE | WLButtonAL::FITTEXTSIZE) ;
 	m_btn_add_color.SetFrgnd(m_img_btn_frgnd, 3, WLButtonAL::FITX) ;
	m_btn_add_color.SetMargin(4, 0, 4, 0) ;
	m_btn_add_color.SetIcon(m_img_add_color) ;

	// 确定取消按钮
	width = 64 ;
	horz_space = 10 ;
	left = cx - (width + horz_space) * 2 ;
	top = 329 ;
	if (!m_btn_ok.Create(WS_VISIBLE, left, top, width, 0, this, ID_BTN_OK))
		return FALSE ;
	m_btn_ok.SetWindowText(_T("确定")) ;
	WLButtonALHelper::Process(m_btn_ok, _T("DefPushButton")) ;

	left += (width + horz_space) ;
	if (!m_btn_cancel.Create(WS_VISIBLE, left, top, width, 0, this, ID_BTN_CANCEL))
		return FALSE ;
	m_btn_cancel.SetWindowText(_T("取消")) ;
	WLButtonALHelper::Process(m_btn_cancel, _T("DefPushButton")) ;

	// 图片皮肤
	left = 10 ;
	top = 50 ;
	width = 50 ;
	height = 70 ;
	horz_space = 7 ;
	vert_space = 7 ;

	int i, col, row ;
	for (i = 0 ; i < kImageSkinBoxCount ; ++ i)
	{
		col = i % kImageSkinBoxColumnCount ;
		row = i / kImageSkinBoxColumnCount ;

		m_image_skin_boxes[i].SetSkinBoxType(WLSkinBox::ImageSkin) ;
		m_image_skin_boxes[i].Create(
			WS_VISIBLE,
			left + col * (width + horz_space),
			top + row * (height + vert_space),
			0, 0,
			this,
			ID_IMAGE_SKIN_BOX_BEGIN + i
			) ;
		m_image_skin_boxes[i].SetListener(this) ;
	}

	col = i % kImageSkinBoxColumnCount ;
	row = i / kImageSkinBoxColumnCount ;

	if (!m_btn_add_image.Create(WS_VISIBLE, left + col * (width + horz_space), top + row * (height + vert_space), 0, 0, this, ID_BTN_ADD_IMAGE))
		return FALSE ;
	m_btn_add_image.SetStyle(WLButtonAL::FITFRGNDSIZE) ;
	m_btn_add_image.SetFrgnd(m_img_add_image, 3, WLButtonAL::FIXED) ;
	m_btn_add_image.AddTooltip(_T("请点击选择一张图片，生成属于您自己风格的MSNLite皮肤。")) ;

	// 系统颜色皮肤
	left = 9 ;	top = 208 ; width = 17 ; height = 18 ; horz_space = 3 ; vert_space = 3 ;
	for (i = 0 ; i < kSystemColorSkinBoxCount ; ++ i)
	{
		col = i % kColorSkinBoxColumnCount ;
		row = i / kColorSkinBoxColumnCount ;

		m_system_color_skin_boxes[i].SetSkinBoxType(WLSkinBox::ColorSkin) ;
		m_system_color_skin_boxes[i].Create(
			WS_VISIBLE,
			left + col * (width + horz_space),
			top + row * (height + vert_space),
			0, 0,
			this,
			ID_SYSTEM_COLOR_SKIN_BOX_BEGIN + i
			) ;
		m_system_color_skin_boxes[i].SetListener(this) ;
	}

	// 用过的颜色皮肤
	left = 9 ; top = 302 ; width = 17 ; height = 18 ; horz_space = 3 ; vert_space = 3 ;
	for (i = 0 ; i < kRecentlyColorSkinBoxCount ; ++ i)
	{
		col = i % kColorSkinBoxColumnCount ;
		row = i / kColorSkinBoxColumnCount ;

		m_recently_color_skin_boxes[i].SetSkinBoxType(WLSkinBox::ColorSkin) ;
		m_recently_color_skin_boxes[i].Create(
			WS_VISIBLE,
			left + col * (width + horz_space),
			top + row * (height + vert_space),
			0, 0,
			this,
			ID_CUSTOM_COLOR_SKIN_BOX_BEGIN + i
			) ;
		m_recently_color_skin_boxes[i].SetListener(this) ;
	}
	
	LoadRecentlySkinList() ;

	return TRUE ;
}

void WLSkinSetupPanel::InitImage() 
{
	m_img_btn_frgnd = Shared_ImageEx::FromId(IDI_BUBBLE_EDIT_BKGND_PNG) ;
	m_img_gain_skin = Shared_ImageEx::FromId(IDI_BTN_GAIN_SKIN_PNG) ;
	m_img_add_skin = Shared_ImageEx::FromId(IDI_BTN_ADD_SKIN_PNG) ;
	m_img_delete_skin = Shared_ImageEx::FromId(IDI_BTN_DELETE_SKIN_PNG) ;
	m_img_add_image = Shared_ImageEx::FromId(IDI_BTN_ADD_IMAGE_PNG) ;
	m_img_add_color = Shared_ImageEx::FromId(IDI_BTN_ADD_COLOR_PNG) ;
}

void WLSkinSetupPanel::LoadRecentlySkinList() 
{
	// 默认皮肤
	ISkin const & current_skin = GetFrame()->GetSkin() ;
	BOOL select_current_skin = FALSE ;

	m_image_skin_boxes[0].SetSkin(new DefaultImageSkin) ;
	if (current_skin.GetMode() == ISkin::DefaultImage)
	{
		m_image_skin_boxes[0].Select(TRUE) ;
		select_current_skin = TRUE ;
		m_selected_skin_box = m_image_skin_boxes ;
	}

	// 获得所有皮肤
	boost::shared_ptr<IPropertyTree> config ;
	config = GetFrame()->GetConfig()->GetUIPrivatePropertyTree()->createView(MSNLite::skin) ;
	SkinList all_skin_list ;
	SkinHelper::LoadRecentlySkinList(config, all_skin_list) ;

	// 分为图片皮肤和颜色皮肤
	SkinList::iterator pos ;
	for (pos = all_skin_list.begin() ; pos != all_skin_list.end() ; ++ pos)
	{
		ISkin::Mode skin_mode = (*pos)->GetMode() ;
		if (skin_mode == ISkin::CustomImage)
			m_image_skin_list.push_back(*pos) ;
		else if (skin_mode == ISkin::SolidColor)
			m_color_skin_list.push_back(*pos) ;
	}

	int i ;
	for (i = 1, pos = m_image_skin_list.begin() ; i < kImageSkinBoxCount && pos != m_image_skin_list.end() ; ++ pos)
	{
		m_image_skin_boxes[i].SetSkin(*pos) ;

		if (!select_current_skin && current_skin.GetGUID() == (*pos)->GetGUID())
		{
			m_image_skin_boxes[i].Select(TRUE) ;
			select_current_skin = TRUE ;
			m_selected_skin_box = m_image_skin_boxes + i ;
		}

		++ i ;
	}

	// 系统颜色皮肤
	for (i = 0 ; i < kSystemColorSkinBoxCount ; ++ i)
	{
		ISkin * skin = new SolidColorSkin(kSystemColorList[i]) ;
		m_system_color_skin_list.push_back(skin) ;
		m_system_color_skin_boxes[i].SetSkin(skin) ;
	}

	// 用过的颜色皮肤
	for (i = 0, pos = m_color_skin_list.begin() ; i < kRecentlyColorSkinBoxCount && pos != m_color_skin_list.end() ; ++ pos)
	{
		m_recently_color_skin_boxes[i].SetSkin(*pos) ;

		if (!select_current_skin && current_skin.GetGUID() == (*pos)->GetGUID())
		{
			m_recently_color_skin_boxes[i].Select(TRUE) ;
			select_current_skin = TRUE ;
			m_selected_skin_box = m_recently_color_skin_boxes + i ;
		}

		++ i ;
	}
}

void WLSkinSetupPanel::OnDestroy() 
{
	if (m_change_skin)
	{
		delete m_old_skin ;
		m_old_skin = NULL ;
	}
	else
	{
		m_old_skin->Activate() ;
		GetFrameImpl()->UseSkin(m_old_skin, FALSE) ;
	}

	GetFrame()->UnRegisterSkinWindow(GetHost()) ;

	FreeSkinList() ;
	__super::OnDestroy() ;
}

void WLSkinSetupPanel::FreeSkinList() 
{
	SkinList::iterator pos ;
	for (pos = m_image_skin_list.begin() ; pos != m_image_skin_list.end() ; ++ pos)
	{
		delete (*pos) ;
	}
	m_image_skin_list.clear() ;
	for (pos = m_color_skin_list.begin() ; pos != m_color_skin_list.end() ; ++ pos)
	{
		delete (*pos) ;
	}
	m_color_skin_list.clear() ;

	for (pos = m_system_color_skin_list.begin() ; pos != m_system_color_skin_list.end() ; ++ pos)
	{
		delete (*pos) ;
	}
	m_system_color_skin_list.clear() ;
}

BOOL WLSkinSetupPanel::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	if (__super::OnWndMsg(message, wParam, lParam, pResult))
		return TRUE ;

	*pResult = 0 ;
	BOOL consumed = FALSE ;

    switch (message)
    {
    case WM_KEYDOWN :
        {
            if (wParam == VK_ESCAPE)
            {
                ::DestroyWindow(GetHost()) ;
                consumed = TRUE ;
            }
        }
        break ;
    }

	return consumed ;
}

BOOL WLSkinSetupPanel::OnControlCommand(WORD id, WORD code, HWL wnd, LRESULT * pResult) 
{
	if (__super::OnControlCommand(id, code, wnd, pResult))
		return TRUE ;

	*pResult = 0 ;
	BOOL consumed = FALSE ;

	switch (id)
	{
	case ID_BTN_GAIN_SKIN :
		{
			if (code == BN_CLICKED)
			{
				std::wstring wstrUrl = GetAddrInfoWUrl(IAddressInfo::ISA_BBS, L"/forum-15-1.html");
				hdutils::OpenURL(wstrUrl);
				consumed = TRUE ;
			}
		}
		break ;

	case ID_BTN_ADD_IMAGE :
	case ID_BTN_ADD_SKIN :
		{
			if (code == BN_CLICKED)
			{
				PickImageToSkin() ;
				consumed = TRUE ;
			}
		}
		break ;

	case ID_BTN_DELETE_SKIN :
		{
			if (code == BN_CLICKED)
			{
				DeleteSelectedSkin() ;
				consumed = TRUE ;
			}
		}
		break ;

	case ID_BTN_ADD_COLOR :
		{
			if (code == BN_CLICKED)
			{
				PickColorToSkin() ;
				consumed = TRUE ;
			}
		}
		break ;

	case ID_BTN_OK :
		{
			if (code == BN_CLICKED)
			{
				m_change_skin = TRUE ;
				ShowSelectedSkin(TRUE) ;
				::DestroyWindow(GetHost()) ;
				consumed = TRUE ;
			}
		}
		break ;

	case ID_BTN_CANCEL :
		{
			if (code == BN_CLICKED)
			{
				::DestroyWindow(GetHost()) ;
				consumed = TRUE ;
			}
		}
		break ;
	}

	return consumed ;
}

void WLSkinSetupPanel::OnClose() 
{
	m_change_skin = TRUE ;
	ShowSelectedSkin(TRUE) ;

	__super::OnClose() ;
}

void WLSkinSetupPanel::PickImageToSkin() 
{
	if (m_main_window_root == NULL)
		return ;

	CWnd * host = CWnd::FromHandle(GetHost()) ;
	CString filter(_T("图像文件(*.bmp;*.jpg;*.jpeg;*.png)|*.bmp;*.jpg;*.jpeg;*.png||")) ;
	CFileDialog fd(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter, host) ;
	if (fd.DoModal() == IDOK)
	{
		CString image_file_path = fd.GetPathName() ;
		Shared_ImageEx image = Shared_ImageEx::FromFile(image_file_path) ;
		if (image->GetLastStatus() != Ok)
			return ;

		CustomImageSkin * new_skin = new CustomImageSkin ;
		POINT origin = {100, 100} ;
		if (!new_skin->Create(image_file_path, CustomImageSkin::Single, origin))
			return ;

		if (!m_main_window_root->BeginEditCustomImageSkin(new_skin, this))
		{
			delete new_skin ;
			new_skin = NULL ;
		}
	}
}

// 选择颜色做皮肤
void WLSkinSetupPanel::PickColorToSkin() 
{
	CWnd * host = CWnd::FromHandle(GetHost()) ;
	CColorDialog cd(0, CC_FULLOPEN, host) ;
	if (cd.DoModal() != IDOK)
		return ;

	COLORREF new_color = cd.GetColor() ;

	// 遍历m_color_skin_list，找到同颜色的皮肤则移到最前面，否则添加新皮肤
	BOOL new_color_is_old = FALSE ;
	SkinList::iterator pos ;
	for (pos = m_color_skin_list.begin() ; pos != m_color_skin_list.end() ; ++ pos)
	{
		SolidColorSkin * color_skin = (SolidColorSkin*)(*pos) ;

		if (color_skin->GetColor() == new_color)
		{
			m_color_skin_list.erase(pos) ;
			m_color_skin_list.push_front(color_skin) ;

			new_color_is_old = TRUE ;
			break ;
		}
	}
	if (!new_color_is_old)
	{
		SolidColorSkin * color_skin = new SolidColorSkin(new_color) ;
		GetFrameImpl()->AddSkin(color_skin) ;
		m_color_skin_list.push_front(color_skin) ;
	}

	// 重新初始化skinbox
	// 用过的颜色皮肤
	LoadColorSkin() ;
	m_selected_skin_box->Select(FALSE) ;
	m_recently_color_skin_boxes[0].Select(TRUE) ;
	m_selected_skin_box = m_recently_color_skin_boxes ;

	ShowSelectedSkin() ;
}

// 删除选中的皮肤
void WLSkinSetupPanel::DeleteSelectedSkin() 
{
	if (m_selected_skin_box == NULL)
		return ;

	// 默认皮肤和系统颜色皮肤不能被删除
	UINT box_id = m_selected_skin_box->GetID() ;
	if (box_id == ID_DEFAULT_SKIN_BOX || (box_id >= ID_SYSTEM_COLOR_SKIN_BOX_BEGIN && box_id < ID_CUSTOM_COLOR_SKIN_BOX_BEGIN))
		return ;

	ISkin * selected_skin = m_selected_skin_box->GetSkin() ;
	if (selected_skin == NULL)
		return ;

	// 选中并使用默认皮肤
	m_selected_skin_box->Select(FALSE) ;
	m_image_skin_boxes[0].Select(TRUE) ;
	m_selected_skin_box = m_image_skin_boxes ;

	// 如果删除的皮肤是旧皮肤，则要使用并保存选中的默认皮肤
	if (m_old_skin != NULL && m_old_skin->GetGUID() == selected_skin->GetGUID())
	{
		delete m_old_skin ;
		m_old_skin = new DefaultImageSkin ;
		ShowSelectedSkin(TRUE) ;
	}
	else
		ShowSelectedSkin(FALSE) ;

	ISkin::Mode deleted_skin_mode = selected_skin->GetMode() ;

	// 删除选中的皮肤
	GetFrameImpl()->DeleteSkin(selected_skin) ;

	// 重载皮肤
	if (deleted_skin_mode == ISkin::CustomImage)
	{
		SkinList::iterator pos ;
		for (pos = m_image_skin_list.begin() ; pos != m_image_skin_list.end() ; ++ pos)
		{
			if (*pos == selected_skin)
			{
				m_image_skin_list.erase(pos) ;
				break ;
			}
		}

		LoadImageSkin() ;
	}
	else if (deleted_skin_mode == ISkin::SolidColor)
	{
		SkinList::iterator pos ;
		for (pos = m_color_skin_list.begin() ; pos != m_color_skin_list.end() ; ++ pos)
		{
			if (*pos == selected_skin)
			{
				m_color_skin_list.erase(pos) ;
				break ;
			}
		}

		LoadColorSkin() ;
	}

	delete selected_skin ;
	selected_skin = NULL ;
}

// 使用选中的皮肤
BOOL WLSkinSetupPanel::ShowSelectedSkin(BOOL save_to_config) 
{
	if (m_selected_skin_box == NULL)
		return FALSE ;

	ISkin * selected_skin = m_selected_skin_box->GetSkin() ;
	if (selected_skin == NULL)
		return FALSE ;

	ISkin * skin_copy = selected_skin->LightDuplicate() ;
	if (skin_copy == NULL)
		return FALSE ;

	skin_copy->Activate() ;
	GetFrameImpl()->UseSkin(skin_copy, save_to_config) ;

	return TRUE ;
}

// 载入图片皮肤
void WLSkinSetupPanel::LoadImageSkin() 
{
	int i ;
	SkinList::iterator pos ;
	for (i = 1, pos = m_image_skin_list.begin() ; i < kImageSkinBoxCount && pos != m_image_skin_list.end() ; ++ i, ++ pos)
	{
		m_image_skin_boxes[i].SetSkin(*pos) ;
	}
	for ( ; i < kImageSkinBoxCount ; ++ i)
	{
		m_image_skin_boxes[i].SetSkin(NULL) ;
	}
}

// 重载入颜色皮肤
void WLSkinSetupPanel::LoadColorSkin() 
{
	int i ;
	SkinList::iterator pos ;
	for (i = 0, pos = m_color_skin_list.begin() ; i < kRecentlyColorSkinBoxCount && pos != m_color_skin_list.end() ; ++ i, ++ pos)
	{
		m_recently_color_skin_boxes[i].SetSkin(*pos) ;
	}
	for ( ; i < kRecentlyColorSkinBoxCount ; ++ i)
	{
		m_recently_color_skin_boxes[i].SetSkin(NULL) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// SkinBoxListener

void WLSkinSetupPanel::OnSelectedSkinBox(WLSkinBox * skin_box) 
{
	if (m_selected_skin_box == skin_box)
		return ;

	if (m_selected_skin_box != NULL)
	{
		m_selected_skin_box->Select(FALSE) ;
	}

	m_selected_skin_box = skin_box ;
	skin_box->SetZorder(HWL_BOTTOM) ;

	ShowSelectedSkin() ;
}

void WLSkinSetupPanel::OnDblClickSkinBox(WLSkinBox * skin_box) 
{
	ISkin * new_skin = skin_box->GetSkin() ;
	if (new_skin == NULL)
		return ;

	ISkin * skin_to_use = new_skin->LightDuplicate();
	if (skin_to_use != NULL)
	{
		m_change_skin = TRUE ;

		skin_to_use->Activate() ;
		GetFrameImpl()->UseSkin(skin_to_use, TRUE) ;
	}

	// 关闭窗口
	::DestroyWindow(GetHost()) ;
}

//////////////////////////////////////////////////////////////////////////
// SkinPreviewListener Interface

void WLSkinSetupPanel::OnConfirmUseSkin(CustomImageSkin * skin) 
{
	skin->Deactivate() ;
	GetFrameImpl()->AddSkin(skin) ;

	m_image_skin_list.push_front(skin) ;

	LoadImageSkin() ;

	m_selected_skin_box->Select(FALSE) ;
	m_image_skin_boxes[1].Select(TRUE) ;
	m_selected_skin_box = m_image_skin_boxes + 1 ;

	ShowSelectedSkin() ;
}

void WLSkinSetupPanel::OnCancelUseSkin(CustomImageSkin * skin) 
{
	if (skin != NULL)
		delete skin ;
}
