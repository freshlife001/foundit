#pragma once

#include <list>

#include "skin\ISkin.h"

#include "common\utils\ImageEx\Shared_ImageEx.h"
#include "common\utils\CanvasDC.h"

class Skin : public ISkin
{
public:
	Skin() ;

	//virtual void SetGUID(std::string const &guid) { m_guid = guid ; }
	virtual std::string GetGUID() const { return m_guid ; }

	// 从配置加载
	virtual BOOL Load(boost::shared_ptr<IPropertyTree> config, std::string guid) ;

	virtual BOOL Save(boost::shared_ptr<IPropertyTree> config) ;

	// 获得使用时间
	virtual time_t GetTime() const { return m_use_time ; }

	// 更新使用时间
	virtual void UpdateUseTime() { m_use_time = ::_time64(NULL) ; }

	// 激活，生成为绘制做准备的数据，如果不需要则啥都不做
	virtual BOOL Activate() { return TRUE ; }

	// 反激活，清除为绘制而准备的数据
	virtual BOOL Deactivate() { return TRUE ; }

	// 浅度拷贝，不复制为绘制做准备的数据
	virtual ISkin * LightDuplicate() const { return NULL ; }

protected:
	std::string m_guid ;
	time_t m_use_time ;
};

class CustomImageSkin : public Skin
{
public:
	enum ImageMode
	{
		Single = 0,
		Tile = 1,
	} ;

public:
	CustomImageSkin(void);
	~CustomImageSkin(void);

	// 初始化

	// 从配置加载创建
	// 还未激活
	virtual BOOL Load(boost::shared_ptr<IPropertyTree> config, std::string guid) ;

	// 用户拖拽文件创建
	// 当时就立即激活了
	BOOL Create(CString const &img_path, ImageMode mode, POINT const &origin) ;

	// 保存皮肤配置
	virtual BOOL Save(boost::shared_ptr<IPropertyTree> config) ;

	// 删除皮肤配置
	virtual BOOL Delete() ;

	// 激活皮肤，即根据皮肤图片生成m_skin_dc
	virtual BOOL Activate() ;

	// 反激活皮肤，销毁m_skin_dc
	virtual BOOL Deactivate() ;

	// 浅度拷贝，不复制为绘制做准备的数据
	virtual ISkin * LightDuplicate() const ;

	inline POINT GetOrigin() const { return m_origin ; }
	void SetOrigin(int x, int y) 
	{
		m_origin.x = x ;
		m_origin.y = y ;
	}

	void SetThumbnailSize(int width, int height) 
	{
		if (width > 0) m_thumbnail_size.cx = width ;
		if (height > 0) m_thumbnail_size.cy = height ;
	}

	inline int GetWidth() const { return m_width ; }
	inline int GetHeight() const { return m_height ; }
	inline int GetImageWidth() const { return m_image_width ; }
	inline int GetImageHeight() const { return m_image_height ; }

	inline int GetLeft() const { return m_origin.x - ((m_image_mode == Single) ? m_few_edge_width : 0) ; }
	inline int GetTop() const { return m_origin.y - ((m_image_mode == Single) ? m_few_edge_width : 0) ; }
	inline int GetRight() const { return m_origin.x + m_image_width ; }
	inline int GetBottom() const { return m_origin.y + m_image_height ; }

	inline void SetLeft(int left) { m_origin.x = left + ((m_image_mode == Single) ? m_few_edge_width : 0) ; }
	inline void SetTop(int top) { m_origin.y = top + ((m_image_mode == Single) ? m_few_edge_width : 0) ; }
	inline void SetRight(int right) { m_origin.x = right - m_image_width ; }
	inline void SetBottom(int bottom) { m_origin.y = bottom - m_image_height ; }

	inline ImageMode GetImgMode() const { return m_image_mode ; }
	void SetImgMode(ImageMode mode) ;

	// 保存图片文件
	void SaveImageFile() ;

	//////////////////////////////////////////////////////////////////////////
	// ISkin Interface

	virtual Mode GetMode() const { return CustomImage ; }
	virtual void Draw(HDC hdc, int left, int top, int width, int height) const ;	// 这里的left,top,width,height是更新区域
	virtual void DrawThumbnail(HDC hdc, int left, int top, int width, int height) const ;
    virtual BYTE GetRoughGrayScale() const ;
    virtual COLORREF GetRoughColor() const ;

protected:
	BOOL InitializeContent( Shared_ImageEx& img ) ;
	void UnInitializeContent() ;

protected:
	BOOL m_initialized ;				// TRUE表示已初始化，FALSE表示未初始化
	BOOL m_activated ;					// TRUE表示已激活
	//BOOL m_init_from_load_or_create ;	// TRUE表示是从配置加载，FALSE表示用户创建

