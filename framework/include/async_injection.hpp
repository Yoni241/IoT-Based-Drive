#ifndef __ASYNC_INJECTION_HPP__
#define __ASYNC_INJECTION_HPP__

#include <functional>

#include "scheduler.hpp"

namespace ilrd
{

class AsyncInjection 
{
public:
    AsyncInjection(std::function<bool()> func,
                                   std::chrono::milliseconds timeToReInjection);

    void Perform();

private:
    class AsyncInjectionTask;
    ~AsyncInjection()= default;
    
    std::function<bool()> m_func;
    std::chrono::milliseconds m_timeToReInjection;
    std::shared_ptr<AsyncInjectionTask> m_aiTask;

    class AsyncInjectionTask : public Scheduler::ISchedTask
    {
    public:
        AsyncInjectionTask(AsyncInjection& ai);
        ~AsyncInjectionTask() = default;
        virtual void Execute();

    private:
        AsyncInjection& m_ai;
    };

};


} //namespace ilrd

#endif // __ASYNC_INJECTION_HPP__

