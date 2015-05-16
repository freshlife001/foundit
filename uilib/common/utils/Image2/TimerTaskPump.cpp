#include "stdafx.h"
#include "TimerTaskPump.h"

//////////////////////////////////////////////////////////////////////////
// TimerTask

TimerTask::TimerTask(
                     TimerTaskPump * pump //= NULL
                     )
                     : m_pump(pump) 
                     , m_running(false)
{

}

TimerTask::~TimerTask()
{
    if (m_running)
    {
        StopTask() ;
    }
}

void TimerTask::SetPump(TimerTaskPump * pump) 
{
    m_pump = pump ;
}

void TimerTask::StartTask(UINT dleayed_time)
{
    if (m_pump && !m_running)
    {
        m_running = true ;
        m_pump->AddTask(dleayed_time, this) ;
    }
}
void TimerTask::StopTask() 
{ 
    if (m_pump && m_running)
    {
        m_running = false ;
        m_pump->RemoveTask(this) ;
    }
}

//////////////////////////////////////////////////////////////////////////
// TimerTaskPump

TimerTaskPump * TimerTaskPump::s_timer_task_pump = NULL ;

IMPLEMENT_DYNAMIC(TimerTaskPump, CWndMsgOnly)

TimerTaskPump::TimerTaskPump()
: m_start_time(0)
{

}

void TimerTaskPump::AddTask(UINT delayed_time, TimerTask * task) 
{
    if (m_task_queue.empty())
    {
        TASKINFO ti = {delayed_time, task} ;
        m_task_queue.push_front(ti) ;

        m_start_time = GetTickCount() ;
        SetTimer((UINT_PTR)this, delayed_time, NULL) ;
    }
    else
    {
        DWORD current_time = GetTickCount() ;
        DWORD elapse_time = current_time - m_start_time ;

        TASKINFO &top = m_task_queue.front() ;
        if (top.m_delayed_time <= elapse_time)
            top.m_delayed_time = 0 ;
        else
            top.m_delayed_time -= elapse_time ;

        if (delayed_time < top.m_delayed_time)
        {
            // 终止当前定时器
            //::KillTimer(m_task_wnd, (UINT_PTR)this) ;
            KillTimer((UINT_PTR)this) ;

            top.m_delayed_time -= delayed_time ;
            TASKINFO new_first_task = {delayed_time, task} ;
            m_task_queue.push_front(new_first_task) ;
            m_start_time = current_time ;

            // 恢复定时器
            //::SetTimer(m_task_wnd, (UINT_PTR)this, new_first_task.m_delayed_time, NULL) ;
            SetTimer((UINT_PTR)this, new_first_task.m_delayed_time, NULL) ;
        }
        else
        {
            std::list<TASKINFO>::iterator iter = m_task_queue.begin() ;
            for (++ iter ; iter != m_task_queue.end() ; ++ iter)
            {
                TASKINFO &temp = *iter ;
                if (delayed_time < temp.m_delayed_time)
                    break ;
                else
                    delayed_time -= temp.m_delayed_time ;
            }

            if (iter != m_task_queue.end())
                iter->m_delayed_time -= delayed_time ;
            TASKINFO new_task = {delayed_time, task} ;
            m_task_queue.insert(iter, new_task) ;
        }
    }
}

