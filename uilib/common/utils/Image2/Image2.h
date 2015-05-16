#pragma once

#include <vector>
#include <string>
#include "TimerTaskPump.h"

class GifCallback
{
public:
    virtual bool Update() = 0 ;     // 返回true，则继续播放动画，否则停止
};

class Image2 : public Gdiplus::Image
{
public:
    enum GIF_STATUS
    {
        STATUS_PLAY = 0,
        STATUS_PAUSE,
        STATUS_STOP,
    };

public:
    Image2(
        IN const WCHAR* filename,
        GifCallback * call_back = NULL, 
        IN BOOL useEmbeddedColorManagement = FALSE
        );

    static Image2 * FromFile(
        IN const WCHAR* filename,
        GifCallback * call_back = NULL, 
        IN BOOL useEmbeddedColorManagement = FALSE 
        ) ;

    virtual ~Image2() ;

    bool IsGif() const ;

    void SetGifCallBack(GifCallback * call_back) ;
    void Play() ;
    void Pause() ;
    void Continue() ;
    void Stop() ;

    bool IsPlaying() const { return m_gif_status == STATUS_PLAY ; }
    bool IsPause() const { return m_gif_status == STATUS_PAUSE ; }
    bool IsStop() const { return m_gif_status == STATUS_STOP ; }

    // 设置当前激活帧的接口
    bool SelectActiveFrame(UINT index) ;

    // 按原格式保存文件
    // fname中不应包含扩展名
    bool ToFile(std::wstring const &dir, std::wstring const &fname) ;

    // 根据编码格式获得扩展名
    static bool GetExtByCodecFormat(GUID const & format_id, std::wstring & ext) ;

protected:
    struct Id_Ext 
    {
        GUID id ;
        wchar_t ext[10] ;
    };
    static Id_Ext s_Id2Ext_map[11] ;

    //bool Init() ;

protected:
    // 每个gif图像都有一个定时器任务
    class GifTimerTask : public TimerTask
    {
    public:
        GifTimerTask(Image2 &gif) ;

        virtual bool DoDelayedWork(UINT & next_delayed_work_time) ;

    protected:
        Image2 & m_gif ;
    };

    bool DoDelayedWork(UINT & next_delayed_work_time) ;

protected:
    GifCallback * m_gif_call_back ;
    GifTimerTask m_gif_timer_task ;

    UINT m_gif_frame_count ;
    UINT m_gif_current_frame_index ;
    std::vector<UINT> m_gif_frame_delay_times ;
    UINT m_gif_total_delay_time ;
    GIF_STATUS m_gif_status ;

    UINT m_gif_play_time ;     // 准确的开始播放动画的时间，不一定是第一帧，可能是暂停后继续的某一帧
    UINT m_gif_play_index ;    // 开始播放动画时的帧索引

    friend class GifTimerTask ;

};

class ImageDelegate : public GifCallback
{
public:
    ImageDelegate(
        IN const WCHAR* filename,
        IN BOOL useEmbeddedColorManagement = FALSE
        ) ;

    void SetWinHost(HWND host) ;
    void SetBound(int x, int y, int width, int height) ;
    void GetBound(RECT &bound) const ;
    void Draw(Graphics & gph, RECT const &update_bound) ;
    void Draw(HDC dc) ;

    void AddBound(int x, int y, int width, int height) ;
    std::vector<RECT> const & GetBound() const ;

    //////////////////////////////////////////////////////////////////////////
    // Delegate Image2 Method
    void Play() ;
    void Pause() ;
    void Continue() ;
    void Stop() ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // GifCallback Implement
    virtual bool Update() ;

private:
    Image2 m_image ;

    HWND m_host ;
    RECT m_bound ;      // 这个坐标必须是参照host的客户区坐标系

    std::vector<RECT> m_bound_group ;

};