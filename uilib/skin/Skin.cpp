#include "StdAfx.h"


#include <boost/filesystem/operations.hpp>

#include "utils/strutils.h"
#include "utils/IProfile.h"

#include "common/utils/ImgConverter.h"
#include "common/utils/Graphics.h"
#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/ImageEx/ImageResource.h"

#include "Skin.h"

namespace config_keys
{
	// *****************skin settings*******************************
	static const std::string skin = "skin" ;

	static const std::string skin_latest = "skin_latest" ;
	static const std::string skin_recently_list = "skin_recently_list" ;

	static const std::string skin_mode = "skin_mode" ;
	static const std::string skin_time = "skin_time" ;

	static const std::string skin_image_path = "skin_image_path" ;
	static const std::string skin_image_mode = "skin_image_mode" ;
	static const std::string skin_origin_x = "skin_origin_x" ;
	static const std::string skin_origin_y = "skin_origin_y" ;
	static const std::string skin_name = "skin_name" ;
	static const std::string skin_thumbnail_path = "skin_thumbnail_path" ;

	static const std::string skin_solid_color = "skin_solid_color" ;

}


//////////////////////////////////////////////////////////////////////////
// class Skin

Skin::Skin() 
: 
m_use_time(0)
{

}

// 从配置加载
BOOL Skin::Load(boost::shared_ptr<IPropertyTree> config, std::string guid) 
{
	m_use_time = config->getTime(config_keys::skin_time) ;
	m_guid = guid ;
	return TRUE ;
}

BOOL Skin::Save(boost::shared_ptr<IPropertyTree> config) 
{
	config->set(config_keys::skin_time, m_use_time) ;
	return TRUE ;
}

//////////////////////////////////////////////////////////////////////////
// class CustomImageSkin

CustomImageSkin::CustomImageSkin(void)
: 
m_initialized(FALSE)
, m_activated(FALSE)
, m_few_edge_width(60)
, m_width(1)
, m_height(1)
, m_image_width(1)
, m_image_height(1)
, m_rough_gray_scale(0)
{
	m_thumbnail_size.cx = 1 ;
	m_thumbnail_size.cy = 1 ;
}

CustomImageSkin::~CustomImageSkin(void)
{
}

// 初始化

