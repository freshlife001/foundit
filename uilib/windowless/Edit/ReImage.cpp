#include "stdafx.h"

#include "common/utils/Graphics.h"

#include "utils.h"

#include "ReImage.h"
#include "EnumFormatEtcImpl.h"
#include "common/MemoryLeakChecker.h"
ReImage::ReImage()
{
    m_image = NULL ;
    m_space_width = 0 ;
}

ReImage::~ReImage()
{
    delete m_image ;
    m_image = NULL ;
}

void ReImage::InitFromImageFile(std::wstring const &path, UINT space_width /*= (UINT)-1*/) 
{
    m_image_file_origin_path = path ;
    m_image = new ImageReDelegate(path.c_str()) ;
    m_image->SetReImage(this) ;

    if (m_image->IsGif())
    {
        CalcSizeBySpaceWidth(-1) ;
        m_image->Play() ;
    }
    else
    {
        CalcSizeBySpaceWidth(space_width) ;
    }
}

bool ReImage::IsGif() const 
{
    if (m_image == NULL)
        return false ;

    return m_image->IsGif() ;
}

// 返回true表示图像大小发生变化
bool ReImage::CalcSizeBySpaceWidth(UINT space_width) 
{
    if (m_image == NULL)
        return false ;

    if (m_space_width == space_width)
        return false ;
    m_space_width = space_width ;

    int width, height ;
    if (space_width >= m_image->GetWidth())
    {
        width = m_image->GetWidth() ;
        height = m_image->GetHeight() ;
    }
    else
    {
        width = space_width ;
        height = width * m_image->GetHeight() / m_image->GetWidth() ;
    }

    if (width == m_width_in_pixel && height == m_height_in_pixel)
        return false ;

    // 对象宽高比图像原始宽高各多2个像素，以防止当Caret在对象左侧时和对象重叠
    m_width_in_pixel = width + 2 ;
    m_height_in_pixel = height + 2 ;

    m_width_in_himetric = DXtoHimetricX(m_width_in_pixel, s_x_pixel_per_inch) ;
    m_height_in_himetric = DYtoHimetricY(m_height_in_pixel, s_y_pixel_per_inch) ;

    return true ;
}

// space_width为-1表示不限制图片的显示空间
void ReImage::AutoAdjustSizeBySpaceWidth(UINT space_width) 
{
    if (m_image->IsGif())
        return ;

    if (!CalcSizeBySpaceWidth(space_width))
        return ;

    if (m_advise_sink != NULL)
    {
        m_advise_sink->OnViewChange(DVASPECT_CONTENT, -1) ;
    }
}

// 获得原始图片路径
void ReImage::GetOriginFilePath(std::wstring & path) 
{
    path = m_image_file_origin_path ;
}

// 对象移出了编辑框的可视区域 - virtual
void ReImage::MoveOutOfVisibleArea() 
{
    if (m_image != NULL)
    {
        // 停止播放动画
        m_image->Stop() ;
    }
}

// 对象移入了编辑框的可视区域 - virtual
void ReImage::MoveIntoVisibleArea() 
{
    if (m_image != NULL)
    {
        // 开始播放动画
        m_image->Play() ;
    }
}

bool ReImage::Redraw() 
{
    IOleClientSite * client_site = NULL ;
    GetClientSite(&client_site) ;

    IOleInPlaceSite * inplace_site = NULL ;
    client_site->QueryInterface(IID_IOleInPlaceSite, (void**)&inplace_site) ;

    RECT bound, clip ;
    HRESULT hr = inplace_site->GetWindowContext(NULL, NULL, &bound, &clip, NULL) ;

    // 因为对象区域比图像区域大
    ::InflateRect(&bound, -1, -1) ;

    RECT intersect ;
    if (::IntersectRect(&intersect, &bound, &clip))
    {
        ::InvalidateRect(m_host, &intersect, FALSE) ;
        return true ;
    }
    else
    {
        return false ;
    }
}

