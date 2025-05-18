#include <iostream>        // for std::cerr
#include "thread_pool.hpp" // for ThreadPool API

namespace ilrd
{

/*
________________________________________________________________________________

                                    TW Class
________________________________________________________________________________*/

class ThreadPool::ThreadWorker
{
public:
    explicit ThreadWorker(ThreadPool* tp);

    void StopThread();
    void JoinThread();
    void WorkerFunc(ThreadPool* tp);
    std::thread::id GetID() const;

private:
    std::thread m_worker;
    std::thread::id m_thread_id;
    mutable std::mutex m_stop_mutex;
    std::atomic<bool> m_isStopped;
};


ThreadPool::ThreadWorker::ThreadWorker(ThreadPool* tp)
    : m_worker(&ThreadWorker::WorkerFunc, this, tp),
    m_thread_id(m_worker.get_id()), m_isStopped(false)
{
    // Empty
}

void ThreadPool::ThreadWorker::StopThread()
{
    m_isStopped.store(true);
} 

void ThreadPool::ThreadWorker::JoinThread()
{
    m_worker.join();   
}

std::thread::id ThreadPool::ThreadWorker::GetID() const
{
    return m_worker.get_id();
}

/*
________________________________________________________________________________

                                    TP Class
________________________________________________________________________________*/

ThreadPool::ThreadPool(size_t num_of_threads)
:m_num_of_threads(num_of_threads == 0 ? DEFAULT_NUM_THREADS : num_of_threads)
{
    for (size_t i = 0; i < m_num_of_threads; ++i)
    {
        CreateThread();
    }
}

void ThreadPool::CreateThread()
{
    std::shared_ptr<ThreadWorker> worker (new ThreadWorker(this));

    {
        auto [it, success] = m_mapThreads.emplace(worker->GetID(), worker);
        if (!success)
        {
            throw std::runtime_error("Failed insert thread into m_mapThreads");
        }
    }
}

/*
________________________________________________________________________________

                                    TP Dtor
________________________________________________________________________________*/

class ThreadPool::StopTask: public ITPTask
{
    public:
    StopTask(ThreadPool* tp)
    : m_tp(tp)
    {
        // Empty
    }
    void operator()()
    {   
        auto it = m_tp->m_mapThreads.find(std::this_thread::get_id());

        if (it != m_tp->m_mapThreads.end())
        {
            it->second->StopThread();
        }
    }
    private:

    ThreadPool* m_tp;       
        

};

ThreadPool::~ThreadPool()
{
    PushStopTaskToWPQTasks(HIGH_ADMIN_P);

    for (auto& [id, worker] : m_mapThreads)
    {
        worker->JoinThread();
    }
}

/*
________________________________________________________________________________

                             Thread Handle Function
________________________________________________________________________________*/

void ThreadPool::ThreadWorker::WorkerFunc(ThreadPool *loop)
{
    std::shared_ptr<TaskPair> task;

    while (!m_isStopped.load())
    {
        loop->m_pqTasks.Pop(task);

        if (loop->m_threadState.load() == PAUSED)
        {
            std::unique_lock<std::mutex> lock(loop->m_TPMutex);

            loop->m_pausedCV.wait(lock);
        }
        
        (*task->second)();
    }
}

/*
________________________________________________________________________________

                                     Add
________________________________________________________________________________*/

void ThreadPool::Add(std::shared_ptr<ITPTask> task, Priority priority)
{
    if (!task)
    {
        throw std::invalid_argument("Task cannot be null");
    }

    if (m_threadState == STOPPED)
    {
        return;
    }

    TaskPair cur_task(static_cast<Priority>(priority), task);
    
    m_pqTasks.Push(cur_task); 
}

/*
________________________________________________________________________________

                                     Stop
________________________________________________________________________________*/

void ThreadPool::Stop()
{
    m_threadState.store(STOPPED);

    PushStopTaskToWPQTasks(LOW_ADMIN_P);
}

/*
________________________________________________________________________________

                                    Pause
________________________________________________________________________________*/

void ThreadPool::Pause()
{
    m_threadState.store(PAUSED);
}

/*
________________________________________________________________________________

                                   Resume
________________________________________________________________________________*/
void ThreadPool::Resume()
{
    m_threadState.store(RUNNING, std::memory_order_release);

    m_pausedCV.notify_all(); 
}

/*
________________________________________________________________________________

                              SetNumThread
________________________________________________________________________________*/

void ThreadPool::SetNumThread(size_t num_of_threads)
{
    if (num_of_threads > m_num_of_threads)
    {
        size_t threads_to_add = num_of_threads - m_num_of_threads;
        for (size_t i = 0; i < threads_to_add; ++i)
        {
            CreateThread();
        }
    }
    else if (num_of_threads < m_num_of_threads)
    {
        size_t threads_to_remove = m_num_of_threads - num_of_threads;

        for (size_t i = 0; i < threads_to_remove; ++i)
        {
            PushStopTaskToWPQTasks(LOW_ADMIN_P); // Signal a thread to stop
        }
    }

    m_num_of_threads = num_of_threads;
}

/*
________________________________________________________________________________

                              Helper Functions
________________________________________________________________________________*/

void ThreadPool::PushStopTaskToWPQTasks(Admin_Priority priority)
{
    TaskPair stop_task = std::make_pair(static_cast<Priority> (priority),
                                              std::make_shared<StopTask>(this));

    for (size_t i = 0; i < m_mapThreads.size(); i++)
    {
        {
            std::lock_guard<std::mutex> lock(m_TPMutex);
            m_pqTasks.Push(stop_task);
        }
    }
}

} // namespace ilrd
