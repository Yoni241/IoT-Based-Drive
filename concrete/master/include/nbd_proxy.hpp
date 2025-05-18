#ifndef __NBD_PROXY_HPP__
#define __NBD_PROXY_HPP__

#include <memory>                 // for std::shared_ptr
#include <cstring>                // for std::memcpy
#include <unistd.h>               // for read()
#include <linux/nbd.h>            // Include the NBD protocol definitions
#include <arpa/inet.h>            // For ntohl and similar functions

#include "input_proxy.hpp"        // For IInputProxy  
#include "communication_lib.hpp"  // For TCPServer
namespace ilrd
{

/*
    _______________________________________________________________________
    
                            From Master To NBD
    _______________________________________________________________________  */

class INBDResponse
{
public:
    explicit INBDResponse() = default;
    virtual ~INBDResponse() = default;

    virtual void ProcessResponse(TCPServer& tcpServer, int clientFD) = 0;
    virtual void BadStatusHandler() = 0;
};

/* ========         ReadNBDResponse        ======== */
class ReadNBDResponse : public INBDResponse
{
    public:

    ReadNBDResponse(std::shared_ptr<char[]> responseBuffer, uint64_t numBytes,
                                                                uint8_t status);

    void ProcessResponse(TCPServer& tcpServer, int clientFD);
    void BadStatusHandler();
    
    private:

    std::shared_ptr<char[]> m_responseBuffer;
    uint64_t m_numBytes;
    uint8_t m_status;
};

/* ========         WriteNBDResponse        ======== */
class WriteNBDResponse : public INBDResponse
{
public:

    WriteNBDResponse(uint8_t status);

    void ProcessResponse(TCPServer& tcpServer, int clientFD);
    void BadStatusHandler();
private:
    
    uint8_t m_status;
};

/*
    _______________________________________________________________________
    
                            From NBD To Master
    _______________________________________________________________________  */

class NBDProxy : public IInputProxy
{
public:
    NBDProxy(std::string ip ,std::string port);
    
    std::shared_ptr<IKeyArgs> GetKeyArgs(int fd, Reactor::Mode mode) override;

    int GetTCPClientFD() const;

    void SendResponseToNBD(std::shared_ptr<INBDResponse> nbdResponse);
    
    static std::shared_ptr<IKeyArgs> ReadKeyArgsCreator(const char* buffer);
    static std::shared_ptr<IKeyArgs> WriteKeyArgsCreator(const char* buffer);

   
    private:
    static const size_t REQUEST_SIZE = sizeof(struct nbd_request); // NBD request packet size
    
    TCPServer m_tcpServer;
    int m_tcpClientFD;
 
    uint32_t safeNtohl(uint32_t value);
    uint64_t safeNtohll(uint64_t value);
    bool IsBigEndian();
    
    /* void InitNBD(); */
};

} // namespace ilrd

#endif /* __NBD_PROXY_HPP__ */