//////////////////////////////////////////////////////////////////////////
// ReObject methods
// 绘制 - virtual 
void ReImage::Draw(HDC hdcDraw, RECT const &draw_bound) 
{
    if (m_image != NULL)
    {
        Graphics gph(hdcDraw) ;
        gph.DrawImage(
            *(m_image),
            Rect(
                draw_bound.left + 1, 
                draw_bound.top + 1, 
                draw_bound.right - draw_bound.left - 2, 
                draw_bound.bottom - draw_bound.top - 2
                ),
//             Rect(
//                 draw_bound.left, 
//                 draw_bound.top, 
//                 draw_bound.right - draw_bound.left, 
//                 draw_bound.bottom - draw_bound.top
//                 ),
            0, 0, m_image->GetWidth(), m_image->GetHeight(), UnitPixel, NULL
            ) ;
    }
}

//////////////////////////////////////////////////////////////////////////
//IOleObject methods

HRESULT __stdcall ReImage::Close( DWORD dwSaveOption )
{
    if (m_image != NULL)
        m_image->Stop() ;

    return S_OK ;
}

//////////////////////////////////////////////////////////////////////////
//IDataObject methods

HRESULT __stdcall ReImage::GetData( FORMATETC *pFormatetcIn, STGMEDIUM *pMedium ) 
{
    switch (pFormatetcIn->cfFormat)
    {
    case CF_BITMAP :
        {
            Bitmap bmp(m_image_file_origin_path.c_str()) ;
            if (bmp.GetLastStatus() != Ok)
                return E_NOTIMPL ;

            pMedium->tymed = TYMED_GDI ;
            pMedium->pUnkForRelease = NULL ;
            Color bkgnd(0, 0, 0, 0) ;
            if (Ok != bmp.GetHBITMAP(bkgnd, &pMedium->hBitmap))
                return E_NOTIMPL ;

            return S_OK ;
        }
        break ;

    case CF_DIB :
        {
            Bitmap bmp(m_image_file_origin_path.c_str()) ;
            if (bmp.GetLastStatus() != Ok)
                return E_NOTIMPL ;

            Color bkgnd(0, 0, 0, 0) ;

            HBITMAP hbmp ;
            if (Ok != bmp.GetHBITMAP(bkgnd, &hbmp))
                return E_NOTIMPL ;

            HGLOBAL mem = hdutils::ConvertDDB2DIB(hbmp) ;
            if (mem == NULL)
                return E_NOTIMPL ;

            pMedium->tymed = TYMED_HGLOBAL ;
            pMedium->pUnkForRelease = NULL ;
            pMedium->hGlobal = mem ;

            return S_OK ;
        }

    case CF_HDROP :
        {
            // 克隆一份文件
            //wchar_t file_path[] = L"C:\\020.gif\0" ;  // 这里的路径一定要用反斜杆
            HGLOBAL mem = ::GlobalAlloc(
                GMEM_MOVEABLE | GMEM_ZEROINIT,
                sizeof(DROPFILES) + (m_image_file_origin_path.length() + 1 + 1) * sizeof(wchar_t)
                ) ;

            DROPFILES * df = (DROPFILES*)::GlobalLock(mem) ;
            df->pFiles = sizeof(DROPFILES) ;
            df->fWide = TRUE ;

            memcpy(df + 1, m_image_file_origin_path.c_str(), (m_image_file_origin_path.length()) * sizeof(wchar_t)) ;

            ::GlobalUnlock(mem) ;

            pMedium->tymed = TYMED_HGLOBAL ;
            pMedium->pUnkForRelease = NULL ;
            pMedium->hGlobal = mem ;

            return S_OK ;
        }
        break ;

    default:
        if (pFormatetcIn->cfFormat == CF_HTML)
        {
            pMedium->tymed = TYMED_HGLOBAL ;
            pMedium->pUnkForRelease = NULL ;

//             RichTextClipboardData cd(true) ;
//             cd.Append(RichTextClipboardData::Type_Image, m_image_file_origin_path) ;

            Clipboard::HtmlPacker packer ;
            std::wstring data ;
            QueryTextStreamClipboardData(Clipboard::Html, data) ;
            packer.Append(Clipboard::Ele_Image, data) ;

            pMedium->hGlobal = packer.GenerateClipboardData() ;
            return S_OK ;
        }
        // 这个可以改变复制后的文件的文件名
/*        else if (pFormatetcIn->cfFormat == CF_FILENAMEMAP)
        {
            wchar_t file_path[] = L"1.gif\0" ;
            HGLOBAL mem = ::GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(file_path)) ;

            wchar_t * pdata = (wchar_t *)::GlobalLock(mem) ;

            memcpy(pdata, file_path, sizeof(file_path)) ;

            ::GlobalUnlock(mem) ;

            pMedium->tymed = TYMED_HGLOBAL ;
            pMedium->pUnkForRelease = NULL ;
            pMedium->hGlobal = mem ;
            return S_OK ;
        }*/
    }

    // 不存在的类型一定要返回E_NOTIMPL，否则可能引起接收数据的程序崩溃
    // 比如往文件夹里粘帖文件时导致explorer.exe崩溃
    return E_NOTIMPL ;
}

