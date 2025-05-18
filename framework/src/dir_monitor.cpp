#include <sys/inotify.h>    // for file monitoring
#include <unistd.h>         // for inotify and getpagesize
#include <fcntl.h>          // for fcntl
#include <iostream>         // for cerr
#include <filesystem>       // for std::filesystem
#include <cstring>

#include "logger.hpp"       // For documentation
#include "handleton.hpp"    // For GetLogger

#include "dir_monitor.hpp"

namespace ilrd
{

const size_t DirMonitor::BUFF_SIZE = 4096;

DirMonitor::DirMonitor(const std::string &dirPath)
: m_dirPath(dirPath),
  m_inotifyFD(InitInotify()),
  m_watchFD(InotifyAddWatch())
{
    ReadFolder();

    m_thread = std::thread(&DirMonitor::RunMonitor, this);
}

DirMonitor::~DirMonitor() noexcept
{
    inotify_rm_watch(m_inotifyFD, m_watchFD);
    close(m_inotifyFD);

    if (m_thread.joinable())
    { 
        m_thread.join();
    }
}

void DirMonitor::Register(ICallback<std::string>* callback, EventType listenTo)
{
    m_disp[listenTo].Attach(callback);
}


void DirMonitor::UnRegister(ICallback<std::string>* callback)
{
    callback->Unsubscribe();
}

std::unordered_set<std::string> DirMonitor::GetExistingFiles()
{  
    return m_filesInFolder;
} 

bool DirMonitor::IsWatcherClosed(uint32_t mask)
{
    return IN_IGNORED == mask;
}

void DirMonitor::RunMonitor()
{
    char buffer[BUFF_SIZE] = {'\0'};
    ssize_t messageLength = 0;
    uint32_t watcherMask = 1;

    while (!IsWatcherClosed(watcherMask)) 
    {
        ParseMessage(buffer, messageLength);
        messageLength = read(m_inotifyFD, buffer, BUFF_SIZE);  
        if(messageLength < 0)
        {
            Singleton<Logger>::GetInstance()->Log
                            ("Failed to read an update from monitored folder \"" 
            + m_dirPath + "\". errno = " + std::to_string(errno),
                                             __FILE__, __LINE__, Logger::ERROR);
        }

        watcherMask = reinterpret_cast<struct inotify_event*>(buffer)->mask;
    }

    Singleton<Logger>::GetInstance()->Log
                               ("Done monitoring folder \"" + m_dirPath + "\".",
                                              __FILE__, __LINE__, Logger::INFO);
}

void DirMonitor::ReadFolder()
{
    for(auto const& entry : std::filesystem::directory_iterator{m_dirPath})
    {
        m_filesInFolder.emplace(entry.path().filename());
    }
}

void DirMonitor::ParseMessage(char* buffer, ssize_t messageLength)
{
    for(char* runner = buffer; runner < buffer + messageLength;)
        {
            struct inotify_event* eventData = 
                                reinterpret_cast<struct inotify_event*>(runner);
            std::string msg = m_dirPath + eventData->name;
            switch (eventData->mask)
            {
            case IN_CREATE:
                m_filesInFolder.insert(msg);
                m_disp[DM_CREATE].Notify(&msg);
                break;
            
            case IN_DELETE:
                m_filesInFolder.erase(msg);
                m_disp[DM_DELETE].Notify(&msg);
                break;

            case IN_MODIFY:
                m_disp[DM_MODIFY].Notify(&msg);
                break;
            }

            runner += (sizeof(struct inotify_event) + eventData->len); 
        } 

}

int DirMonitor::InitInotify()
{
    int fd = inotify_init();
    if (fd < 0)
    {
        throw std::runtime_error("inotify_init failed\n");
    }

    return fd;
}

int DirMonitor::InotifyAddWatch()
{
    int fd = inotify_add_watch(m_inotifyFD, m_dirPath.c_str(), IN_CREATE | IN_DELETE);

    if (fd < 0)
    {
        std::cerr << "inotify_add_watch failed: " << strerror(errno) << std::endl;

        Singleton<Logger>::GetInstance()->Log(
            "inotify_add_watch failed \"" + m_dirPath + "\": " + strerror(errno),
            __FILE__, __LINE__, Logger::INFO);

        throw std::runtime_error("inotify_add_watch failed");
    }

    return fd;
}
}

