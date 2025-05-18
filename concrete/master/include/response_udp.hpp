#ifndef __RESPONSE_UDP_HPP__
#define __RESPONSE_UDP_HPP__

#include <memory>

#include "communication_lib.hpp" // For UDPSocket, SocketAddr
#include "iminion_proxy.hpp"     // For IMinionProxy
#include "nbd_proxy.hpp"         // For IInputProxy, INBDResponse
#include "amessage.hpp"          // For creators args

namespace ilrd
{

class ResponseUDP : public IInputProxy
{
public:
    explicit ResponseUDP(UDPSocket& masterUDPSocket);
    ~ResponseUDP() = default;
    
    std::shared_ptr<IKeyArgs> GetKeyArgs(int fd, Reactor::Mode mode) override;

    /* --- NBDResponseCreators --- */
    static std::shared_ptr<INBDResponse> ReadNBDResponseCreator
                                     (std::shared_ptr<AMessage> nbdResponseMsg);

    static std::shared_ptr<INBDResponse> WriteNBDResponseCreator
                                     (std::shared_ptr<AMessage> nbdResponseMsg);
                                     
    /* --- ResponseCreators --- */

    static std::shared_ptr<AMessage> ReadResponseCreator();
    
    static std::shared_ptr<AMessage> WriteResponseCreator();

    
        
private:
    UDPSocket& m_masterUDPSocket;    
    std::shared_ptr<AMessage> Receive();
};

struct WriteResponseChecker
{
    WriteResponseChecker(UID uid, uint64_t threshold);

    bool operator()();
    
private:
    UID m_uid;
    uint64_t m_threshold;
};

struct ReadResponseChecker
{
    ReadResponseChecker(UID uid, uint64_t threshold);   

    bool operator()();

private:
    UID m_uid;
    uint64_t m_threshold;
};


} // namespace ilrd

#endif /* __RESPONSE_UDP_HPP__ */
    