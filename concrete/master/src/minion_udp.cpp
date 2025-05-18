#include <stdexcept>      // For runtime_error
    
#include "read_msg.hpp"   // For wrapping the read cmd data
#include "write_msg.hpp"  // For wrapping the write cmd data
#include "minion_udp.hpp" // Forward decleration
#include "read_response_msg.hpp"    
#include "write_response_msg.hpp"
#include "factory.hpp"      // For 
#include "response_manager.hpp"
#include "master_concrete.hpp"

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                                 Minion UDP 
________________________________________________________________________________*/

MinionUDP::MinionUDP(std::string& minionIP, std::string& minionPort, 
                                                     UDPSocket& masterUDPSocket)
: m_minionAddr(minionIP, minionPort), 
  m_masterUDPSocket(masterUDPSocket) 
{
    // Empty
}

void MinionUDP::Read(UID uid, uint64_t offset, uint64_t numBytesToRead) 
{
    std::cout << "\nMinionUDP::Read" << std::endl;
    std::cout << "offset" << offset << std::endl;
    std::cout << "numBytesToRead" << numBytesToRead << std::endl;

    SendRequest(ReadMsg(uid, offset, numBytesToRead)); 
}

void MinionUDP::Write(std::shared_ptr<char[]> dataToWrite, UID uid,
                                      uint64_t offset, uint64_t numBytesToWrite)
{

    std::cout << "MinionUDP::Write" << std::endl;
    std::cout << "offset" << offset << std::endl;
    std::cout << "numBytesToWrite" << numBytesToWrite << std::endl;
    for (size_t i = 0; i < numBytesToWrite; ++i)
    {
        std::cout << dataToWrite[i];
    }
    std::cout << std::endl;

    SendRequest(WriteMsg(uid, dataToWrite, offset, numBytesToWrite)); 
}

void MinionUDP::SendRequest(const AMessage& request)
{
    uint64_t size = request.GetBufferSize();

    std::cout << "the size of msg: " << size <<std::endl; 

    std::shared_ptr<char[]> buffer = std::make_shared<char[]>(size);

    request.ToBuffer(buffer.get());
    
    ssize_t bytesSent = m_masterUDPSocket.SendMsg(m_minionAddr, buffer.get(),
                                                                          size);

    if (bytesSent < 0)
    {
        std::cout << "Error Sending msg (MinionUDP::SendRequest)" << std::endl;
    }
}

} // namespace ilrd
