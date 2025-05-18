#include "raid_manager.hpp"     // Forward decleration

#include "minion_udp.hpp"       // For minion_udp
#include "response_manager.hpp" // For response_manager symbols 
#include "response_udp.hpp"     // FOr response udp

namespace ilrd
{

                                                                                /*
________________________________________________________________________________

                                RAID Manager
________________________________________________________________________________
                                                                                */

                                                                                /*
=================================  SET UP  =====================================*/


void RAIDManager::ConfigUDPMinions(const nlohmann::json& configMinions, 
                                             const nlohmann::json& configMaster, 
                                                     UDPSocket& masterUDPSocket)
{
    m_numMinions    = configMinions.at("number_of_minions").get<size_t>();
    m_minionMemSize = 
                  configMinions.at("minion_memory_size_in_bytes").get<size_t>();
    m_numBackups = configMinions.at("number_of_backups").get<size_t>();
                  
    m_primaryMemSize = m_minionMemSize / 2;

    if (m_numBackups >= m_numMinions)
    {
        throw std::out_of_range("invalid backup");
    }

    m_minionProxies.resize(m_numMinions);

    auto &udpConfig = configMinions.at("udp_minion");
    size_t udpCount = udpConfig.at("amount").get<size_t>();

    for (size_t i = 0; i < udpCount; ++i)
    {
        auto& data = udpConfig.at("data").at(i);

        std::string minionIP = data.at("ip").get<std::string>();
        std::string minionPort = data.at("port").get<std::string>();
        std::string masterIP = configMaster.at("master_ip").get<std::string>();
        std::string proxyPort = 
                          configMaster.at("nbd_master_port").get<std::string>();

        m_minionProxies[i] = std::make_shared<MinionUDP>(minionIP, minionPort,
                                                               masterUDPSocket);
    }

    m_responseUDP = std::make_shared<ResponseUDP>(masterUDPSocket);
}
                                                                                /*
==================================  READ  ======================================*/

ICmd::BoolFuncAndDeltaPair RAIDManager::Read(uint64_t type, uint64_t offset, 
    uint64_t length)
{
    VectorOfPairs minions = OffsetToMinionHandler(type, offset);

    if (minions->empty())
    {
        throw std::runtime_error
                            ("Error: No minions available for read operation.");
    }

    UID uid = Singleton<ResponseManager>::GetInstance()->RegisterCommand(1);

    std::shared_ptr<IMinionProxy> minion = minions->at(0).first;
    uint64_t minionOffset = minions->at(0).second;

    minion->Read(uid, minionOffset, length);

    auto readResponseAsync = ReadResponseChecker(uid, 5);                       // 5 for threshold. 
    std::chrono::milliseconds interval = ComputeRetryIntervalHandler();


    return {readResponseAsync, interval};  
    
    /* return AsyncHandler(uid); */
}
                                                                                /*
==================================  WRITE  =====================================*/


ICmd::BoolFuncAndDeltaPair 
        RAIDManager::Write(std::shared_ptr<char[]> dataToWrite, uint64_t type,
        uint64_t offset, uint64_t length)
{
    VectorOfPairs minionsToHandle = OffsetToMinionHandler(type, offset);
    UID uid = Singleton<ResponseManager>::GetInstance()->
                                                  RegisterCommand(m_numBackups); 
                                                      

    // Write to primary and backup minions
    for (const auto& [minion, minionOffset] : *minionsToHandle)
    {   
        minion->Write(dataToWrite, uid, minionOffset, length);
    }

    auto writeResponseAsync = WriteResponseChecker(uid, 5);                     // 5 for threshold.  
    std::chrono::milliseconds interval = ComputeRetryIntervalHandler();


    return {writeResponseAsync, interval};  

    // return AsyncHandler(uid);
}

                                                                                /*
=================================  GETTERS  ====================================*/

std::shared_ptr<IInputProxy> RAIDManager::GetResponseUDP()
{
    return m_responseUDP;
}
                                                                                /*
================================  HANDLERS  ====================================*/

RAIDManager::VectorOfPairs RAIDManager::VectorOfPairsHandler(uint64_t type,
                                      uint64_t primeIndex, uint64_t primeOffset)
{
    VectorOfPairs minionsToHandle = nullptr;
    if (type == 1) // change to macro or union
    {
        /* --- Create Two Pairs Vector (1st Prime, 2nd Backup) --- */

        minionsToHandle = std::make_shared<std::vector<MinionAndOffset>>();
        minionsToHandle->reserve(m_numBackups);
        
        /* --- Insert Prime --- */

        minionsToHandle->emplace_back(m_minionProxies[primeIndex], primeOffset);

        /* --- Insert Backup --- */

        uint64_t backupIndex = primeIndex;
        
        for (size_t idx = 1; idx < m_numBackups; ++idx)
        {
            backupIndex = (backupIndex + 1) % m_numMinions;
            uint64_t backupOffset = (primeOffset % m_primaryMemSize) +
                                                               m_primaryMemSize;

            minionsToHandle->emplace_back(m_minionProxies[backupIndex],
                                                                  backupOffset);
        }
    }
    else if (type == 0)
    {
        minionsToHandle = std::make_shared<std::vector<MinionAndOffset>>(); 
        minionsToHandle->reserve(1);                                            // READ only from one minion 

        minionsToHandle->emplace_back(m_minionProxies[primeIndex], primeOffset);
    }
    else
    {
        throw std::invalid_argument("Unknown type");
    }

    return minionsToHandle;
}

RAIDManager::VectorOfPairs RAIDManager::OffsetToMinionHandler(uint64_t type, 
                                                                uint64_t offset)
{
    if (offset >= (m_numMinions * m_primaryMemSize))
    {
        throw std::out_of_range("invalid offset");
    }

    if (m_numBackups > m_numMinions)
    {
        throw std::out_of_range("invalid backup");
    }

    uint64_t primeIndex = GetPrimeIndex(offset);                                // may throw out_of_range
    uint64_t primeOffset =  GetPrimeOffset(offset);                             // may throw out_of_range

    return VectorOfPairsHandler(type, primeIndex, primeOffset);
}

// Compute retry interval based on response time
std::chrono::milliseconds RAIDManager::ComputeRetryIntervalHandler()
{
    constexpr std::chrono::milliseconds MIN_RETRY_INTERVAL(50);
    constexpr std::chrono::milliseconds MAX_RETRY_INTERVAL(500);

    auto avgResponseTime = Singleton<ResponseManager>::GetInstance()->
                                                       GetAverageResponseTime();

    if (avgResponseTime < MIN_RETRY_INTERVAL)
    {
        return MIN_RETRY_INTERVAL;
    }
    else if (avgResponseTime > MAX_RETRY_INTERVAL)
    {
        return MAX_RETRY_INTERVAL;
    }
    else
    {
        return avgResponseTime + std::chrono::milliseconds(10); // why + 10 milliseconds?
    }
}

/* ICmd::BoolFuncAndDeltaPair RAIDManager::AsyncHandler(UID uid)
{
    std::function<bool()> ai_func = Singleton<ResponseManager>::GetInstance()->
                                                           GetAsyncHandler(uid);

    std::chrono::milliseconds interval = ComputeRetryIntervalHandler();


    return {ai_func, interval};                                                 // Return async handler for retries
} */

uint64_t RAIDManager::GetPrimeIndex(uint64_t offset) const
{
    return  offset / m_primaryMemSize;
}

uint64_t RAIDManager::GetPrimeOffset(uint64_t offset) const
{
    return offset % m_primaryMemSize;    
}

} // namespace ilrd