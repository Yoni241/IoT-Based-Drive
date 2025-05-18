#ifndef __COMMUNICATION_LIB_HPP__
#define __COMMUNICATION_LIB_HPP__

#include <string>        // For std::string
#include <arpa/inet.h>   // For sockaddr structures
#include <unordered_set> // For std::unordered_set
#include <stdexcept>     // For std::invalid_argument, std::runtime_error

namespace ilrd
{

/*________________________________________________________________________________
  
                                  SocketAddr
________________________________________________________________________________*/
class SocketAddr
{
public:
    explicit SocketAddr() = default;
    explicit SocketAddr(const std::string& IP, const std::string& port);

    struct sockaddr* GetAddr() const;
    socklen_t GetStructLen() const;
    std::string GetIPStr() const;
    std::string GetPortStr() const;

private:
    struct sockaddr m_addr;
    std::string m_IP;
    std::string m_port;
};

/*________________________________________________________________________________
  
                                  UDP Socket
________________________________________________________________________________*/
class UDPSocket
{
public:
    explicit UDPSocket() = default;
    explicit UDPSocket(const std::string& IP, const std::string& port,
                                                                      int flag);
    ~UDPSocket();

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket(UDPSocket&&) = delete;
    UDPSocket& operator=(UDPSocket&&) = delete;

    size_t SendMsg(const SocketAddr& dest, const void* msg, size_t msgLen);
    size_t RecvMsg(void* bufferOut, size_t buffSize, int flags, 
                                                        SocketAddr* srcAddrOut);

    enum ToBind
    {
        DO_BIND,
        NO_BIND
    };

    int GetFD() const;

private:
    SocketAddr m_sockAddr;
    int m_FD;                                                        
};

/*________________________________________________________________________________
  
                                  TCP Server
________________________________________________________________________________*/
class TCPServer
{
public:
    explicit TCPServer(const std::string& IP, const std::string& port);
    ~TCPServer();
    TCPServer(const TCPServer& other) = delete;
    TCPServer& operator=(const TCPServer& other) = delete;


    int AcceptTCPClient();
    size_t SendMsg(const void* msg, size_t msgLen, int clientFD);
    size_t RecvMsg(void* bufferOut, size_t buffSIze, int clientFD);

    int GetFD() const;
    std::unordered_set<int> GetClients();

private:
    SocketAddr m_listenerAddr;
    int m_FDListener;
    std::unordered_set<int> m_clients;

    static const int LISTEN_QUEUE_SIZE;
    static const int CLIENT_DISCONNECTED;
};

/*________________________________________________________________________________
  
                                  TCP Client
________________________________________________________________________________*/
class TCPClient
{
public:
    explicit TCPClient(const std::string& IP, const std::string& port);
    ~TCPClient();
    TCPClient(const TCPClient& other) = delete; 
    TCPClient operator=(const TCPClient& other) = delete;
    
    size_t SendMsg(const void* msg, size_t msgLen);
    size_t RecvMsg(void* bufferOut, size_t buffSize);
    
    int GetFD() const;

private:
    SocketAddr m_serverAddr;
    int m_FDClient;
};

} // namespace ilrd

#endif /* __COMMUNICATION_LIB_HPP__ */