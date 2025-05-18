#ifndef __ILRD_RD161_DIR_MONITOR_HPP__
#define __ILRD_RD161_DIR_MONITOR_HPP__

#include <string>
#include <thread>  
#include <unordered_set>
#include <unordered_map>

#include "dispatcher.hpp"

namespace ilrd
{

class DirMonitor
{
public:
    explicit DirMonitor(const std::string& dirPath); // may throw runtime_error
    ~DirMonitor() noexcept;
    DirMonitor(const DirMonitor& other) = delete;
    DirMonitor& operator=(const DirMonitor& other) = delete;
    
    enum EventType
    {
        DM_CREATE,
        DM_DELETE,
        DM_MODIFY   // advanced
    };
    
    void Register(ICallback<std::string>* callback, EventType listenTo);
    void UnRegister(ICallback<std::string>* callback);
    
    std::unordered_set<std::string> GetExistingFiles(); // include <filesystem> in src to handle this functionality
    
private:
    static const size_t BUFF_SIZE;

    std::unordered_set<std::string> m_filesInFolder;
    std::unordered_map<EventType, Dispatcher<std::string>> m_disp;

    const std::string m_dirPath;
    int m_inotifyFD;
    int m_watchFD;
    std::thread m_thread;

    int InitInotify();
    int InotifyAddWatch();
    void RunMonitor();
    void ReadFolder();
    void ParseMessage(char*, ssize_t);
    bool IsWatcherClosed(uint32_t);
};

} // namespace ilrd

#endif // __ILRD_RD161_DIR_MONITOR_HPP__