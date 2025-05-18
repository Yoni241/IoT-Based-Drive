// Developer: Yehonatan Peleg
// File:       wqueue.hpp
// Reviewer:   Shay Kressel
// Date:       4 dec 2024
// Version:    1.0
// Status:     1.0: Approved

#ifndef __WQUEUE_HPP__
#define __WQUEUE_HPP__ 

#include <chrono>             // for chrono::time_point
#include <queue>              // for deque
#include <mutex>              // for timed_mutex, unique_lock
#include <condition_variable> // for condition_variable_any
#include <iostream>           // for std::invalid_argument

namespace ilrd
{

enum wqueue_status
{
    SUCCESS,
    TIME_OUT_EMPTY,
    TIME_OUT_LOCK  
};

template<typename T, class Container = std::deque<T>>
class WQueue
{
public: 
    WQueue()                         = default; 
    WQueue(const WQueue&)            = delete;
    WQueue& operator=(const WQueue&) = delete;
    ~WQueue()                        = default;

    void          Pop(std::shared_ptr<T>&);                                       
    wqueue_status Pop(std::shared_ptr<T>& outP,
                                   const std::chrono::milliseconds& timeToWait);  // may throw invalid arg
    void          Push(const T& val);                                             // may throw bad_alloc
    bool          IsEmpty() const;
    
private:
    Container m_container;
    mutable std::timed_mutex m_mutex;
    std::condition_variable_any m_cv;
}; 

template<typename T, class Container>
void ilrd::WQueue<T, Container>::Pop(std::shared_ptr<T>& outP)
{
    std::unique_lock<std::timed_mutex> lock(m_mutex);

    m_cv.wait(lock, [this]() { return !m_container.empty(); });

    outP = std::make_shared<T>(m_container.front()); // Create shared_ptr from the front element

    m_container.pop_front();
}

template<typename T, class Container>
void ilrd::WQueue<T, Container>::Push(const T& val)
{
    std::unique_lock<std::timed_mutex> lock(m_mutex);

    m_container.push_back(val);

    m_cv.notify_one();
}

template<typename T, class Container>
wqueue_status WQueue<T, Container>::Pop(std::shared_ptr<T>& outP, 
                                    const std::chrono::milliseconds& timeToWait)
{
    std::chrono::time_point<std::chrono::steady_clock> timeout = 
                                  std::chrono::steady_clock::now() + timeToWait;

    std::unique_lock<std::timed_mutex> uniqueLock(m_mutex, std::defer_lock);

    if (!uniqueLock.try_lock_until(timeout))
    {
        return TIME_OUT_LOCK; 
    }

    if (!m_cv.wait_until(uniqueLock, timeout, 
                         [this]() { return !m_container.empty(); }))
    {
        return TIME_OUT_EMPTY; 
    }

    outP = std::make_shared<T>(m_container.front()); // Create shared_ptr from the front element
    m_container.pop_front();

    return SUCCESS; 
}

template<typename T, class Container>
bool WQueue<T, Container>::IsEmpty() const
{
    std::unique_lock<std::timed_mutex> lock(m_mutex);

    return m_container.empty();
}

} // namespace ilrd

#endif //__WQUEUE_HPP__