// 从配置加载
BOOL CustomImageSkin::Load(boost::shared_ptr<IPropertyTree> config, std::string guid) 
{
	if (m_initialized)
		return FALSE ;

//	m_init_from_load_or_create = TRUE ;

	Skin::Load(config, guid) ;

	// 图片路径
	m_image_path = config->getWString(config_keys::skin_image_path).c_str() ;

	// 判断图片文件是否存在，不存在则加载失败
	DWORD file_attributes = GetFileAttributes(m_image_path) ;
	if ((file_attributes == INVALID_FILE_ATTRIBUTES) || (file_attributes & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE ;

	// 模式
	m_image_mode = static_cast<ImageMode>(config->getInt(config_keys::skin_image_mode, 0)) ;
	if (m_image_mode < Single || m_image_mode > Tile)
		m_image_mode = Single ;

	// 位置
	m_origin.x = config->getUInt(config_keys::skin_origin_x, 0) ;
	m_origin.y = config->getUInt(config_keys::skin_origin_y, 0) ;

	// 缩略图路径
	m_thumbnail_path = config->getWString(config_keys::skin_thumbnail_path).c_str() ;
	m_image_thumbnail = Shared_ImageEx::FromFile(m_thumbnail_path) ;

	m_initialized = TRUE ;

	return TRUE ;

	//return Initialize(/*sPath.c_str(), img_mode, origin*/) ;
}

BOOL CustomImageSkin::Save(boost::shared_ptr<IPropertyTree> config) 
{
	if (!m_initialized)
		return FALSE ;

	Skin::Save(config) ;

	// 图片路径
	std::wstring img_path(m_image_path) ;
	config->set(config_keys::skin_image_path, img_path) ;

	// 模式
	config->set(config_keys::skin_image_mode, m_image_mode) ;

	// 位置
	config->set(config_keys::skin_origin_x, m_origin.x) ;
	config->set(config_keys::skin_origin_y, m_origin.y) ;

	// 缩略图路径
	std::wstring thumbnail_path(m_thumbnail_path) ;
	config->set(config_keys::skin_thumbnail_path, thumbnail_path) ;

	return TRUE ;
}

BOOL CustomImageSkin::Delete() 
{
	if (!m_initialized)
		return FALSE ;

	m_image_thumbnail.Reset() ;

	// 删除图片文件和缩略图文件
	boost::filesystem::remove(m_image_path.GetString()) ;
	boost::filesystem::remove(m_thumbnail_path.GetString()) ;

	m_initialized = FALSE ;
	return TRUE ;
}

// 激活皮肤，即根据皮肤图片生成m_skin_dc
BOOL CustomImageSkin::Activate() 
{
	if (!m_initialized)
		return FALSE ;
	if (m_activated)
		return TRUE ;

	if (!InitializeContent( Shared_ImageEx::FromFile( m_image_path )))
		return FALSE ;

	m_image_thumbnail.Reset() ;

	m_activated = TRUE ;
	return TRUE ;
}

// 反激活皮肤，销毁m_skin_dc
BOOL CustomImageSkin::Deactivate() 
{
	if (!m_initialized)
		return FALSE ;
	if (!m_activated)
		return TRUE ;

	UnInitializeContent() ;

	m_activated = FALSE ;
	return TRUE ;
}

// 浅度拷贝，不复制为绘制做准备的数据
ISkin * CustomImageSkin::LightDuplicate() const 
{
	if (!m_initialized)
		return NULL ;

	CustomImageSkin * copy_skin = new CustomImageSkin ;

	copy_skin->m_guid = m_guid ;

	copy_skin->m_initialized = TRUE ;
	copy_skin->m_image_path = m_image_path ;
	copy_skin->m_image_mode = m_image_mode ;
	copy_skin->m_origin = m_origin ;
	copy_skin->m_name = m_name ;
	copy_skin->m_thumbnail_path = m_thumbnail_path ;
	copy_skin->m_image_thumbnail = Shared_ImageEx::FromFile(m_thumbnail_path) ;

	return copy_skin ;
}

BOOL CustomImageSkin::Create(CString const &img_path, ImageMode mode, POINT const &origin) 
{
	if (m_initialized)
		return FALSE ;

//	m_init_from_load_or_create = FALSE ;

	m_image_path = img_path ;
	m_image_mode = mode ;
	m_origin = origin ;
	m_guid = genGUID() ;

	m_initialized = TRUE ;

	return Activate() ;
	//return Initialize(/*img_path, mode, origin*/) ;
}

COLORREF FewImageEdge(HBITMAP bmp, int few_edge_width)
{
	BITMAP bmp_info ;
	if (0 == ::GetObject(bmp, sizeof(bmp_info), &bmp_info))
		return 0 ;

	if (bmp_info.bmBitsPixel != 32)
		return 0 ;

	DWORD bytes_count = bmp_info.bmWidthBytes * bmp_info.bmHeight ;
	BYTE * bytes_data = new BYTE[bytes_count] ;

	::GetBitmapBits(bmp, bytes_count, bytes_data) ;

	ASSERT(few_edge_width >= 1) ;

	// 左上、左、上镜像
	int i, j, m, n ;
	for (j = few_edge_width - 1, n = few_edge_width ; j >= 0 && n < bmp_info.bmHeight ; -- j, ++ n)
	{
		for (i = few_edge_width - 1, m = few_edge_width ; i >= 0 && m < bmp_info.bmWidth ; -- i, ++ m)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			int index2 = n * bmp_info.bmWidthBytes + m * 4 ;

			*(DWORD*)(bytes_data + index) = *(DWORD*)(bytes_data + index2) ;
		}
	}

	for (j = few_edge_width - 1, n = few_edge_width ; j >= 0 && n < bmp_info.bmHeight ; -- j, ++ n)
	{
		for (i = few_edge_width ; i < bmp_info.bmWidth ; ++ i)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			int index2 = n * bmp_info.bmWidthBytes + i * 4 ;

			*(DWORD*)(bytes_data + index) = *(DWORD*)(bytes_data + index2) ;
		}
	}

	for (j = few_edge_width ; j < bmp_info.bmHeight ; ++ j)
	{
		for (i = few_edge_width - 1, m = few_edge_width ; i >= 0 && m < bmp_info.bmWidth ; -- i, ++ m)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			int index2 = j * bmp_info.bmWidthBytes + m * 4 ;

			*(DWORD*)(bytes_data + index) = *(DWORD*)(bytes_data + index2) ;
		}
	}

	// 获得平均颜色
	int pixel_count = 0 ;
	DWORD average_red = 0, average_green = 0, average_blue = 0 ;
	for (j = few_edge_width ; j < bmp_info.bmHeight ; ++ j)
	{
		for (i = few_edge_width ; i < bmp_info.bmWidth ; ++ i)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			average_blue += bytes_data[index] ;
			average_green += bytes_data[index + 1] ;
			average_red += bytes_data[index + 2] ;

			++ pixel_count ;
		}
	}

	average_red /= pixel_count ;
	average_green /= pixel_count ;
	average_blue /= pixel_count ;

	// 模糊化边缘
	BYTE alpha ;
	int k ;
	for (k = 0 ; k < few_edge_width ; ++ k)
	{
		alpha = (BYTE)((few_edge_width - 1 - k) * 255 / few_edge_width) ;

		// top
		j = k ;
		for (i = k ; i < bmp_info.bmWidth - 1 - k ; ++ i)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			bytes_data[index] = (BYTE)(average_blue * alpha / 255 + bytes_data[index] * (255 - alpha) / 255) ;
			bytes_data[index + 1] = (BYTE)(average_green * alpha / 255 + bytes_data[index + 1] * (255 - alpha) / 255) ;
			bytes_data[index + 2] = (BYTE)(average_red * alpha / 255 + bytes_data[index + 2] * (255 - alpha) / 255) ;
		}

		// right
		i = bmp_info.bmWidth - 1 - k ;
		for (j = k ; j < bmp_info.bmHeight - 1 - k ; ++ j)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			bytes_data[index] = (BYTE)(average_blue * alpha / 255 + bytes_data[index] * (255 - alpha) / 255) ;
			bytes_data[index + 1] = (BYTE)(average_green * alpha / 255 + bytes_data[index + 1] * (255 - alpha) / 255) ;
			bytes_data[index + 2] = (BYTE)(average_red * alpha / 255 + bytes_data[index + 2] * (255 - alpha) / 255) ;
		}

		// bottom
		j = bmp_info.bmHeight - 1 - k ;
		for (i = k + 1 ; i < bmp_info.bmWidth - k ; ++ i)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			bytes_data[index] = (BYTE)(average_blue * alpha / 255 + bytes_data[index] * (255 - alpha) / 255) ;
			bytes_data[index + 1] = (BYTE)(average_green * alpha / 255 + bytes_data[index + 1] * (255 - alpha) / 255) ;
			bytes_data[index + 2] = (BYTE)(average_red * alpha / 255 + bytes_data[index + 2] * (255 - alpha) / 255) ;
		}

		// left
		i = k ;
		for (j = k + 1 ; j < bmp_info.bmHeight - k ; ++ j)
		{
			int index = j * bmp_info.bmWidthBytes + i * 4 ;
			bytes_data[index] = (BYTE)(average_blue * alpha / 255 + bytes_data[index] * (255 - alpha) / 255) ;
			bytes_data[index + 1] = (BYTE)(average_green * alpha / 255 + bytes_data[index + 1] * (255 - alpha) / 255) ;
			bytes_data[index + 2] = (BYTE)(average_red * alpha / 255 + bytes_data[index + 2] * (255 - alpha) / 255) ;
		}
	}

	::SetBitmapBits(bmp, bytes_count, bytes_data) ;

	delete bytes_data ;

	return RGB(average_red, average_green, average_blue) ;
}

