#pragma once

#include <list>
#include "WndMsgOnly.h"

class TimerTaskPump ;

class TimerTask
{
public:
    TimerTask(TimerTaskPump * pump = NULL) ;
    virtual ~TimerTask() ;
    virtual bool DoDelayedWork(UINT & next_delayed_work_time) = 0 ;

    void SetPump(TimerTaskPump * pump) ;
    void StartTask(UINT dleayed_time) ;
    void StopTask() ;

private:
    TimerTaskPump * m_pump ;
    bool m_running ;
};

struct TASKINFO
{
    UINT m_delayed_time ;

    TimerTask * m_timer_task ;
};

class TimerTaskPump : public CWndMsgOnly 
{
    DECLARE_DYNAMIC(TimerTaskPump)

public:
    TimerTaskPump() ;

    void AddTask(UINT delayed_time, TimerTask * task) ;
    void RemoveTask(TimerTask * task) ;

    void HandleTimerMessage() ;

public:
    static bool Initialize() ;
    static bool Uninitialize() ;
    static TimerTaskPump * GetPump() ;

protected:
    DECLARE_MESSAGE_MAP()

protected:
//     virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

private:
    //CWndMsgOnly m_task_wnd ;
    //HWND m_task_wnd ;

    DWORD m_start_time ;
    std::list<TASKINFO> m_task_queue ;

    static TimerTaskPump * s_timer_task_pump ;

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
