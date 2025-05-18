/*
    Developer: Yehonatan Peleg
    File:      dispatcher.hpp
    Reviewer:  Amos Duek
    Date: 	   22.12.2024
    Status:    Approved
    Version:   1.0
*/

#include "logger.hpp"

namespace ilrd
{

Logger::Logger()
:
 m_isRunning(true),
 m_typeToString({{ ERROR, "ERROR" },{ WARNING, "WARNING" },
                 { DEBUG, "DEBUG" },{ INFO,  "INFO"}}),
 m_loggerLevel(DEBUG)
{
    m_file.open("Logger.log");
    if (!m_file.is_open())
    {
        throw std::runtime_error("Failed to open file: Logger.log");
    }

    m_thread = (std::thread(&Logger::LogLoop, this));
}

Logger::~Logger()
{
    m_isRunning = false;

    m_thread.join();
}

std::string Logger::GetMessage(const std::string& msg, const std::string& file,
                                                            int line, Type type)
{
    std::time_t result = std::time(nullptr);
    std::string messageTime(std::asctime(std::localtime(&result)));
    std::string full_msg = messageTime + "=> " + msg + ": " +
                  m_typeToString[type] + " " + file + " " + std::to_string(line)
                                                                       + "\n\n";

    return full_msg; 
} 

void Logger::Log(const std::string& msg, const std::string& file, int line,
                                                             Type type) noexcept
{
    if (type <= m_loggerLevel)
    {
        try
        {
            std::string logMessage = GetMessage(msg, file, line, type);

            if (logMessage.empty())  // Debug check
            {
                std::cerr << "Logger Error: GetMessage()" <<
                                      " returned an empty string!" << std::endl;

                return;
            }

            m_msgQueue.Push(std::move(logMessage));  // Move to avoid copies
        }
        catch(const std::bad_alloc& e)
        {
            std::cerr << "Logger Error: Memory allocation failed: " << e.what()
                                                                   << std::endl;
        } 
    }
}

void Logger::SetLevel(Type level) noexcept
{
    m_loggerLevel = level;
}

void Logger::LogLoop()
{
    std::shared_ptr<std::string> message;

    while (m_isRunning || !m_msgQueue.IsEmpty())
    {
        if (m_msgQueue.Pop(message, std::chrono::milliseconds(1000)) ==
                                                         wqueue_status::SUCCESS)
        {
            if (message)
            {
                m_file << *message << std::flush; 

                message.reset();
            }
            else
            {
                m_file << "Error: message is empty " << std::flush;
            }
        }
    }
}


}