void TimerTaskPump::RemoveTask(TimerTask * task) 
{
    if (m_task_queue.empty())
        return ;

    TASKINFO &first_task = m_task_queue.front() ;
    if (first_task.m_timer_task == task)
    {
        KillTimer((UINT_PTR)this) ;
        UINT first_task_delayed_time = first_task.m_delayed_time ;
        m_task_queue.pop_front() ;

        if (m_task_queue.empty())
        {
            m_start_time = 0 ;
        }
        else
        {
            DWORD current_time = GetTickCount() ;
            DWORD elapse_time = current_time - m_start_time ;

            TASKINFO &new_first_task = m_task_queue.front() ;
            if (first_task_delayed_time > elapse_time)
            {
                new_first_task.m_delayed_time += (first_task.m_delayed_time - elapse_time) ;
            }
            else
            {
                elapse_time -= first_task_delayed_time ;
                if (new_first_task.m_delayed_time > elapse_time)
                {
                    new_first_task.m_delayed_time -= elapse_time ;
                }
                else
                    new_first_task.m_delayed_time = 0 ;
            }
            // bug，已修正
            //new_first_task.m_delayed_time += (first_task.m_delayed_time - elapse_time) ;

            SetTimer((UINT_PTR)this, new_first_task.m_delayed_time, NULL) ;
        }
    }
    else
    {
        std::list<TASKINFO>::iterator iter = m_task_queue.begin() ;
        for (++ iter ; iter != m_task_queue.end() ; ++ iter)
        {
            if (iter->m_timer_task == task)
            {
                DWORD task_time = iter->m_delayed_time ;
                iter = m_task_queue.erase(iter) ;
                if (iter != m_task_queue.end())
                    iter->m_delayed_time += task_time ;

                break ;
            }
        }
    }
}

void TimerTaskPump::HandleTimerMessage() 
{
    //::KillTimer(m_task_wnd, (UINT_PTR)this) ;
    KillTimer((UINT_PTR)this) ;

    if (m_task_queue.empty())
        return ;

    // 执行已经过时的那些任务
    DWORD elapse_time = GetTickCount() - m_start_time + 8 ;     // 额外增加windows时钟精度一半时间

    std::list<TASKINFO> new_task_queue ;
    std::list<TASKINFO>::iterator iter ;
    for (iter = m_task_queue.begin() ; iter != m_task_queue.end() ; )
    {
        TASKINFO &task = *iter ;
        if (elapse_time >= task.m_delayed_time)
        {
            TASKINFO new_task = task ;
            iter = m_task_queue.erase(iter) ;

            elapse_time -= new_task.m_delayed_time ;
            if (new_task.m_timer_task->DoDelayedWork(new_task.m_delayed_time))
            {
                new_task_queue.push_back(new_task) ;
            }
        }
        else
        {
            task.m_delayed_time -= elapse_time ;
            break ;
        }
    }

    m_start_time = GetTickCount() ;
    for (iter = new_task_queue.begin() ; iter != new_task_queue.end() ; ++ iter)
    {
        AddTask(iter->m_delayed_time, iter->m_timer_task) ;
    }

    // 开始等待
    if (!m_task_queue.empty())
    {
        TASKINFO &new_top_task = m_task_queue.front() ;
        //::SetTimer(m_task_wnd, (UINT_PTR)this, new_top_task.m_delayed_time, NULL) ;
        //SetTimer((UINT_PTR)this, new_top_task.m_delayed_time, NULL) ;
        SetTimer((UINT_PTR)this, 50, NULL) ;
    }
}

// static 
bool TimerTaskPump::Initialize() 
{
    if (s_timer_task_pump == NULL)
    {
        s_timer_task_pump = new TimerTaskPump ;
        s_timer_task_pump->Create() ;
    }

    return true ;
}

// static 
bool TimerTaskPump::Uninitialize() 
{
    if (s_timer_task_pump != NULL)
    {
        s_timer_task_pump->DestroyWindow() ;
        delete s_timer_task_pump ;
        s_timer_task_pump = NULL ;
    }

    return true ;
}

// static 
TimerTaskPump * TimerTaskPump::GetPump() 
{
    return s_timer_task_pump ;
}

BEGIN_MESSAGE_MAP(TimerTaskPump, CWndMsgOnly)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// LRESULT TimerTaskPump::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
// {
//     return __super::WindowProc(message, wParam, lParam) ;
// }

void TimerTaskPump::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == (UINT_PTR)this)
    {
        HandleTimerMessage() ;
    }
    CWndMsgOnly::OnTimer(nIDEvent);
}