BOOL CustomImageSkin::InitializeContent( Shared_ImageEx& img )
{
	// 验证图片的有效性
	//Image img(m_image_path) ;
	if ( img.IsNull() || img->GetLastStatus() != Ok)
		return FALSE ;

	m_image_width = img->GetWidth() ;
	m_image_height = img->GetHeight() ;

	HDC desktop_dc = ::GetDC(NULL) ;
	if (m_image_mode == Single)
	{
		if (m_few_edge_width > m_image_width)
			m_few_edge_width = m_image_width ;
		if (m_few_edge_width > m_image_height)
			m_few_edge_width = m_image_height ;

		m_width = m_image_width + m_few_edge_width ;
		m_height = m_image_height + m_few_edge_width ;

		m_skin_dc.Create32BitDC(desktop_dc, m_width, m_height) ;
		{
			Graphics gph(m_skin_dc) ;
			gph.DrawImage(
				img,
				Rect(m_few_edge_width, m_few_edge_width, m_image_width, m_image_height),
				0, 0, m_image_width, m_image_height, UnitPixel, NULL
				) ;
		}

		HBITMAP img_bmp = m_skin_dc.GetBitmap() ;
		m_padding_color = FewImageEdge(img_bmp, m_few_edge_width) ;
		m_skin_dc.ReleaseBitmap(img_bmp) ;
	}
	else if (m_image_mode == Tile)
	{
		int column = 1, row = 1 ;
		if (m_image_width < 200)			// 防止图片太小，导致绘制效率低下
		{
			column = (200 + m_image_width - 1) / m_image_width ;
		}
		if (m_image_height < 200)
		{
			row = (200 + m_image_height - 1) / m_image_height ;
		}

		m_width = column * m_image_width ;
		m_height = row * m_image_height ;

		m_skin_dc.CreateCompatibleDC(desktop_dc, m_width, m_height) ;
		{
			Graphics gph(m_skin_dc) ;

			int i, j ;
			for (j = 0 ; j < row ; ++ j)
			{
				for (i = 0 ; i < column ; ++ i)
				{
					gph.DrawImage(
						img,
						Rect(i * m_image_width, j * m_image_height, m_image_width, m_image_height),
						0, 0, m_image_width, m_image_height, UnitPixel, NULL
						) ;
				}
			}
		}
	}
	::ReleaseDC(NULL, desktop_dc) ;

	RECT bound = {0, 0, m_width, m_height} ;
	m_rough_gray_scale = hdutils::GetRoughGrayScale(m_skin_dc, &bound) ;

	return TRUE ;
}