HRESULT __stdcall ReImage::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc ) 
{
    *ppEnumFormatEtc = NULL;
    if ( dwDirection == DATADIR_GET )
    {
        // 图像对象，要提供CF_BITMAP/CF_DIB、CF_HTML、CF_HDROP和内部标准格式
        // 这里严格要求cfFormat字段和tymed字段的正确匹配
        FORMATETC fetc[3] = 
        {
            {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}, 
            {CF_HTML, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}, 
            {CF_DIB, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}, 
        } ;

        *ppEnumFormatEtc = new EnumFormatEtcImpl(3, fetc) ;

        if ( *ppEnumFormatEtc )
        {
            (*ppEnumFormatEtc)->AddRef();
        }
    }
    return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// IClipboardDataObject methods

// 转换成剪切板数据
bool ReImage::QueryTextStreamClipboardData(
    Clipboard::Format format,
    std::wstring &data
    ) 
{
    switch (format)
    {
    case Clipboard::Text :
        {
            data = L" " ;
        }
        break ;

    case Clipboard::Html :
        {
            data.append(L"<img src=\"file://") ;    // 这里加 file:// 是为了让QQ能识别
            data.append(m_image_file_origin_path) ;
            data.append(L"\" />") ;
        }
        break ;

    default:
        return false ;
    }

    return true ;
}

//////////////////////////////////////////////////////////////////////////
// ImageReDelegate 

ImageReDelegate::ImageReDelegate(
                                 IN const WCHAR* filename,
                                 IN BOOL useEmbeddedColorManagement// = FALSE
                                 ) 
                                 : m_image(filename, this, useEmbeddedColorManagement)
                                 , m_object(NULL)
{

}

void ImageReDelegate::SetReImage(ReImage * reimage) 
{
    m_object = reimage ;
}

void ImageReDelegate::SetBound(int x, int y, int width, int height) 
{
    ::SetRect(&m_bound, x, y, x + width, y + height) ;
}

void ImageReDelegate::GetBound(RECT &bound) const
{
    ::CopyRect(&bound, &m_bound) ;
}

void ImageReDelegate::Draw(HDC dc, RECT const &update_bound) 
{
    Graphics gph(dc) ;
    gph.DrawImage(
        &m_image,
        Rect(m_bound.left, m_bound.top, m_bound.right - m_bound.left, m_bound.bottom - m_bound.top),
        0, 0, m_image.GetWidth(), m_image.GetHeight(), UnitPixel, NULL
        ) ;
}

// Delegate Image2 Method
bool ImageReDelegate::IsGif() const { return m_image.IsGif() ; }
void ImageReDelegate::Play() { m_image.Play() ; }
void ImageReDelegate::Pause() { m_image.Pause() ; }
void ImageReDelegate::Continue() { m_image.Continue() ; }
void ImageReDelegate::Stop() { m_image.Stop() ; }
UINT ImageReDelegate::GetWidth() { return m_image.GetWidth() ; }
UINT ImageReDelegate::GetHeight() { return m_image.GetHeight() ; }
Status ImageReDelegate::GetLastStatus() const { return m_image.GetLastStatus() ; }

ImageReDelegate::operator Gdiplus::Image * () 
{
    return &m_image ;
}

// GifCallback Implement - virtual
bool ImageReDelegate::Update() 
{
    if (m_object == NULL)
    {
        ASSERT(FALSE) ;
    }

    bool hold = m_object->Redraw() ;
    if (!hold) 
        m_image.Stop() ;
    return hold ;
}
