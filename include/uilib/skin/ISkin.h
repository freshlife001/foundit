#pragma once

#include "uilib/uilib_exports.h"

#include "boost/shared_ptr.hpp"
class IPropertyTree;
class ISkin
{
public:
	enum Mode
	{
		Default = 0,
		SolidColor = 1,
		CustomImage = 2,
		DefaultImage,
	};

	virtual ~ISkin() {}

	virtual std::string GetGUID() const = 0 ;

	// 从配置加载
	// 由于guid是作为皮肤配置的key，所以会导致load和save接口的不一致
	virtual BOOL Load(boost::shared_ptr<IPropertyTree> config, std::string guid) = 0 ;

	virtual BOOL Save(boost::shared_ptr<IPropertyTree> config) = 0 ;

	virtual BOOL Delete() = 0 ;

	virtual Mode GetMode() const = 0 ;

	// 绘制，后面的参数是更新区域
	virtual void Draw(HDC hdc, int left, int top, int width, int height) const = 0 ;

	// 绘制缩略图，后面的参数是整个绘制区域
	virtual void DrawThumbnail(HDC hdc, int left, int top, int width, int height) const = 0 ;

	// 获得大致的灰度，供绘制发光字体用
	virtual BYTE GetRoughGrayScale() const = 0 ;

    // 获得大致的颜色
    virtual COLORREF GetRoughColor() const = 0 ;

	// 获得使用时间
	virtual time_t GetTime() const = 0 ;

	// 更新使用时间
	virtual void UpdateUseTime() = 0 ;

	// 激活，生成为绘制做准备的数据，如果不需要则啥都不做
	virtual BOOL Activate() = 0 ;

	// 反激活，清除为绘制而准备的数据
	virtual BOOL Deactivate() = 0 ;

	// 浅度拷贝，不复制为绘制做准备的数据
	virtual ISkin * LightDuplicate() const = 0 ;

};


class ISkinMgr
{
public:
	virtual ~ISkinMgr() {};
	virtual ISkin const & GetSkin() const = 0 ;
	virtual void RegisterSkinWindow(HWND wnd) = 0 ;
	virtual BOOL UnRegisterSkinWindow(HWND wnd) = 0 ;
};


UILIB_EXPORT ISkinMgr * GetSkinMgr();


