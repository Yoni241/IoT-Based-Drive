/* 
    Developer: Yehonatan Peleg
    File: 	  shared_ptr.hpp
    Reviewer:  Alon Kalif
    Date: 	  27 nov 2024
    Version:   1.0 
    Status: Approved
*/

#ifndef __SHARED_PTR_HPP__
#define __SHARED_PTR_HPP__

#include <cstddef> // for size_t
#include <cassert> // assert

namespace ilrd
{

template <typename T>
class SharedPtr
{
public:
    explicit inline SharedPtr(T* ptr = NULL); // May throw bad_alloc
    inline SharedPtr(const SharedPtr& other);

    template <typename S> inline SharedPtr(const SharedPtr<S>& sp);
    
    inline SharedPtr& operator=(const SharedPtr& other);
    inline ~SharedPtr();    
    
    inline T& operator*() const;
    inline T* operator->() const;
    inline T* GetPtr() const;
    
private:
    template <typename S> 
    friend class SharedPtr;
    T* m_ptr;
    size_t* m_counter;
    inline bool IsUnique() const;
    inline void Detach();
    inline void Attach();
    inline bool IsSameObject(const SharedPtr& other);
};
/*______________________________________________________________________________

                                Ctor/Dtor/CCtor 
________________________________________________________________________________*/

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr)
: m_ptr(ptr), m_counter(new size_t(1))
{
    assert(ptr);
}

template <typename T>
SharedPtr<T>::~SharedPtr() 
{
    Detach();
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
:m_ptr(other.m_ptr), m_counter(other.m_counter)
{
    Attach();
}
/*______________________________________________________________________________

                              Implicit Conversion
________________________________________________________________________________*/
template <typename T>
template <typename S>
inline SharedPtr<T>::SharedPtr(const SharedPtr<S>& sp)
:   m_ptr(sp.m_ptr), m_counter(sp.m_counter)
{
    Attach();
}

/*______________________________________________________________________________

                                    Getter 
________________________________________________________________________________*/

template <typename T>
inline T* SharedPtr<T>::GetPtr() const
{
    return m_ptr;
}

/*______________________________________________________________________________

                                Operator Methods
________________________________________________________________________________*/

template <typename T>
inline T* SharedPtr<T>::operator->() const
{
    return m_ptr;
}

template <typename T>
inline T& SharedPtr<T>::operator*() const
{
    return *m_ptr;
}

template <typename T>
inline SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &other)
{
    if (IsSameObject(other) == false)
    {
        Detach();

        m_ptr = other.m_ptr;
        m_counter = other.m_counter;

        Attach();
    }

    return *this;
}

/*______________________________________________________________________________

                                Private Methods
________________________________________________________________________________*/
template <typename T>
inline bool SharedPtr<T>::IsUnique() const
{
    return *m_counter == 1;
}

template <typename T>
inline void SharedPtr<T>::Detach()
{
    if (IsUnique())
    {
        delete m_ptr;
        delete m_counter;
    }
    else
    {
        --(*m_counter);
    }
}

template <typename T>
inline bool SharedPtr<T>::IsSameObject(const SharedPtr& other)
{
    return (GetPtr() == other.GetPtr());
}

template <typename T>
inline void SharedPtr<T>::Attach()
{
    ++(*m_counter);
}

} // namespace ilrd

#endif //__SHARED_PTR_HPP__