// Developer: Yehonatan Peleg
// File: 	  handleton.hpp
// Reviewer:  
// Date: 	  16 dec 2024
// Version:   1.0
// Status:    1.0: Approved

#ifndef __HANDLETON_HPP__
#define __HANDLETON_HPP__

#include <mutex>      // for std::mutex 
#include <cstdlib>    // for std::atexit
#include <functional> // for std::function
#include <atomic>     // for std::atomic
#include <string>

namespace ilrd
{

/*
________________________________________________________________________________

                          get instances declelration 
________________________________________________________________________________
                                                                                */

template <class T, class ...Args>
class Singleton
{
public:
    Singleton() = delete;
    Singleton(const Singleton& other) = delete;
    Singleton(Singleton&& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;
    ~Singleton() = delete;

    static T* GetInstance(Args... args);
private:

    static std::function<T*(Args...)> m_currGetInstance;
    static std::mutex m_mutex;
    static std::atomic<T*> m_instance;

    static T* GetLocking(Args... args);
    static T* GetNonLocking(Args... args);
    static void ReleaseInstance();
};

#ifdef __HANDLETON__

template <class T, class ...Args>
std::mutex Singleton<T, Args...>::m_mutex;

template <class T, class ...Args>
std::atomic<T*> Singleton<T, Args...>::m_instance(NULL); 

template <class T, class ...Args>
std::function<T*(Args...)> 
    Singleton<T, Args...>::m_currGetInstance(Singleton<T, Args...>::GetLocking);

template <class T, class ...Args>
T* Singleton<T, Args...>::GetInstance(Args... args)
{
    return m_currGetInstance(args...);
}

template <class T, class ...Args>
T* Singleton<T, Args...>::GetLocking(Args... args)
{
    T* temp =  m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    
    if (NULL == m_instance)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        temp = m_instance.load(std::memory_order_relaxed);

        if (NULL == m_instance)
        {
            temp = new T(args...);

            std::atexit(ReleaseInstance);
            
            std::atomic_thread_fence(std::memory_order_release);

            m_instance.store(temp, std::memory_order_relaxed);

            m_currGetInstance = GetNonLocking;
        }
    }

    return temp;
}

template <class T, class ...Args>
T* Singleton<T, Args...>::GetNonLocking(Args... args)
{
    auto var = std::make_tuple(args...);
    static_cast<void>(var);

    return m_instance.load();
}

template <class T, class ...Args>
void Singleton<T, Args...>::ReleaseInstance()
{
    delete m_instance;
}

#endif // HANDLETON

} // namespace ilrd

#endif //__HANDLETON_HPP__