void CustomImageSkin::UnInitializeContent() 
{
	m_skin_dc.Clear() ;

	m_width = 1 ;
	m_height = 1 ;

	m_image_width = 1 ;
	m_image_height = 1 ;

	m_rough_gray_scale = 0 ;
}

void CustomImageSkin::SetImgMode(ImageMode mode) 
{
	if (mode == m_image_mode)
		return ;

	// 切换模式，先反激活，再激活
	Deactivate() ;
	m_image_mode = mode ;
	Activate() ;
}

// 保存图片文件
void CustomImageSkin::SaveImageFile() 
{
	// 保存图片文件

	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT] ;
	_tsplitpath_s(m_image_path, drive, dir, fname, ext) ;

	std::wstring new_fname = U8TOW(m_guid) ;
	std::wstring profile_path = GetDBStorePath() ;
	std::wstring new_image_path = profile_path + L"Skin\\" + new_fname + ext ;
	
	CopyFile(m_image_path, new_image_path.c_str(), FALSE) ;
	m_image_path = new_image_path.c_str() ;

	//////////////////////////////////////////////////////////////////////////
	// 保存缩略图文件

	std::wstring thumbnail_temp_string_path(profile_path + L"Skin\\" + new_fname + L"_thumbnail.png") ; 

	CCanvasDC thumbnail_dc ;
	thumbnail_dc.Create32BitDC(NULL, m_thumbnail_size.cx, m_thumbnail_size.cy) ;
	Draw(thumbnail_dc, 0, 0, m_thumbnail_size.cx, m_thumbnail_size.cy) ;
	HBITMAP thumbnail_bmp = thumbnail_dc.GetBitmap() ;

	{
		Bitmap bmp(thumbnail_bmp, NULL) ;

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", NULL, &pngClsid);
		bmp.Save(thumbnail_temp_string_path.c_str(), &pngClsid, NULL) ;
	}

	thumbnail_dc.ReleaseBitmap(thumbnail_bmp) ;

	// 重命名缩略图文件
	/*if (!getFileSHA1DBase64(thumbnail_temp_string_path, sha1d))
		return ;
	md5 = strHash(sha1d) ;
	std::wstring fthumbname = U8TOW(md5) ;
	std::wstring thumbnail_string_path = profile_path + L"Skin\\" + fname + L"_" + fthumbname + L".png" ;
	m_thumbnail_path = thumbnail_string_path.c_str() ;

	boost::filesystem::rename(thumbnail_temp_string_path, m_thumbnail_path.GetString()) ;*/

	m_thumbnail_path = thumbnail_temp_string_path.c_str() ;
	m_image_thumbnail = Shared_ImageEx::FromFile(m_thumbnail_path) ;
}

