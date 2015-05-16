#include "stdafx.h"
#include "Image2.h"

Image2::Id_Ext Image2::s_Id2Ext_map[11] = 
{
    ImageFormatUndefined, L"",
    ImageFormatMemoryBMP, L".bmp",
    ImageFormatBMP, L".bmp",
    ImageFormatEMF, L".emf",
    ImageFormatWMF, L".wmf",
    ImageFormatJPEG, L".jpeg",
    ImageFormatPNG, L".png",
    ImageFormatGIF, L".gif",
    ImageFormatTIFF, L".tiff",
    ImageFormatEXIF, L".exit",
    ImageFormatIcon, L".ico",
} ;

Image2::Image2(
               IN const WCHAR* filename,
               GifCallback * call_back, 
               IN BOOL useEmbeddedColorManagement// = FALSE
               )
               : Image(filename, useEmbeddedColorManagement)
               , m_gif_call_back(call_back)
               , m_gif_timer_task(*this)
               , m_gif_frame_count(0)
               , m_gif_total_delay_time(0)
               , m_gif_status(STATUS_STOP) 
               , m_gif_play_time(0)
               , m_gif_play_index(0)
{
    // 分析gif中的帧数和每帧之间的间隔时间

    int frame_dimension_count = GetFrameDimensionsCount() ;
    if (frame_dimension_count > 0)
    {
        GUID * dimension_ids = new GUID[frame_dimension_count] ;
        GetFrameDimensionsList(dimension_ids, frame_dimension_count) ;
        m_gif_frame_count = GetFrameCount(dimension_ids) ;
        delete [] dimension_ids ;

        if (m_gif_frame_count > 1)
        {
            int property_item_size = GetPropertyItemSize(PropertyTagFrameDelay) ;
            PropertyItem * items = (PropertyItem*) new char[property_item_size] ;
            GetPropertyItem(PropertyTagFrameDelay, property_item_size, items) ;

            UINT i, temp_delayed_time ;
            for (i = 0 ; i < m_gif_frame_count ; ++ i)
            {
                temp_delayed_time = *((UINT*)(items->value) + i) ;
                if (temp_delayed_time < 5) temp_delayed_time = 10 ;
                m_gif_frame_delay_times.push_back(temp_delayed_time * 10) ;
                m_gif_total_delay_time += (temp_delayed_time * 10) ;
            }

            delete [] items ;

            SelectActiveFrame(0) ;
        }
    }
}

// static
Image2 * Image2::FromFile(
                          IN const WCHAR* filename,
                          GifCallback * call_back/* = NULL*/, 
                          IN BOOL useEmbeddedColorManagement/* = FALSE*/ 
                          ) 
{
    return new Image2(filename, call_back, useEmbeddedColorManagement) ;
}

Image2::~Image2() 
{

}

bool Image2::IsGif() const 
{
    return m_gif_frame_count > 1 ;
}

void Image2::SetGifCallBack(GifCallback * call_back) 
{
    if (!IsGif())
        return ;

    m_gif_call_back = call_back ;
}

void Image2::Play() 
{
    if (!IsGif())
        return ;

    if (m_gif_status == STATUS_PLAY)
        return ;

    m_gif_status = STATUS_PLAY ;
    m_gif_current_frame_index = 0 ;

    // 记录下开始播放的时间和帧数
    m_gif_play_time = GetTickCount() ;
    SelectActiveFrame(0) ;

    m_gif_timer_task.StartTask(m_gif_frame_delay_times[0]) ;
}

void Image2::Pause() 
{
    if (!IsGif())
        return ;

    if (m_gif_status == STATUS_PAUSE || m_gif_status == STATUS_STOP)
        return ;

    m_gif_status = STATUS_PAUSE ;
    m_gif_timer_task.StopTask() ;
}

void Image2::Continue() 
{
    if (!IsGif())
        return ;

    if (m_gif_status != STATUS_PAUSE)
        return ;

    ASSERT(m_gif_current_frame_index < m_gif_frame_delay_times.size()) ;
    m_gif_timer_task.StartTask(m_gif_frame_delay_times[m_gif_current_frame_index]) ;

    m_gif_status = STATUS_PLAY ;
}

void Image2::Stop() 
{
    if (!IsGif())
        return ;

    if (m_gif_status == STATUS_STOP)
        return ;

    SelectActiveFrame(0) ;
    if (m_gif_status == STATUS_PLAY)
        m_gif_timer_task.StopTask() ;

    m_gif_status = STATUS_STOP ;
}

bool Image2::SelectActiveFrame(UINT index) 
{
    if (index >= m_gif_frame_delay_times.size())
        return false ;

    m_gif_current_frame_index = index ;
    return Ok == __super::SelectActiveFrame(&Gdiplus::FrameDimensionTime, index) ;
}

// 未完成接口，用记录原始文件方式实现
// 按原格式保存文件
bool Image2::ToFile(std::wstring const &dir, std::wstring const &fname) 
{
    // 组合全路径
    if (dir.empty() || fname.empty())
    {
        ASSERT(FALSE) ;
        return false ;
    }

    std::wstring path ;
    path.append(dir) ;
    wchar_t trail = path.at(path.length() - 1) ;
    if (trail != L'\\' && trail != L'/')
        path.append(L"\\") ;
    path.append(fname) ;

    // 获得当前编码格式
    GUID codec_format = {0} ;
    if (Ok != GetRawFormat(&codec_format))
        return false ;

    std::wstring ext ;
    if (!GetExtByCodecFormat(codec_format, ext))
        return false ;
    path.append(ext) ;

    return true ;
}

