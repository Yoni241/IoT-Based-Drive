#ifndef __MASTER_CONCRETE_HPP__
#define __MASTER_CONCRETE_HPP__

#include <fstream>             // For open file

#include "raid_manager.hpp"    // For init minions
#include "nbd_proxy.hpp"       // For NBDProxy member
#include "framework.hpp"       // For Framework member

namespace ilrd
{

class MasterSide
{
public:
    bool Config();

    void RunMaster();

    std::shared_ptr<NBDProxy> GetNBDProxy();

private:
    MasterSide(); 
    friend class Singleton<MasterSide>;

    nlohmann::json m_config;
    std::string m_masterIP;
    std::string m_masterNBDPort;
    std::string m_masterUDPPort;
    std::string m_PAndPPath;
    std::string m_SystemDirPath;
    size_t m_numOfBackups;
    size_t m_totalDriveMem;
    std::chrono::milliseconds m_readInterval;

    std::unique_ptr<UDPSocket> m_UDPSocket; // Safe ownership without copy/move

    void LoadConfigFileToSystem();
    void ConfigMasterSide();
    void ConfigMinionsData();
    void InitNBD(); // STUB FOR NOW
    void InitUDPConnection(); 

    std::shared_ptr<Framework> m_framework;
    std::shared_ptr<NBDProxy> m_NBDProxy;
    std::shared_ptr<IInputProxy> m_responseUDP;
    
    void InitFramework();
};

} // namespace ilrd

#endif /* __MASTER_CONCRETE_HPP__ */
