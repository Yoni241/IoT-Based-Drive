#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <memory>               // for std::shared_ptr
#include <thread>               // for std::thread
#include <unordered_map>        // for std::unordered_map
#include <functional>           // for std::function
#include <atomic>               // for std::atomic
#include <condition_variable>   // for std::conditional_variable
#include <mutex>
#include <future>               // for std::future

#include "wqueue.hpp"           // for waitable queue
#include "wpqueue.hpp"          // for priority queue wrapper
#include "handleton.hpp"        // for singleton

namespace ilrd
{

/*
________________________________________________________________________________

                                 ThreadPool
________________________________________________________________________________*/

class ThreadPool
{
public:
    friend class Singleton<ThreadPool>;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

/*
____________________________________________

               Task Interface
____________________________________________
*/

    class ITPTask
    {
    public:
        ITPTask() = default;
        ITPTask(const ITPTask&) = default;
        virtual ~ITPTask() = default;
        virtual void operator()() = 0;
    };

/*
____________________________________________

        FutureTask Templated Class
____________________________________________
*/

    template< class F, class... Args >
    class FutureTask : public ITPTask
    {
    public:
        FutureTask(std::function<F(Args...)> func);
        FutureTask(const FutureTask&) = default;
        virtual ~FutureTask() = default;
        virtual void operator()();
        F GetResult();
    private:
        std::function<F(Args...)> m_func;
        std::future<F> m_return_val;
    };

    enum Priority {LOW, MID, HIGH};

    void Add(std::shared_ptr<ITPTask> task, Priority prior = LOW); 
    void Stop();                                                
    void Pause(); 
    void Resume();
    void SetNumThread(size_t num_of_threads);                    
private:

    explicit ThreadPool(size_t num_of_threads = 
                                           std::thread::hardware_concurrency());
    ~ThreadPool();
    /* ----- Inner classes ----- */
    class ThreadWorker;
    class StopTask;

    /* -----    Aliasses   ----- */
    using TaskPair = std::pair<Priority, std::shared_ptr<ITPTask>>;
    using ThreadPair = std::unordered_map<std::thread::id, 
                                                 std::shared_ptr<ThreadWorker>>;

    /* ----- Enums ----- */
    enum Admin_Priority{LOW_ADMIN_P = -1, LOW_P, MID_P, HIGH_P, HIGH_ADMIN_P};
    enum State {RUNNING, STOPPED, PAUSED};

    /* ----- Private Members ----- */
    std::atomic<State> m_threadState;
    size_t m_num_of_threads;
    WQueue<TaskPair, WPQueue<TaskPair>> m_pqTasks;
    ThreadPair m_mapThreads;
    std::mutex m_TPMutex;
    std::condition_variable m_pausedCV;

    /* -----  Private Methods ----- */
    void CreateThread(); 
    void PushStopTaskToWPQTasks(Admin_Priority p);

    /* ----- const static members  ----- */
    static const size_t DEFAULT_NUM_THREADS = 8;
};

/*
________________________________________________________________________________

                    futureTask template implementation
________________________________________________________________________________*/

template <class F, class... Args>
ThreadPool::FutureTask<F, Args...>::FutureTask(std::function<F(Args...)> func)
    : m_func(func)
{
    // Empty
}

template <class F, class... Args>
void ThreadPool::FutureTask<F, Args...>::operator()()
{
    m_return_val = std::async(std::launch::async, m_func, Args()...);
}

template <class F, class... Args>
F ThreadPool::FutureTask<F, Args...>::GetResult()
{
    return m_return_val.get();
}


} // namespace ilrd

#endif // __THREAD_POOL_HPP__
