#include "reactor.hpp"
#include "logger.hpp"

namespace ilrd
{

Reactor::Reactor(std::shared_ptr<Ilistener> listener)
: m_listener(listener), m_isStopped(false)
{
    m_logger = Singleton<Logger>::GetInstance();
    m_logger->Log("Reactor initialized", __FILE__, __LINE__, Logger::DEBUG);
}

void Reactor::Register(int fd, Mode mode, std::function<void(int, Mode)> 
                                                                       callback)
{
    m_logger->Log("Registering FD: " + std::to_string(fd) + 
                   ", Mode: " + std::to_string(mode),
                   __FILE__, __LINE__, Logger::INFO);

    if (fd < 0)
    {
        m_logger->Log("ERROR: Invalid FD: " + std::to_string(fd), 
                       __FILE__, __LINE__, Logger::ERROR);
        return;
    }

    m_umap[std::pair<int, Mode>(fd, mode)] = callback;
    
    m_logger->Log("Successfully registered FD: " + std::to_string(fd), 
                   __FILE__, __LINE__, Logger::INFO);
}

void Reactor::UnRegister(int fd, Mode mode)
{
    auto it = m_umap.find(std::pair<int, Mode>(fd, mode));
    if (it != m_umap.end())
    {
        m_logger->Log("Unregistering FD: " + std::to_string(fd), __FILE__,
                                                        __LINE__, Logger::INFO);
                                                        
        m_umap.erase(it);
    }
    else
    {
        m_logger->Log("Warning: Attempted to unregister non-existent FD: " + 
                       std::to_string(fd), __FILE__, __LINE__, Logger::WARNING);
    }
}

void Reactor::PushFdAndModeToVector(std::vector<std::pair<int, Mode>>& listenTo)
{
    m_logger->Log("Pushing FDs to listen vector", __FILE__, __LINE__,
                                                                 Logger::DEBUG);

    for (auto const& pair : m_umap)
    {
        listenTo.push_back(pair.first);
        m_logger->Log("Added FD: " + std::to_string(pair.first.first) + 
                      ", Mode: " + std::to_string(pair.first.second),
                      __FILE__, __LINE__, Logger::DEBUG);
    }
}

void Reactor::ActivateCallbacks(std::vector<std::pair<int, Mode>>& listenTo)
{
    m_logger->Log("Activating Callbacks", __FILE__, __LINE__, Logger::DEBUG);

    for (auto pair : listenTo)
    {
        try
        {
            m_logger->Log("Checking FD: " + std::to_string(pair.first) +
                          ", Mode: " + std::to_string(pair.second),
                          __FILE__, __LINE__, Logger::DEBUG);

            auto it = m_umap.find(pair);

            if (it != m_umap.end())
            {
                m_logger->Log("Executing callback for FD: " +
                 std::to_string(pair.first), __FILE__, __LINE__, Logger::DEBUG);

                it->second(pair.first, pair.second);
            }
            else
            {
                m_logger->Log("ERROR: Key not found for FD: " +
                 std::to_string(pair.first), __FILE__, __LINE__, Logger::ERROR);
            }
        }
        catch (const std::out_of_range& e)
        {
            m_logger->Log("EXCEPTION: unordered_map::at() out of range: " + 
                      std::string(e.what()), __FILE__, __LINE__, Logger::ERROR);
        }
    }
}

void Reactor::Run()
{
    std::vector<std::pair<int, Mode>> listenTo;

    m_logger->Log("Reactor Running", __FILE__, __LINE__, Logger::INFO);

    while (!m_isStopped)
    {
        PushFdAndModeToVector(listenTo);

        listenTo = m_listener->Listen(listenTo);

        ActivateCallbacks(listenTo);

        listenTo.clear();
    }

    m_logger->Log("Reactor stopped", __FILE__, __LINE__, Logger::INFO);
}           

void Reactor::Stop()
{
    m_logger->Log("Stopping Reactor", __FILE__, __LINE__, Logger::INFO);
    m_isStopped = true;
}

}
