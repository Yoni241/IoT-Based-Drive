#ifndef __ILRD_REACTOR_HPP__
#define __ILRD_REACTOR_HPP__

#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <cstring>

#include "logger.hpp"

namespace ilrd
{

class Reactor
{
public:
    enum Mode
    {
        READ, 
        WRITE
    };
    
    using ListenPair = std::pair<int, Reactor::Mode>;
    
    class Ilistener
    {
    public: 
        virtual ~Ilistener() = default;
        virtual std::vector<ListenPair> Listen(const std::vector<ListenPair>&
                                                                  listenTo) = 0;
    };
    
    explicit Reactor(std::shared_ptr<Ilistener> listen);
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;
    ~Reactor() = default;

    void Register(int fd, Mode mode, std::function<void(int, Mode)>);
    void UnRegister(int fd, Mode mode);
    void Run();
    void Stop();
    
private:
    struct HashPair
    {
        size_t operator()(const ListenPair p) const
        {
            return std::hash<int>{}(p.first * (1 + (10 * p.second))); // not concrete may change
        }
    };

    void PushFdAndModeToVector(std::vector<std::pair<int, Mode>>&);
    void ActivateCallbacks(std::vector<std::pair<int, Mode>>&);

    std::unordered_map<ListenPair, std::function<void(int, Mode)>, 
                                                               HashPair> m_umap;
    std::shared_ptr<Ilistener> m_listener;
    bool m_isStopped;
    Logger* m_logger;
    
};// class Reactor

} // namespace ilrd

#endif //__ILRD_REACTOR_HPP__
