#ifndef __RESPONSE_MANAGER_HPP__
#define __RESPONSE_MANAGER_HPP__

#include <unordered_map>
#include <memory>
#include <mutex>

#include "cmd.hpp"
#include "uid.hpp"
#include "handleton.hpp"

namespace ilrd
{

class ResponseManager
{
public:
    friend class Singleton<ResponseManager>;

    UID RegisterCommand(uint64_t numOfResponses);
    bool HandleResponse(const UID& uid);
    bool IsCmdFinished(const UID& uid);

    // get the average response time
    std::chrono::milliseconds GetAverageResponseTime();

    // log response times
    void RecordResponseTime(std::chrono::milliseconds responseTime);

    //std::function<bool()> GetAsyncHandler(const UID& uid);

    static size_t s_threshold; // Threshold for failures
    
    //
    void HandleFailure(const UID& uid);
    
    private:
    ResponseManager() = default;
    
    std::unordered_map<UID, uint64_t, UID::HashUID> m_cmdRegistry;
    std::mutex m_mapLock;

    std::vector<std::chrono::milliseconds> m_responseTimes;
    std::mutex m_responseLock;

    std::unordered_map<UID, uint64_t, UID::HashUID> m_retryCounts;
    std::mutex m_retryLock;


};

} // namespace ilrd

#endif /* __RESPONSE_MANAGER_HPP__ */