	CString m_image_path ;
	ImageMode m_image_mode ;
	POINT m_origin ;
	CString m_name ;
	CString m_thumbnail_path ;
	SIZE m_thumbnail_size ;			// 临时记录变量，不作为皮肤属性，根据缩略图图片即可得

	CCanvasDC m_skin_dc ;			// 皮肤dc
	COLORREF m_padding_color ;		// 填充色
	int m_few_edge_width ;			// 模糊边缘宽度

	int m_width ;					// 皮肤宽度
	int m_height ;					// 皮肤高度

	int m_image_width ;				// 图片宽度
	int m_image_height ;			// 图片高度

	BYTE m_rough_gray_scale ;

	Shared_ImageEx m_image_thumbnail ;

};

//////////////Default Image Skin//////////////////////
class DefaultImageSkin : public CustomImageSkin
{
public:
	DefaultImageSkin();
	//{
	//	POINT origin = {0, 0} ;
	//	Create(_T("anniversary_bkgnd.jpg"),CustomImageSkin::Single, origin);
	//	m_image_thumbnail = Shared_ImageEx::FromId(_T("anniversary_bkgnd.jpg"));
	//	m_guid = "DefaultImageSkin";
	//}
	virtual BOOL Activate() ;
	virtual void SetGUID(std::string const &/*guid*/) { /* do nothing !!! */ }
	virtual BOOL Load(boost::shared_ptr<IPropertyTree> config, std::string guid) {return TRUE; };
	virtual BOOL Save(boost::shared_ptr<IPropertyTree> config) {return TRUE; };
	virtual BOOL Delete() {return TRUE; };
	virtual Mode GetMode() const { return DefaultImage; }
	ISkin * LightDuplicate() const 
	{
		DefaultImageSkin * copy_skin = new DefaultImageSkin ;
		return copy_skin ;
	}

private:
	UINT m_imgId;
};
//////////////////////////////////////////////////////////

class DefaultSkin : public Skin
{
public:
	DefaultSkin() ;

	//////////////////////////////////////////////////////////////////////////
	// ISkin Interface

	virtual void SetGUID(std::string const &/*guid*/) { /* do nothing !!! */ }

	virtual Mode GetMode() const { return Default ; }

	// 默认皮肤的绘制由各个窗口自行决定，不提供具体绘制
	virtual void Draw(HDC /*hdc*/, int /*left*/, int /*top*/, int /*width*/, int /*height*/) const { ASSERT(FALSE) ; }

	virtual void DrawThumbnail(HDC hdc, int left, int top, int width, int height) const ;

    virtual BYTE GetRoughGrayScale() const { ASSERT(FALSE) ; return 0 ;}
    virtual COLORREF GetRoughColor() const { ASSERT(FALSE) ; return 0 ; }

	// 删除默认皮肤没有意义
	virtual BOOL Delete() { return FALSE ; }

	ISkin * LightDuplicate() const { return new DefaultSkin ; }

protected:
	Shared_ImageEx m_image_base_wnd_bkgnd ;

};

class SolidColorSkin : public Skin
{
public:
	SolidColorSkin() ;
	SolidColorSkin(COLORREF color) ;

	COLORREF GetColor() const { return m_color ; }

	//////////////////////////////////////////////////////////////////////////
	// ISkin Interface

	virtual BOOL Load(boost::shared_ptr<IPropertyTree> config, std::string guid) ;
	virtual BOOL Save(boost::shared_ptr<IPropertyTree> config) ;
	virtual Mode GetMode() const { return SolidColor ; }
	virtual void Draw(HDC hdc, int left, int top, int width, int height) const ;
	virtual void DrawThumbnail(HDC hdc, int left, int top, int width, int height) const ;
    virtual BYTE GetRoughGrayScale() const ;
    virtual COLORREF GetRoughColor() const { return m_color ; }
	virtual BOOL Delete() { return FALSE ; }

	ISkin * LightDuplicate() const ;

private:
	COLORREF m_color ;

};

namespace SkinHelper
{
	//ISkin * Load(boost::shared_ptr<IPropertyTree> config) ;
	//BOOL Save(boost::shared_ptr<IPropertyTree> config, ISkin * skin) ;

	// 获得最后一次使用的皮肤
	ISkin * GetLatestSkin(boost::shared_ptr<IPropertyTree> config) ;
	void SetLatestSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) ;

	typedef std::list<ISkin*> SkinList ;
	void LoadRecentlySkinList(boost::shared_ptr<IPropertyTree> config, SkinList & skin_list) ;
	//void SaveRecentlySkinList(boost::shared_ptr<IPropertyTree> config, SkinList & skin_list) ;

	// 添加皮肤
	void AddSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) ;

	// 删除皮肤
	void DeleteSkin(boost::shared_ptr<IPropertyTree> config, ISkin * skin) ;

} ;