// 根据编码格式获得扩展名 - static 
bool Image2::GetExtByCodecFormat(GUID const & format_id, std::wstring & ext) 
{
    int i ;
    for (i = _countof(s_Id2Ext_map) - 1 ; i >= 0 ; -- i)
    {
        if (IsEqualGUID(format_id, s_Id2Ext_map[i].id))
        {
            ext = s_Id2Ext_map[i].ext ;
            return true ;
        }
    }
    return false ;
}

bool Image2::DoDelayedWork(UINT & next_delayed_work_time) 
{
    if (m_gif_call_back == NULL || !IsGif() || !IsPlaying())
        return false ;

    // 计算当前需要绘制哪一帧以及推迟多少毫秒后绘制下一帧
    UINT current_time = GetTickCount() ;
    UINT elapsed_time = (current_time - m_gif_play_time) % m_gif_total_delay_time ;

    UINT i = m_gif_play_index ;
    while (elapsed_time > 0)
    {
        UINT frame_delay_time = m_gif_frame_delay_times[i] ;
        if (elapsed_time >= frame_delay_time)
        {
            elapsed_time -= frame_delay_time ;
            i = (i + 1) % m_gif_frame_delay_times.size() ;
        }
        else
            break ;
    }
    //TRACE(_T("et=%u, idx=%u\n"), elapsed_time, i) ;
    SelectActiveFrame(i) ;
    next_delayed_work_time = m_gif_frame_delay_times[i] - elapsed_time ;

    return m_gif_call_back->Update() ;
}

//////////////////////////////////////////////////////////////////////////
// Image2::GifTimerTask

Image2::GifTimerTask::GifTimerTask(Image2 &gif)
: m_gif(gif)
{
    SetPump(TimerTaskPump::GetPump()) ;
}

bool Image2::GifTimerTask::DoDelayedWork(UINT & next_delayed_work_time)
{
    return m_gif.DoDelayedWork(next_delayed_work_time) ;
}


//////////////////////////////////////////////////////////////////////////
// ImageDelegate

ImageDelegate::ImageDelegate(
                   IN const WCHAR* filename,
                   IN BOOL useEmbeddedColorManagement// = FALSE
                   ) 
                   : m_image(filename, this, useEmbeddedColorManagement)
                   , m_host(NULL)
{

}

void ImageDelegate::SetWinHost(HWND host) 
{
    m_host = host ;
}

void ImageDelegate::SetBound(int x, int y, int width, int height) 
{
    ::SetRect(&m_bound, x, y, x + width, y + height) ;
}

void ImageDelegate::GetBound(RECT &bound) const
{
    ::CopyRect(&bound, &m_bound) ;
}

void ImageDelegate::Draw(Graphics & gph, RECT const &update_bound) 
{
    gph.DrawImage(
        &m_image,
        Rect(m_bound.left, m_bound.top, m_bound.right - m_bound.left, m_bound.bottom - m_bound.top),
        0, 0, m_image.GetWidth(), m_image.GetHeight(), UnitPixel, NULL
        ) ;

/*    RECT temp_bound ;
    std::vector<RECT>::iterator iter ;
    for (iter = m_bound_group.begin() ; iter != m_bound_group.end() ; ++ iter)
    {
        if (::IntersectRect(&temp_bound, &update_bound, &(*iter)))
        {
            gph.DrawImage(
                &m_image,
                Rect(iter->left, iter->top, iter->right - iter->left, iter->bottom - iter->top),
                0, 0, m_image.GetWidth(), m_image.GetHeight(), UnitPixel, NULL
                ) ;
        }
    }*/
}

void ImageDelegate::Draw(HDC dc) 
{
    Graphics gph(dc) ;
    gph.DrawImage(
        &m_image,
        Rect(m_bound.left, m_bound.top, m_bound.right - m_bound.left, m_bound.bottom - m_bound.top),
        0, 0, m_image.GetWidth(), m_image.GetHeight(), UnitPixel, NULL
        ) ;
}

void ImageDelegate::AddBound(int x, int y, int width, int height) 
{
    RECT bound = {x, y, x + width, y + height} ;
    m_bound_group.push_back(bound) ;
}

std::vector<RECT> const & ImageDelegate::GetBound() const
{
    return m_bound_group ;
}

// Delegate Image2 Method
void ImageDelegate::Play() { m_image.Play() ; }
void ImageDelegate::Pause() { m_image.Pause() ; }
void ImageDelegate::Continue() { m_image.Continue() ; }
void ImageDelegate::Stop() { m_image.Stop() ; }

// GifCallback Implement - virtual
bool ImageDelegate::Update() 
{
    if (m_host == NULL)
    {
        ASSERT(FALSE) ;
    }

    ::InvalidateRect(m_host, &m_bound, FALSE) ;
    return true ;
    /*std::vector<RECT>::iterator iter ;
    for (iter = m_bound_group.begin() ; iter != m_bound_group.end() ; ++ iter)
    {
        ::InvalidateRect(m_host, &(*iter), FALSE) ;
    }*/
}
