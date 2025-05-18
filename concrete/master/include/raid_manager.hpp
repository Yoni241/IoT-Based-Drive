/*
    Developer: Yehonatan Peleg
    File:      raid_manager.hpp
    Reviewer:  
    Date: 	   30.1.2025
    Status:    CR in progress
    Version:   1.0


    # need to check what will happened if offset is exactly between two minions
*/

#ifndef __RAID_MANAGER_HPP__
#define __RAID_MANAGER_HPP__
               
#include <nlohmann/json.hpp>	  // To handle json config file

#include "handleton.hpp"          // To make RaidManager singleton
#include "iminion_proxy.hpp"      // To direct requests to UDP minion
#include "input_proxy.hpp"
#include "communication_lib.hpp"  // To Init UDPSocket of master
#include "cmd.hpp"                // for ICmd symbols

namespace ilrd
{

class RAIDManager
{
public:
    friend class Singleton<RAIDManager>;

    using MinionAndOffset = std::pair<std::shared_ptr<IMinionProxy>, uint64_t>;
    using VectorOfPairs = std::shared_ptr<std::vector<MinionAndOffset>>;

    void ConfigUDPMinions(const nlohmann::json& config_minions, 
                                            const nlohmann::json& config_master,
                                                     UDPSocket& masterUDSocket);

    ICmd::BoolFuncAndDeltaPair Read(uint64_t type, 
                                              uint64_t offset, uint64_t length);
    ICmd::BoolFuncAndDeltaPair Write(std::shared_ptr<char[]> dataToWrite, 
                               uint64_t type, uint64_t offset, uint64_t length);

    std::shared_ptr<IInputProxy> GetResponseUDP();                              
       
private:
    RAIDManager() = default;
 
    VectorOfPairs OffsetToMinionHandler(uint64_t type, uint64_t offset);        // may throw out_of_range
    VectorOfPairs VectorOfPairsHandler(uint64_t type, uint64_t primeIndex, 
                                                          uint64_t primeOffset);

    std::chrono::milliseconds ComputeRetryIntervalHandler();

    uint64_t GetPrimeIndex(uint64_t offset) const;
    uint64_t GetPrimeOffset(uint64_t offset) const;

    size_t m_numMinions;  
    size_t m_numBackups;    
    size_t m_minionMemSize;       
    size_t m_primaryMemSize;

    std::vector<std::shared_ptr<IMinionProxy>> m_minionProxies;
    ICmd::BoolFuncAndDeltaPair AsyncHandler(UID uid);

    std::shared_ptr<IInputProxy> m_responseUDP;
};


} // namespace ilrd

#endif	// ILRD_RD161_RAID_MANAGER_HPP 