//////////////////////////////////////////////////////////////////////////
// ISkin Interface

void CustomImageSkin::Draw(HDC hdc, int left, int top, int width, int height) const
{
	if (!m_initialized || !m_activated)
		return ;

	if (m_image_mode == Single)
	{
		// 获得皮肤范围和绘制区域的交集
		RECT skin_bound ;
		skin_bound.left = m_origin.x - m_few_edge_width ;
		skin_bound.top = m_origin.y - m_few_edge_width ;
		skin_bound.right = skin_bound.left + m_width ;
		skin_bound.bottom = skin_bound.top + m_height ;
		RECT draw_bound = {left, top, left + width, top + height} ;

		RECT intersect ;
		if (::IntersectRect(&intersect, &skin_bound, &draw_bound))
		{
			// 绘制交叉部分的皮肤
			RECT intersect_in_skin ;		// 交叉部分在皮肤范围中的位置
			::CopyRect(&intersect_in_skin, &intersect) ;
			::OffsetRect(&intersect_in_skin, - skin_bound.left, - skin_bound.top) ;

			::BitBlt(
				hdc,
				intersect.left, intersect.top, intersect.right - intersect.left, intersect.bottom - intersect.top,
				m_skin_dc,
				intersect_in_skin.left, intersect_in_skin.top,
				SRCCOPY
				) ;

			// 填充剩余区域
			COLORREF old_bkgnd_color = ::SetBkColor(hdc, m_padding_color) ;
			if (intersect.top > top)
			{
				RECT blank = {left, top, intersect.right, intersect.top} ;
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &blank, NULL, 0, NULL) ;
			}
			if (intersect.right < left + width)
			{
				RECT blank = {intersect.right, top, left + width, intersect.bottom} ;
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &blank, NULL, 0, NULL) ;
			}
			if (intersect.bottom < top + height)
			{
				RECT blank = {intersect.left, intersect.bottom, left + width, top + height} ;
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &blank, NULL, 0, NULL) ;
			}
			if (intersect.left > left)
			{
				RECT blank = {left, intersect.top, intersect.left, top + height} ;
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &blank, NULL, 0, NULL) ;
			}
			::SetBkColor(hdc, old_bkgnd_color) ;
		}
		else
		{
			COLORREF old_bkgnd_color = ::SetBkColor(hdc, m_padding_color) ;
			::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &draw_bound, NULL, 0, NULL) ;
			::SetBkColor(hdc, old_bkgnd_color) ;
		}
	}
	else if (m_image_mode == Tile)
	{
		int src_left = (left - m_origin.x) % m_width ;
		if (src_left < 0)
			src_left += m_width ;

		int dst_left = left ;
		int dst_right = dst_left + m_width - src_left ;

		BOOL horz_stop = FALSE ;
		for ( ; !horz_stop ; dst_left = dst_right, dst_right += m_width)
		{
			if (dst_right > left + width)
			{
				dst_right = left + width ;
				horz_stop = TRUE ;
			}

			int src_top = (top - m_origin.y) % m_height ;
			if (src_top < 0)
				src_top += m_height ;

			int dst_top = top ;
			int dst_bottom = dst_top + m_height - src_top ;

			BOOL vert_stop = FALSE ;
			for ( ; !vert_stop ; dst_top = dst_bottom, dst_bottom += m_height)
			{
				if (dst_bottom > top + height)
				{
					dst_bottom = top + height ;
					vert_stop = TRUE ;
				}
				::BitBlt(
					hdc,
					dst_left, dst_top, dst_right - dst_left, dst_bottom - dst_top,
					m_skin_dc,
					src_left, src_top,
					SRCCOPY
					) ;
				src_top = 0 ;
			}
			src_left = 0 ;
		}
	}
}

