#ifndef __WPQUEUE_HPP__
#define __WPQUEUE_HPP__

#include <queue>

template <typename T, class Container = std::vector<T>, 
                     class Compare = std::less<typename Container::value_type> >
class WPQueue
{
public:
    const T& front() const;
    void pop_front();
    void push_back(const T& val);
    bool empty() const;

private:
    std::priority_queue<T, Container, Compare> m_pqueue;
};

template <class T, class Container, class Compare>
const T& WPQueue<T, Container, Compare>::front() const
{
    return m_pqueue.top();
}

template <class T, class Container, class Compare>
void WPQueue<T, Container, Compare>::pop_front()
{
    m_pqueue.pop();
}

template <class T, class Container, class Compare>
void WPQueue<T, Container, Compare>::push_back(const T& val)
{
    m_pqueue.push(val);
}

template <class T, class Container, class Compare>
bool WPQueue<T, Container, Compare>::empty() const
{
    return m_pqueue.empty();
}

#endif /*__PWQUEUE_HPP__*/