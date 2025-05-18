// Developer: Yehonatan Peleg
// File: 	  factory.hpp
// Reviewer:  shimi
// Date: 	  16 dec 2024
// Version:   1.0
// Status:    1.0: Approved

#ifndef __FACTORY_HPP__
#define __FACTORY_HPP__

#include <memory>           // std::shared_ptr
#include <functional>       // for std::function
#include <unordered_map>    // for std::unordered_map
#include <mutex>            // for std::mutex
#include <stdexcept>        // for std::runtime_error

#include "handleton.hpp" // for singleton api

namespace ilrd
{

template <class K, class F, class ...Args>
class Factory
{
public:
    friend class Singleton<Factory<K, F, Args...>>;
    
    Factory(const Factory& other) = delete; // No copy constructor
    Factory(Factory&& other) = delete;      // No move constructor
    Factory& operator=(const Factory& other) = delete; // No copy assignment

    using FPtr = std::shared_ptr<F>; 

    void Add(K key, std::function<FPtr(Args...)> creator);

    FPtr Create(const K& key, Args... args);

private:
    explicit Factory() = default; // Constructor
    ~Factory() = default;

    std::unordered_map<K, std::function<FPtr(Args...)>> m_factory_map;   
};

// Add Method Implementation
template <class K, class F, class ...Args>
void Factory<K, F, Args...>::Add(K key, std::function<FPtr(Args...)> creator)
{
    m_factory_map[key] = creator;
}

// Create Method Implementation
template <class K, class F, class ...Args>
typename Factory<K, F, Args...>::FPtr Factory<K, F, Args...>::
                                              Create(const K& key, Args... args)
{
    // forward is for preserving value category
    return m_factory_map.at(key)(std::forward<Args>(args)...); 
}

} // namespace ilrd

#endif // __FACTORY_HPP__