void CustomImageSkin::DrawThumbnail(HDC hdc, int left, int top, int width, int height) const 
{
	if (m_image_thumbnail->GetLastStatus() != Ok)
		return ;

	Graphics gph(hdc) ;
	gph.DrawImage(
		m_image_thumbnail,
		Rect(left, top, width, height),
		0, 0, m_image_thumbnail->GetWidth(), m_image_thumbnail->GetHeight(),
		UnitPixel, NULL
		) ;
}

BYTE CustomImageSkin::GetRoughGrayScale() const 
{
	if (!m_initialized)
		return 0 ;

	//RECT bound = {0, 0, m_width, m_height} ;
	//return hdutils::GetBkgndRoughGrayScale(m_img_dc, &bound) ;

	return m_rough_gray_scale ;
}

COLORREF CustomImageSkin::GetRoughColor() const 
{
    return m_padding_color ;
}

//////////////////////////////////////////////////////////////////////////
// DefaultSkin

DefaultSkin::DefaultSkin()
{
	m_guid = "DefaultSkin" ;
	m_image_base_wnd_bkgnd = Shared_ImageEx::FromId( IDI_BASE_WND_BKGND_PNG ) ;
}

void DefaultSkin::DrawThumbnail(HDC hdc, int left, int top, int width, int height) const 
{
	if (m_image_base_wnd_bkgnd->GetLastStatus() != Ok)
		return ;

	int image_height = m_image_base_wnd_bkgnd->GetHeight() ;

	Graphics gph(hdc) ;
	if (image_height < height)
	{
		gph.DrawImage(
			m_image_base_wnd_bkgnd,
			Rect(left, top, width, image_height),
			0, 0, 1, image_height, UnitPixel, NULL
			) ;

		SolidBrush sb(Color(255, 0xf3, 0xfb, 0xff)) ;
		gph.FillRectangle(&sb, left, top + image_height, width, height - image_height) ;
	}
	else
	{
		gph.DrawImage(
			m_image_base_wnd_bkgnd,
			Rect(left, top, width, height),
			0, 0, 1, height, UnitPixel, NULL
			) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// SolidColorSkin

SolidColorSkin::SolidColorSkin() 
{

}

SolidColorSkin::SolidColorSkin(COLORREF color) 
: m_color(color)
{
	char szguid[10] ;
	sprintf_s(szguid, sizeof(szguid), "%02X%02X%02X", GetRValue(m_color), GetGValue(m_color), GetBValue(m_color)) ;
	m_guid = szguid ;
}

BOOL SolidColorSkin::Load(boost::shared_ptr<IPropertyTree> config, std::string guid) 
{
	Skin::Load(config, guid) ;

	m_color = (COLORREF)config->getUInt(config_keys::skin_solid_color) ;

	return TRUE ;
}

BOOL SolidColorSkin::Save(boost::shared_ptr<IPropertyTree> config) 
{
	__super::Save(config) ;

	config->set(config_keys::skin_solid_color, (unsigned int)m_color) ;

	return TRUE ;
}

void SolidColorSkin::Draw(HDC hdc, int left, int top, int width, int height) const 
{
	if (width <= 0 || height <= 0)
		return ;

	COLORREF old_bkgnd_color = ::SetBkColor(hdc, m_color) ;

	RECT draw_bound = {left, top, left + width, top + height} ;
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &draw_bound, NULL, 0, NULL) ;

	::SetBkColor(hdc, old_bkgnd_color) ;
}

void SolidColorSkin::DrawThumbnail(HDC hdc, int left, int top, int width, int height) const 
{
	if (width <= 0 || height <= 0)
		return ;

	COLORREF old_bkgnd_color = ::SetBkColor(hdc, m_color) ;

	RECT draw_bound = {left, top, left + width, top + height} ;
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &draw_bound, NULL, 0, NULL) ;

	::SetBkColor(hdc, old_bkgnd_color) ;
}

