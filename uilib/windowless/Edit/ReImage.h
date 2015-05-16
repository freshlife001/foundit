//////////////////////////////////////////////////////////////////////////
//
//	描述:   可嵌入RichEdit的图像对象类
//
//	
//	日期:   2011/11/23
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "common/utils/Image2/Image2.h"

#include "ReObject.h"

class ImageReDelegate ;

class ReImage : public ReObject
{
public:
    ReImage() ;
    virtual ~ReImage() ;

public:

    void InitFromImageFile(std::wstring const &path, UINT space_width = (UINT)-1) ;
    bool IsGif() const ;

    // 根据图片显示空间宽度计算图片对象大小
    bool CalcSizeBySpaceWidth(UINT space_width) ;

    // 根据图片显示空间宽度自动调整图片对象大小
    // gif不进行调整
    void AutoAdjustSizeBySpaceWidth(UINT space_width) ;

    // 获得原始图片路径
    void GetOriginFilePath(std::wstring & path) ;

public:
    //////////////////////////////////////////////////////////////////////////
    // Internal methods
    // 供tomEdit和ImageDelegate使用

    // 对象移出了编辑框的可视区域
    virtual void MoveOutOfVisibleArea() ;

    // 对象移入了编辑框的可视区域
    virtual void MoveIntoVisibleArea() ;

    // 这个接口提供给动画任务对象以重绘
    bool Redraw() ;

public:

    //////////////////////////////////////////////////////////////////////////
    // ReObject methods
    // 绘制
    virtual void Draw(HDC hdcDraw, RECT const &draw_bound) ;

    //////////////////////////////////////////////////////////////////////////
    //IOleObject methods
    virtual HRESULT __stdcall Close( DWORD dwSaveOption );

    //////////////////////////////////////////////////////////////////////////
    //IDataObject methods
    virtual HRESULT __stdcall GetData( FORMATETC *pFormatetcIn, STGMEDIUM *pMedium ) ;
    virtual HRESULT __stdcall EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc ) ;

    //////////////////////////////////////////////////////////////////////////
    // IClipboardDataObject methods

    // 转换成剪切板数据
    virtual bool QueryTextStreamClipboardData(
        Clipboard::Format format,
        std::wstring &data
        ) ;

protected:
    ImageReDelegate * m_image ;
    std::wstring m_image_file_origin_path ; // 图像文件原始路径
    UINT m_space_width ;        // 图片所在的空间宽度
};

class ImageReDelegate : public GifCallback
{
public:
    ImageReDelegate(
        IN const WCHAR* filename,
        IN BOOL useEmbeddedColorManagement = FALSE
        ) ;

    void SetReImage(ReImage * reimage) ;
    void SetBound(int x, int y, int width, int height) ;
    void GetBound(RECT &bound) const ;
    void Draw(HDC dc, RECT const &update_bound) ;

    //////////////////////////////////////////////////////////////////////////
    // Delegate Image2 Method
    bool IsGif() const ;
    void Play() ;
    void Pause() ;
    void Continue() ;
    void Stop() ;
    UINT GetWidth() ;
    UINT GetHeight() ;
    Status GetLastStatus() const ;

    operator Gdiplus::Image * () ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // GifCallback Implement
    virtual bool Update() ;

private:
    Image2 m_image ;

    ReImage * m_object ;
    RECT m_bound ;      // 这个坐标必须是参照host的客户区坐标系

    std::vector<RECT> m_bound_group ;

};
