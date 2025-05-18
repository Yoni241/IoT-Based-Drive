#ifndef __MINION_UDP_HPP__
#define __MINION_UDP_HPP__

#include <memory>

#include "communication_lib.hpp" // For UDPSocket, SocketAddr
#include "iminion_proxy.hpp"     // For IMinionProxy
#include "nbd_proxy.hpp"         // For IInputProxy, INBDResponse
#include "amessage.hpp"          // For creators arg

namespace ilrd
{

class MinionUDP : public IMinionProxy
{
public:
    MinionUDP(std::string& minionIP, std::string& minionPort, 
                                                    UDPSocket& masterUDPSocket);
    ~MinionUDP() = default;

    void Write(std::shared_ptr<char[]> dataToWrite, UID uid, 
                                     uint64_t numBytesToWrite, uint64_t offset); 
    void Read(UID uid, uint64_t numBytesToRead, uint64_t offset);

private:
    SocketAddr m_minionAddr;
    UDPSocket& m_masterUDPSocket; 

    void SendRequest(const AMessage& request);
};

} // namespace ilrd

#endif /* __MINION_UDP_HPP__ */
