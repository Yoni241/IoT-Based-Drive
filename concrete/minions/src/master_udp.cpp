#include <iostream>

#include "master_udp.hpp"
#include "factory.hpp"          // For keyArgs creators 
#include "master_key_args.hpp"  // For keyArgs symbols

namespace ilrd
{

MasterUDP::MasterUDP(std::string ip, std::string port) : m_UDPSocket(ip, port,
                                                             UDPSocket::DO_BIND)
{
    // Empty
}

/* msg creators for factory */
std::shared_ptr<AMessage> MasterUDP::ReadMsgCreator()
{
    return std::make_shared<ReadMsg>();
}

std::shared_ptr<AMessage> MasterUDP::WriteMsgCreator()
{
    return std::make_shared<WriteMsg>();
}

/* KeyArgs creators for factory */
std::shared_ptr<IKeyArgs> MasterUDP::
                            ReadKeyArgsCreator(std::shared_ptr<AMessage> cmdMsg)
{
    auto readCmdMsg = std::dynamic_pointer_cast<ReadMsg>(cmdMsg);
    if (readCmdMsg == nullptr)
    {
        throw(std::runtime_error("Error in ReadKeyArgs Minion"));
    }

    return std::make_shared<ReadKeyArgs>(readCmdMsg->GetKey(),
                                         readCmdMsg->GetUID(),
                                         readCmdMsg->GetOffset(),
                                         readCmdMsg->GetNumOfBytes());
}

std::shared_ptr<IKeyArgs> MasterUDP::
                           WriteKeyArgsCreator(std::shared_ptr<AMessage> cmdMsg)
{
    auto writeCmdMsg = std::dynamic_pointer_cast<WriteMsg>(cmdMsg);
    if (writeCmdMsg == nullptr)
    {
        throw(std::runtime_error("Error in ReadKeyArgs Minion"));
    }

    return std::make_shared<WriteKeyArgs>(writeCmdMsg->GetKey(),
                                          writeCmdMsg->GetUID(),
                                          writeCmdMsg->GetOffset(),
                                          writeCmdMsg->GetNumOfBytes(),
                                          writeCmdMsg->GetDataToWrite());
}

std::shared_ptr<IKeyArgs> MasterUDP::GetKeyArgs(int fd, Reactor::Mode mode)
{
    (void)fd;
    (void)mode; // unused

    std::scoped_lock<std::mutex> lock(m_sockLock);
    // Receive master's data

    uint64_t buffSizeAndKey[2];

    m_UDPSocket.RecvMsg(buffSizeAndKey, sizeof(uint64_t) * 2, MSG_PEEK,
                                                                 &m_masterAddr);

    uint64_t key = buffSizeAndKey[0];
    uint64_t size = buffSizeAndKey[1];

    std::shared_ptr<char[]> msgBuffer = std::make_shared<char[]>(size);
   
    m_UDPSocket.RecvMsg(msgBuffer.get(), size, 0, nullptr);

    // getting Read|Write Msg 
    std::shared_ptr<AMessage> cmdMsg = 
             Singleton<Factory<uint64_t, AMessage>>::GetInstance()->Create(key);

    // Construct message from data
    cmdMsg->FromBuffer(msgBuffer.get());

    // getting Read|Write KeyArgs 
    std::shared_ptr<IKeyArgs> cmdKeyArgs = 
    Singleton<Factory<uint64_t, IKeyArgs, std::shared_ptr<AMessage>>>::
                                            GetInstance()->Create(key, cmdMsg);

    return cmdKeyArgs;
}

void MasterUDP::SendResponseToMaster(const AMessage& responseMsg)
{
    uint64_t buffSize = responseMsg.GetBufferSize();

    std::shared_ptr<char[]> msgBuffer = std::make_shared<char[]>(buffSize);
    responseMsg.ToBuffer(msgBuffer.get());

    {
        std::scoped_lock<std::mutex> lock(m_sockLock);

        m_UDPSocket.SendMsg(m_masterAddr, msgBuffer.get(), buffSize);
    }
}

int MasterUDP::GetMasterUDPFD() const
{   
    return m_UDPSocket.GetFD();
}

} // namespace ilrd

