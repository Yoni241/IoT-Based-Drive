/*
    Developer: Yehonatan Peleg
    File:      scheduler.cpp
    Reviewer:  Irina
    Date: 	   28.12.2024
    Status:    Ready for CR
    Version:   1.0
*/

#include <cassert>

#include "scheduler.hpp"
#include "logger.hpp"

namespace ilrd
{

/*______________________________________________________________________________

                            Scheduler Implementation
________________________________________________________________________________
                                                                                */

Scheduler::Scheduler() 
: m_schedTimer(Timer(ActivateCallbackOnExp, this))
{
    m_nextTaskExec = nullptr;
}

void Scheduler::AddTask(std::shared_ptr<ISchedTask> task,
                        const std::chrono::milliseconds& milliSecDelta)
{
    Singleton<Logger>::GetInstance()->
                    Log("Scheduler::AddTask", __FILE__, __LINE__, Logger::INFO);

    std::lock_guard<std::mutex> lock(m_lock); 

    auto absTime = GetCurrTime() + milliSecDelta; 
    if (m_nextTaskExec != nullptr && (absTime >= m_nextTaskExec->second))
    {
        m_pq.Push({task, absTime});

        return;
    }

    if (m_nextTaskExec != nullptr)
    {
        m_pq.Push(*m_nextTaskExec); 
    }


    m_nextTaskExec = std::make_shared<Item>(task, absTime);

    m_schedTimer.Set(m_nextTaskExec->second);
}



void Scheduler::ActivateCallbackOnExp(union sigval thisSched)
{
    Singleton<Logger>::GetInstance()->
                                  Log("Scheduler::OnTimer", __FILE__, __LINE__);

    Scheduler* schedPtr = reinterpret_cast<Scheduler*>(thisSched.sival_ptr);

    schedPtr->m_nextTaskExec->first->Execute();

    std::lock_guard<std::mutex> lock(schedPtr->m_lock);

    if (schedPtr->m_pq.IsEmpty())
    {
        schedPtr->m_nextTaskExec = NULL;
        return;
    }

    schedPtr->m_pq.Pop(schedPtr->m_nextTaskExec);
    

    schedPtr->m_schedTimer.Set(schedPtr->m_nextTaskExec->second);
}


std::chrono::time_point<std::chrono::system_clock> Scheduler::GetCurrTime()
{
    return std::chrono::system_clock::now();
}

bool Scheduler::Comparator::operator()(const Item& a, const Item& b)
{
    return a.second > b.second;
}

/*______________________________________________________________________________

                            Timer Wrapper Implementation
________________________________________________________________________________
                                                                                */

const int Scheduler::Timer::ERROR = -1;

const size_t Scheduler::Timer::MICRO = 1000000; // 1sc = 1,000,000 macroseconds
const size_t Scheduler::Timer::MILLI = 1000;    // 1sc = 1,000 milliseconds 

// Constructor

Scheduler::Timer::Timer(void (*activateCallbackOnExp)(union sigval),
                                                            Scheduler* schedPtr)
{
    /*  sigevent purpuse:
            sigevent struct purpuse is to configure
            the behiavor of the Timer on expiration.
    */  
    struct sigevent config = {};


    /*  sigev_notify: 
            choose how the timer expiration is notified.
            SIGEV_THREAD => flag to notify by thread.

            This flag is necessary because it defines the mechanism for 
            delivering the timer notification, ensuring the callback is 
            executed asynchronously.            
    */
    config.sigev_notify = SIGEV_THREAD; 
    config.sigev_notify_function = activateCallbackOnExp;

    /*  sigev_value:
            union field in sigevent struct. one of the union field
            is sival_ptr. this allowes (when callback is activate
            on expiration) user defined data (in our case => scheduler)
            to be accessed.
    */
    config.sigev_value.sival_ptr = schedPtr;
    config.sigev_signo = SIGALRM;

    if(ERROR == timer_create(CLOCK_REALTIME, &config, &m_timerid))
    {
        throw std::runtime_error("failed to create system timer");
    }
}

void Scheduler::Timer::Set(std::chrono::time_point<std::chrono::system_clock,
                                              std::chrono::nanoseconds> absTime)
{
    auto now = std::chrono::system_clock::now();

    if (absTime <= now)
    {
        absTime = now + std::chrono::milliseconds(1); 
    }

    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(absTime);
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(absTime) -
              std::chrono::time_point_cast<std::chrono::nanoseconds>(secs);

    struct itimerspec timerSpec = 
                        {{0, 0}, {secs.time_since_epoch().count(), ns.count()}};

    if (-1 == timer_settime(m_timerid, TIMER_ABSTIME, &timerSpec, NULL))
    {
        throw std::runtime_error("scheduler: failed to set timer\n");
    }
}

Scheduler::Timer::~Timer()
{
    if(ERROR == timer_delete(m_timerid))
    {
       Singleton<Logger>::GetInstance()->
                           Log("timer destruction failed.", __FILE__, __LINE__);
    }
}

struct timespec Scheduler::Timer::MiliToTimespec(std::chrono::milliseconds val)
{
    struct timespec ts;
    ts.tv_sec = val.count() / MILLI;            // Seconds
    ts.tv_nsec = (val.count() % MILLI) * MICRO; // Nanoseconds

    return ts;
}

} // namespace ilrd