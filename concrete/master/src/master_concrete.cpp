#include "master_concrete.hpp"  // Forward decleration
#include "amessage.hpp"         // For messages
#include "read_cmd.hpp"         // For Read Creator
#include "write_cmd.hpp"        // For Write Creator
#include "raid_manager.hpp"     // For init minions

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define LIGHT_BLUE "\033[1;34m"

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                                 MasterSide 
________________________________________________________________________________*/


/* ==========                    Constructor                       ========== */

MasterSide::MasterSide()
{
    Factory<uint64_t, IKeyArgs, const char*>* keyArgsFactory = 
             Singleton<Factory<uint64_t, IKeyArgs, const char*>>::GetInstance();

    keyArgsFactory->Add(AMessage::Type::READ, NBDProxy::ReadKeyArgsCreator);
    keyArgsFactory->Add(AMessage::Type::WRITE, NBDProxy::WriteKeyArgsCreator);                             
}

/* ==========                      Getters                         ========== */


std::shared_ptr<NBDProxy> MasterSide::GetNBDProxy()
{
    return m_NBDProxy;
}

/* ==========                   Configurations                     ========== */

/* ---                               Step 1                               --- */

void MasterSide::LoadConfigFileToSystem()
{ 
    std::ifstream configFile("/home/yoni/git/projects/final_project/concrete/config.json");
    if (!configFile) // failure check
    {
        std::cerr << "Error: Could not open config.json\n";
    }

    configFile >> m_config;
}

/* -------------------------------------------------------------------------- */

/* ---                               Step 2                               --- */

void MasterSide::InitUDPConnection()
{
    m_UDPSocket = std::make_unique<UDPSocket>(m_masterIP, m_masterUDPPort,
        UDPSocket::DO_BIND);
}

void MasterSide::ConfigMasterSide()
{
    const nlohmann::json& config = m_config.at("MasterSide");

    m_masterIP = config.at("master_ip").get<std::string>();
    m_masterNBDPort = config.at("nbd_master_port").get<std::string>();
    m_masterUDPPort = config.at("udp_master_port").get<std::string>();
    m_PAndPPath = config.at("plug_and_play_path").get<std::string>();
    m_SystemDirPath = config.at("dir_path").get<std::string>();
    m_totalDriveMem = config.at("total_drive_mem_in_bytes").get<size_t>(); 

    InitUDPConnection(); 
}
/* -------------------------------------------------------------------------- */
/* ---                               Step 3                               --- */

void MasterSide::ConfigMinionsData()
{
    Singleton<RAIDManager>::GetInstance()->
        ConfigUDPMinions(m_config.at("MinionsData"), 
                         m_config.at("MasterSide"),
                         *m_UDPSocket);

    m_responseUDP = Singleton<RAIDManager>::GetInstance()->GetResponseUDP();                         
}

/* -------------------------------------------------------------------------- */

void MasterSide::InitNBD()
{
    m_NBDProxy = std::make_shared<NBDProxy>(m_masterIP, m_masterNBDPort);
}

bool MasterSide::Config()
{
    try
    {
        /* --- Step 1: Load configuration file to system --- */
        LoadConfigFileToSystem();

        /* --- Step 2: Get the relevant data for master --- */
        ConfigMasterSide();

        /* --- Step 3: Get the relevant data for minions--- */
        ConfigMinionsData();
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "JSON Parsing Error in Config: " << e.what() << "\n";

        return false;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid Argument Error in Config: " << e.what() << "\n";

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Runtime Error in Config: " << e.what() << "\n";

        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unknown Error in Config: " << e.what() << "\n";

        return false;
    }

    return true; 
}

/* ==========                      Run Master                      ========== */

void MasterSide::InitFramework()
{
    std::vector<Framework::FdCallbacks> callbacks;

    callbacks.emplace_back(m_NBDProxy->GetTCPClientFD(),
                                                     Reactor::READ, m_NBDProxy);

    callbacks.emplace_back(m_UDPSocket->GetFD(), Reactor::READ, m_responseUDP);                                                     
                                            
    std::vector<Framework::CmdCreators> creators;
    creators.emplace_back(AMessage::Type::READ, ReadCmd::ReadCmdCreator);
    creators.emplace_back(AMessage::Type::WRITE, WriteCmd::WriteCmdCreator);

    m_framework = std::make_shared<Framework>(callbacks, creators, m_PAndPPath);
}

void MasterSide::RunMaster()
{
    InitNBD();
    

    InitFramework();

    std::cout << LIGHT_BLUE << "Master Program Is On" << RESET <<std::endl;

    m_framework->RunReactor();
}

}

