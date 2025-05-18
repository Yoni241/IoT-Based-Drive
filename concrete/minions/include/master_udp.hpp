#ifndef __MASTER_UDP__
#define __MASTER_UDP__

#include "input_proxy.hpp"        // For IInputProxy 
#include "communication_lib.hpp"  // For UDPSocket, SocketAddr
#include "read_msg.hpp"           // For ReadMsg
#include "write_msg.hpp"          // For WriteMsg

#include <mutex>

namespace ilrd
{

class MasterUDP : public IInputProxy
{
public:
    MasterUDP(std::string ip, std::string port);

    std::shared_ptr<IKeyArgs> GetKeyArgs(int fd, Reactor::Mode mode) override;

    void SendResponseToMaster(const AMessage& responseMsg);

    int GetMasterUDPFD() const;

    static std::shared_ptr<IKeyArgs> 
                                  ReadKeyArgsCreator(std::shared_ptr<AMessage>);

    static std::shared_ptr<IKeyArgs> 
                                 WriteKeyArgsCreator(std::shared_ptr<AMessage>);

    static std::shared_ptr<AMessage> ReadMsgCreator();
    static std::shared_ptr<AMessage> WriteMsgCreator();

private:
    UDPSocket  m_UDPSocket;
    SocketAddr m_masterAddr;
    
    size_t m_minionSize;
    std::mutex m_sockLock;
};

} // namespace ilrd

#endif /* __MASTER_UDP__ */