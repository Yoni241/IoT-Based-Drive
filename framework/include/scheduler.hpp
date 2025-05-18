#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <chrono>           // For time duration
#include <memory>           // For shared_ptr 
 #include <ctime>            // 
#include <functional>       // For std::function
#include <signal.h>         // For struct sigevent

#include "handleton.hpp"    // For making scheduler singleton
#include "wpqueue.hpp"      // For priority queue
#include "wqueue.hpp"       // For waitable queue
#include "logger.hpp"       // For Logger


namespace ilrd
{

class Scheduler
{
public:
    
    class ISchedTask
    {
    public:
        virtual ~ISchedTask() = default;
        virtual void Execute() = 0;
    };
    
    ~Scheduler(){};

    void AddTask(std::shared_ptr<ISchedTask> task, 
                     const std::chrono::milliseconds& miliSecDelta);

private:
    Scheduler(); // for Singleton
    friend class Singleton<Scheduler>;

    using TimePoint = std::chrono::time_point<
                        std::chrono::_V2::system_clock, 
                        std::chrono::_V2::system_clock::duration>;


    using Item = std::pair<std::shared_ptr<ISchedTask>, TimePoint>;

    struct Comparator
    {
        bool operator()(const Item& a, const Item& b);
    };

    WQueue <Item, WPQueue<Item, std::vector<Item>, Comparator>> m_pq;

    class Timer
    {
        public:
            Timer(void (*ActivateCallbackOnExp)(union sigval), 
                                                           Scheduler* schedPtr);
            ~Timer() noexcept;   

            void Set(TimePoint value);  
        private:
            struct itimerspec m_nextExpiration;
            timer_t m_timerid;

            static const int ERROR;
            static const size_t MICRO; 
            static const size_t MILLI; 

            struct timespec MiliToTimespec(std::chrono::milliseconds val);
    };

    std::mutex m_lock;
    Timer m_schedTimer;
    std::shared_ptr<Item> m_nextTaskExec;

    static void ActivateCallbackOnExp(union sigval thisSched);
    std::chrono::time_point<std::chrono::system_clock> GetCurrTime();
};

} // namespace ilrd

#endif //__SCHEDULER_HPP__