BYTE SolidColorSkin::GetRoughGrayScale() const 
{
	return (BYTE)(0.30 * GetRValue(m_color) + 0.59 * GetGValue(m_color) + 0.11 * GetBValue(m_color)) ;
}

// 浅度拷贝，不复制为绘制做准备的数据
ISkin * SolidColorSkin::LightDuplicate() const 
{
	SolidColorSkin * copy_skin = new SolidColorSkin ;

	copy_skin->m_guid = m_guid ;
	copy_skin->m_color = m_color ;

	return copy_skin ;
}


namespace SkinHelper
{
	ISkin * Load(boost::shared_ptr<IPropertyTree> skin_list_config, std::string guid) 
	{
		boost::shared_ptr<IPropertyTree> skin_config = skin_list_config->createView(guid) ;

		ISkin * skin = NULL ;
		ISkin::Mode mode = (ISkin::Mode)skin_config->getInt(config_keys::skin_mode, -1) ;
		switch (mode)
		{
		case ISkin::Default :
			{
				skin = new DefaultImageSkin ; // modify DefaultSkin to DefaultImageSkin
				if (!skin->Load(skin_config, guid))
				{
					delete skin ;
					skin = NULL ;
				}
			}
			break ;

		case ISkin::SolidColor :
			{
				skin = new SolidColorSkin ;
				if (!skin->Load(skin_config, guid))
				{
					delete skin ;
					skin = NULL ;
				}
			}
			break ;

		case ISkin::CustomImage :
			{
				skin = new CustomImageSkin ;
				if (!skin->Load(skin_config, guid))
				{
					delete skin ;
					skin = NULL ;
				}
			}
			break ;
		}

		return skin ;
	}

	BOOL Save(boost::shared_ptr<IPropertyTree> skin_list_config, ISkin * skin) 
	{
		if (skin == NULL)
			return FALSE ;

		boost::shared_ptr<IPropertyTree> skin_config = skin_list_config->createView(skin->GetGUID()) ;

		ISkin::Mode mode = skin->GetMode() ;
		skin_config->set(config_keys::skin_mode, (int)mode) ;
		BOOL ret = skin->Save(skin_config);

		return  ret;
	}

