/*
    Developer: Yehonatan Peleg
    File:      dispatcher.hpp
    Reviewer:  Amos Duek
    Date: 	   22.12.2024
    Status:    Approved
    Version:   1.0
*/

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <thread>
#include <unordered_map>    // unordered map
#include <fstream>          // std::fstream

#include "wqueue.hpp"       // For thread-safe waiting queue
#include "handleton.hpp"    // For making the class singleton

namespace ilrd
{

class Logger
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger() noexcept;
    
    enum Type
    {
        ERROR,
        WARNING,
        INFO,
        DEBUG
    };
    
    void SetLevel(Type level) noexcept;
    void Log(const std::string& msg, const std::string& file, int line, 
                                                    Type type = ERROR) noexcept;
    
private:
    Logger(); 

    friend class Singleton<Logger>;
    
    void LogLoop();

    bool m_isRunning;

    std::unordered_map<Type, std::string> m_typeToString;
    std::ofstream m_file;
    std::thread m_thread;
    std::string GetMessage(const std::string& msg, const std::string& file, 
                                                           int line, Type type);

    WQueue<std::string> m_msgQueue;

    Type m_loggerLevel;
};

} // namespace ilrd

#endif // __LOGGER_HPP__
