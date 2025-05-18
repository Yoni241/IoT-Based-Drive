#ifndef __MINION_SIDE_HPP__
#define __MINION_SIDE_HPP__

#include "framework.hpp"      // making Minion singleton
#include "master_udp.hpp"

namespace ilrd
{

class MinionSide
{
public:
    friend class Singleton<MinionSide>;

    ~MinionSide();

    void Config(std::string plugAndPlay, std::string memFilePath, 
               std::string minionIp, std::string minionPort, size_t memorySize);
    void RunMinion();

    size_t GetMinionMemSize() const;
    std::fstream& GetMemFile();
    std::shared_ptr<MasterUDP> GetMasterUDP();

private:
    MinionSide();

    std::string m_minionIp;
    std::string m_minionPort;
    std::string m_memoryFilePath;
    std::string m_PAndPPath;
    size_t m_memorySize;

    std::shared_ptr<Reactor::Ilistener> m_listener;
    std::shared_ptr<Framework> m_framework;
    std::shared_ptr<MasterUDP> m_masterUDP;

    std::fstream m_memFile;

    void Init();
        
    };

}// namespace ilrd

#endif /* __MINION_SIDE_HPP__ */