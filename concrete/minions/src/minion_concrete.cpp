#include "minion_concrete.hpp"  // forward decleration
#include "read_cmd_minion.hpp"  // for read cmd creator
#include "write_cmd_minion.hpp" // for write cmd creator

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define LIGHT_BLUE "\033[1;34m"

namespace ilrd
{

MinionSide::MinionSide() : m_listener(std::make_shared<SelectListener>())
{
    Factory<uint64_t, AMessage>* msgFactory = 
                          Singleton<Factory<uint64_t, AMessage>>::GetInstance();

    msgFactory->Add(AMessage::Type::READ, MasterUDP::ReadMsgCreator);
    msgFactory->Add(AMessage::Type::WRITE, MasterUDP::WriteMsgCreator);        
    
    Factory<uint64_t, IKeyArgs, std::shared_ptr<AMessage>>* keyArgsFactory = 
    Singleton<Factory<uint64_t, IKeyArgs,
                                     std::shared_ptr<AMessage>>>::GetInstance();

    keyArgsFactory->Add(AMessage::Type::READ, MasterUDP::ReadKeyArgsCreator);
    keyArgsFactory->Add(AMessage::Type::WRITE, MasterUDP::WriteKeyArgsCreator);
}
    
MinionSide::~MinionSide()
{
    m_memFile.close();
}

size_t MinionSide::GetMinionMemSize() const
{
    return m_memorySize;
}

std::fstream& MinionSide::GetMemFile() 
{
    return m_memFile;
}

std::shared_ptr<MasterUDP> MinionSide::GetMasterUDP()
{
    return m_masterUDP;
}

void MinionSide::Init()
{
    m_memFile.open(m_memoryFilePath, std::ios::in | std::ios::out);

    m_masterUDP = std::make_shared<MasterUDP>(m_minionIp, m_minionPort);

    std::vector<Framework::FdCallbacks> callbacks;
    callbacks.emplace_back(m_masterUDP->GetMasterUDPFD(), Reactor::READ,    
                                                                   m_masterUDP);

    std::vector<Framework::CmdCreators> creators;
                                                                       
    creators.emplace_back(AMessage::Type::READ,  ReadCmd::ReadCmdCreator);
    creators.emplace_back(AMessage::Type::WRITE, WriteCmd::WriteCmdCreator);

    m_framework = std::make_shared<Framework>(callbacks, creators, m_PAndPPath,
                                                                    m_listener);
}

void MinionSide::Config(std::string plugAndPlay, std::string memFilePath, 
                std::string minionIp, std::string minionPort, size_t memorySize)
{
    m_PAndPPath = plugAndPlay;
    m_memoryFilePath = memFilePath;
    m_minionIp = minionIp;
    m_minionPort = minionPort;
    m_memorySize = memorySize;
}

void MinionSide::RunMinion()
{
    std::cout << "calling Init in RunMinion()" << std::endl;
    Init();

    std::cout << LIGHT_BLUE << "Minion Program Is On" << std::endl;
    
    m_framework->RunReactor();
}

} // namespace ilrd