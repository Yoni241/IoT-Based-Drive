#include <iostream>
#include "response_manager.hpp"


namespace ilrd
{

UID ResponseManager::RegisterCommand(uint64_t numOfResponses)
{
    std::scoped_lock<std::mutex> lock(m_mapLock);
    UID uid = UID(); // Explicitly store the UID
    m_cmdRegistry[uid] = numOfResponses;
    
    return uid;
}
    

bool ResponseManager::HandleResponse(const UID& uid)
{
    std::scoped_lock<std::mutex> lock(m_mapLock);

    if (m_cmdRegistry.contains(uid))
    {
        if ((0 == (--m_cmdRegistry.at(uid))))
        {
            m_cmdRegistry.erase(uid); 
           /*  m_retryCounts.erase(uid); */

            return true;
        }
    }
    else
    {
        std::cout << "UID NOT FOUND!" << std::endl;  
    }

    return false;
}

bool ResponseManager::IsCmdFinished(const UID& uid)
{
    std::scoped_lock<std::mutex> lock(m_mapLock);

    return !(m_cmdRegistry.contains(uid));
}

// Record response time for later averaging
void ResponseManager::RecordResponseTime(std::chrono::milliseconds responseTime)
{
    std::scoped_lock<std::mutex> lock(m_responseLock);
    if (m_responseTimes.size() >= 100) // Keep last 100 responses
    {
        m_responseTimes.erase(m_responseTimes.begin());
    }
    m_responseTimes.push_back(responseTime);
}

// Compute the average response time
std::chrono::milliseconds ResponseManager::GetAverageResponseTime()
{
    std::scoped_lock<std::mutex> lock(m_responseLock);

    if (m_responseTimes.empty())
    {
        return std::chrono::milliseconds(100); // Default if no data
    }

    uint64_t sum = 0;
    for (const auto& time : m_responseTimes)
    {
        sum += time.count();
    }

    return std::chrono::milliseconds(sum / m_responseTimes.size());
}

/* std::function<bool()> ResponseManager::GetAsyncHandler(const UID& uid)
{
    return 
        [this, uid]() -> bool
        {
            std::scoped_lock<std::mutex> lock(m_retryLock);

            // Step 1: Check if command already completed
            if (GetCommandStatus(uid))
            {
                return true; // Task is removed from AsyncInjection
            }

            // Step 2: Increment retry count when function executes
            uint64_t& retryCount = m_retryCounts[uid]; 
            retryCount++;

            // Step 3: If retries exceed threshold, fail
            if (retryCount >= s_threshold)
            {
                HandleFailure(uid);
                return true;  // Stops further retries
            }

            // Step 4: Command still pending, reinject task
            return false;
        };   
} */

void ResponseManager::HandleFailure(const UID& uid)
{
    std::scoped_lock<std::mutex> lock(m_mapLock);
    
    m_cmdRegistry.erase(uid);
    /* m_retryCounts.erase(uid); */
}

} // namespace ilrd
