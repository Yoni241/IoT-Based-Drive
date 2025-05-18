/*
    Developer: Yehonatan Peleg
    File:      dispatcher.hpp
    Reviewer:  Alon Kalif
    Date: 	   22.12.2024
    Status:    CR in progress
    Version:   1.0
*/

#ifndef __ILRD_RD161_DISPATCHER_HPP__
#define __ILRD_RD161_DISPATCHER_HPP__

#include <unordered_set>
#include <iterator>       // For std::next

#include "logger.hpp"

namespace ilrd
{

/*
________________________________________________________________________________

                              Dispatcher Class
________________________________________________________________________________*/
template <class T>
class ICallback;

template <class T>
class Dispatcher
{
public:
    Dispatcher() = default;
    Dispatcher(const Dispatcher&) = delete; 
    Dispatcher& operator=(const Dispatcher&) = delete; 
    ~Dispatcher() noexcept;
    void Attach(const ICallback<T>* callback);
    void Notify(const T* message) const;
    
private:
    friend class ICallback<T>;
    std::unordered_set<ICallback<T>*> m_callbacks;
    void Detach(const ICallback<T>* callback);
};


/*
________________________________________________________________________________

                            ICallback Class
________________________________________________________________________________*/


template <class T>
class ICallback
{
public:
    ICallback() = default; 
    void Unsubscribe();
    virtual void Update(const T* message) = 0;
    virtual ~ICallback();

private:
    friend class Dispatcher<T>;
    void Subscribe(Dispatcher<T>* disp);
    void NotifyDeath();
    virtual void OnNotifyDeath();
    Dispatcher<T>* m_dispatcher;
};


/*
________________________________________________________________________________

                           Dispatcher Implementation     
________________________________________________________________________________*/

template <class T>
Dispatcher<T>::~Dispatcher() noexcept
{
    for (auto callback : m_callbacks)
    {
        callback->NotifyDeath();
    }
}

template <class T>
void Dispatcher<T>::Attach(const ICallback<T>* callback)
{
    /*
        Explaination: cb->Subscribe(this)

        The callback attached sets its dispatcher member 
        to be familiar with its owner dispather.
    */

    ICallback<T>* cb = const_cast<ICallback<T>*>(callback);
    cb->Subscribe(this);
    m_callbacks.emplace(cb);
}

template <class T>
void Dispatcher<T>::Notify(const T* message) const
{
    /*
        Explaination: temp

        while loop m_callbacks set the another thread can 
        Unsubscribe a callbac which cause an invalid iter.
    */

    std::unordered_set<ilrd::ICallback<T>*> temp = m_callbacks; 
    for (auto iter : temp)
    {
        iter->Update(message);
    }
}


/*
________________________________________________________________________________

                           IcallBack Implementation     
________________________________________________________________________________*/


template <class T>
ICallback<T>::~ICallback()
{
    Unsubscribe();
}

template <class T>
void ICallback<T>::Unsubscribe()
{
    /*
        Explaination: if(NULL != m_dispatcher)

        check if m_dispatcher is NULL to avoid NULL->Detach.
        There is two scenarios calling Unsubscribe():
        1. User call Unsubscribe().
        2. In the end of the program (Dtor).

        if user call Unsubscribe() this function will be called
        twice. At second time we have a problam of NULL->Detach
    */

    if (NULL != m_dispatcher)
    {
        m_dispatcher->Detach(this);
        m_dispatcher = NULL;
    }
}

template <class T>
void Dispatcher<T>::Detach(const ICallback<T>* callback)
{
    m_callbacks.erase(const_cast<ICallback<T>*>(callback));
}


template <class T>
void ICallback<T>:: Subscribe(Dispatcher<T>* disp)
{
    m_dispatcher = disp;
} 

template <class T>
void ICallback<T>::NotifyDeath()
{
    OnNotifyDeath();
    m_dispatcher = NULL;
}

template <class T>
void ICallback<T>::OnNotifyDeath()
{
    // user
}


}
#endif //__ILRD_RD161_DISPATCHER_HPP__