	ISkin * GetLatestSkin(boost::shared_ptr<IPropertyTree> config) 
	{
		std::string recently_skin_guid = config->getString(config_keys::skin_latest) ;
		if (recently_skin_guid.empty())
			return NULL ;

		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;
		ISkin * recently_skin = Load(skin_list_config, recently_skin_guid) ;
		return recently_skin ;
	}

	void SetLatestSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) 
	{
		if (skin == NULL)
			return ;

		std::string guid = skin->GetGUID() ;
		config->set(config_keys::skin_latest, guid) ;

		// 判断skin是否已经被保存
		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;
		Save(skin_list_config, skin) ;
// 		if (!skin_list_config->hasProperty(guid))
// 		{
// 			// 保存skin
// 		}
// 		else
// 		{
// 			// 保存skin的时间
// 		}
	}

	bool CompareSkinUseTime(ISkin * lhs, ISkin * rhs)
	{
		if (lhs == NULL || rhs == NULL)
		{
			ASSERT(FALSE) ;
			return false ;
		}

		return lhs->GetTime() > rhs->GetTime() ;
	}

	void LoadRecentlySkinList(boost::shared_ptr<IPropertyTree> config, SkinList & skin_list) 
	{
		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;

		IPropertyTree::Keys key_list ;
		skin_list_config->keys(key_list) ;

		IPropertyTree::Keys::iterator pos ;
		for (pos = key_list.begin() ; pos != key_list.end() ; ++ pos)
		{
			ISkin * skin = Load(skin_list_config, (*pos)) ;
			if (skin != NULL)
				skin_list.push_back(skin) ;
		}

		// 对皮肤列表按时间进行从最近到最远排序
		skin_list.sort(CompareSkinUseTime) ;
	}

	void SaveRecentlySkinList(boost::shared_ptr<IPropertyTree> config, SkinList & skin_list) 
	{
		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;
		skin_list_config->clear() ;

		SkinList::iterator pos ;
		for (pos = skin_list.begin() ; pos != skin_list.end() ; ++ pos)
		{
			Save(skin_list_config, *pos) ;
		}
	}

	void AddSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) 
	{
		if (skin == NULL)
		{
			ASSERT(FALSE) ;
			return ;
		}

		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;
		Save(skin_list_config, skin) ;
	}

	// 删除皮肤
	void DeleteSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) 
	{
		if (skin == NULL)
		{
			ASSERT(FALSE) ;
			return ;
		}

		// 先从列表中删除
		boost::shared_ptr<IPropertyTree> skin_list_config = config->createView(config_keys::skin_recently_list) ;
		skin_list_config->unset(skin->GetGUID()) ;

		// 判断其是否最后使用的皮肤，如果是，则清除
		std::string latest_skin_guid = config->getString(config_keys::skin_latest) ;
		if (latest_skin_guid == skin->GetGUID())
			config->unset(config_keys::skin_latest) ;

		skin->Delete() ;
	}

} ;

///////////////////default image Skin//////////////////////////////////////////////////////////////
DefaultImageSkin::DefaultImageSkin()
:m_imgId( IDI_DEFAULT_SKIN_IMAGE_PNG )
{
	POINT origin = {0, 30} ;
	Create(_T(""),CustomImageSkin::Single, origin);
}

BOOL DefaultImageSkin::Activate()
{
	if (!m_initialized)
		return FALSE ;
	if (m_activated)
		return TRUE ;

	if (!InitializeContent( Shared_ImageEx::FromId( m_imgId )))
		return FALSE ;

	m_image_thumbnail = Shared_ImageEx::FromId( m_imgId );

	m_guid = "DefaultImageSkin";

	m_activated = TRUE ;
	return TRUE ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
