#include "async_injection.hpp"

namespace ilrd
{

AsyncInjection::AsyncInjection(std::function<bool()> func, 
                                    std::chrono::milliseconds timeToReInjection)
    : m_func(func), 
      m_timeToReInjection(timeToReInjection),
      m_aiTask(std::make_shared<AsyncInjectionTask>(*this))
{
    Singleton<Logger>::GetInstance()->
                 Log("AsyncInjection: Ctor", __FILE__, __LINE__, Logger::DEBUG);

    Singleton<Scheduler>::GetInstance()->AddTask(m_aiTask, m_timeToReInjection);
}

void AsyncInjection::Perform()
{
    Singleton<Logger>::GetInstance()->
                               Log("AsyncInjection: Perform", __FILE__, __LINE__
                                                               , Logger::DEBUG);

    if (m_func() == true)
    {
        Singleton<Logger>::GetInstance()->
                             Log("AsyncInjection: Task completed, self-deleting"
                                           , __FILE__, __LINE__, Logger::DEBUG);
        delete this;
    }
    else
    {
        Singleton<Logger>::GetInstance()->
                                         Log("AsyncInjection: Rescheduling task"
                                           , __FILE__, __LINE__, Logger::DEBUG);
        Singleton<Scheduler>::GetInstance()->
                                         AddTask(m_aiTask, m_timeToReInjection);
    }
}

AsyncInjection::AsyncInjectionTask::AsyncInjectionTask(AsyncInjection& ai)
: m_ai(ai)
{
    Singleton<Logger>::GetInstance()->Log("AsyncInjectionTask: Ctor"
                                           , __FILE__, __LINE__, Logger::DEBUG);
}

void AsyncInjection::AsyncInjectionTask::Execute()
{
    Singleton<Logger>::GetInstance()->Log("AsyncInjectionTask: Execute"
                                           , __FILE__, __LINE__, Logger::DEBUG);

    m_ai.Perform();
}

} // namespace ilrd
