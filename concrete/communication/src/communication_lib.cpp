#include "communication_lib.hpp"
#include <iostream>
#include <sys/socket.h>             // For socket()
#include <unistd.h>                 // For FD close()
#include <fcntl.h>
#include <cstring>

namespace ilrd
{
static const int DEFAULT_PROTOCOL = 0;
static const int CLIENT_SUCCESS = 0;
static const int CLIENT_ERROR = -1;
static const int NO_FLAGS = 0;

                                                                                /*
________________________________________________________________________________

                                  SocketAddr
________________________________________________________________________________
                                                                                */

/*
        Cast Explain:
        struct sockaddr is a generic address structure used for various socket
        functions (bind, connect, etc). 
        Its primary role is to act as a generic interface,  
        allowing socket functions to work with different types
        of address families (e.g., IPv4, IPv6).

        struct sockaddr # size is 16 bytes
        {
            sa_family_t sa_family; (short 2 bytes)  // Address family (AF_INET(IPv4), AF_INET6(IPv6)
            char sa_data[14];      (14 bytes)       // Protocol-specific address data

            #sa_data[14]
            #{
            #    sa_data[0..1]  -> sin_port (2 bytes for port)
            #    sa_data[2..5]  -> sin_addr (4 bytes for IP)
            #    sa_data[6..13] -> padding (8 bytes for extensibility)
            #};


        struct sockaddr_in  # size is 16 bytes
        {
        sa_family_t sin_family; (2 bytes) // Address family (AF_INET(IPv4), AF_INET6(IPv6)
        in_port_t sin_port;     (2 bytes) // Port number
        struct in_addr sin_addr;(4 bytes) // IPv4 address
        char sin_zero[8];                 // Padding for extensibility
        };

        for making the struct sockaddr generic. we need buffer that can contain
        any type of data. so we know what the char buffer sa_data purpose.
        But why 14 bytes and not 6 bytes (2 bytes(port) + 4 bytes(IP))?
        the answer is Extensibility. 
            The size of struct sockaddr_in is:
                2 bytes (short) + 2 bytes (short) + 4 bytes (int) = 8 bytes 
                so here we dont need the other 8 bytes 
                but for other protocol family? we need other bytes.  
    */

SocketAddr::SocketAddr(const std::string &IP, const std::string &port)
    : m_IP(IP), m_port(port)

{
    std::memset(&m_addr, 0, sizeof(m_addr));
    struct sockaddr_in *addr_in = reinterpret_cast<struct sockaddr_in *>(&m_addr);
    std::cout << "brull IP: " << IP << std::endl;
    std::cout << "brull PORT: " << port << std::endl;
    sleep(1);

    addr_in->sin_family = AF_INET;                // for IPv4
    addr_in->sin_port = htons(std::stol(m_port)); // stoi is string to long.
                                                  // htons converts port number from the host's byte order to network byte order.

    if (0 == inet_pton(AF_INET, m_IP.c_str(), &addr_in->sin_addr)) // Internet Address Presentation to Network. It converts a human-readable string representation of an IP address
    {
        // Returns 0 if the src string is not a valid IP address.
        throw std::invalid_argument("src string is not a valid IP address");
    }
}

/* SocketAddr::SocketAddr(const std::string& IP, const std::string& port)
    : m_IP(IP), m_port(port)
{
    std::cout << "Creating SocketAddr with IP: " << IP << " and Port: " << port << std::endl;

    if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
    {
        throw std::invalid_argument("Invalid port: [" + port + "]");
    }

    struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(&m_addr);
    addr_in->sin_family = AF_INET;
    addr_in->sin_port = htons(std::stoi(m_port)); 

    if (inet_pton(AF_INET, m_IP.c_str(), &addr_in->sin_addr) == 0)
    {
        throw std::invalid_argument("Invalid IP address: " + IP);
    }
} */


struct sockaddr* SocketAddr::GetAddr() const
{
    return const_cast<struct sockaddr*>(&m_addr);
}

socklen_t SocketAddr::GetStructLen() const
{
    return sizeof(m_addr);
}

std::string SocketAddr::GetIPStr() const
{
    return m_IP;
}

std::string SocketAddr::GetPortStr() const
{
    return m_port;
}

                                                                                /*
________________________________________________________________________________

                                  UDP Socket
________________________________________________________________________________
                                                                                */

/*
    UDP Socket - Establishing a file descriptor for communication via UDP.
    This file  will be used for both sending messages to a receiver
    and receiving messages from a sender.

    Steps to set up the UDP socket:

    1. Create a socket using the socket() function:
       - domain   = AF_INET (IPv4 address family)
       - type     = SOCK_DGRAM (UDP protocol)
       - protocol = 0 (default, auto-select the appropriate protocol for the domain/type)

    2. Bind the socket to the server's IP address and port using the bind() function:
       - sockFD   = file  returned by socket()
       - addr     = struct sockaddr, containing:
           1) domain (AF_INET for IPv4)
           2) port (server's port number)
           3) IP (server's IP address)
       - addrlen  = size of the struct sockaddr

    If any step fails, appropriate cleanup (e.g., closing the file descriptor) is performed,
    and an exception is thrown to indicate the failure.
*/

UDPSocket::UDPSocket(const std::string& IP, const std::string& port, int flag) 
: m_sockAddr(IP, port)
{
    m_FD = socket(AF_INET, SOCK_DGRAM, DEFAULT_PROTOCOL);

    if(CLIENT_ERROR == m_FD)
    {
        throw std::runtime_error("SOCKET CLIENT ERROR");
    }
    
    if(flag == DO_BIND &&
         CLIENT_ERROR == 
                    bind(m_FD, m_sockAddr.GetAddr(), m_sockAddr.GetStructLen()))
    {
        close(m_FD);

        std::cerr << "bind: " << strerror(errno) << std::endl;
        
        throw std::runtime_error("BIND FAILED");
    }
}

UDPSocket::~UDPSocket()
{
    close(m_FD);
}

/*
    SendMsg() - Sends a message to the specified receiver using a UDP socket.

    Parameters:
      - dest:      The destination address (IP and port) encapsulated in a SocketAddr object.
      - msg:       A pointer to the buffer containing the data to be sent.
      - msgLen:    The length of the buffer (number of bytes to send).

    Internally, the function uses the sendto() system call:
      - sockFD:    The file descriptor of the UDP socket (m_FD).
      - buf:       The buffer containing the data to be sent (msg).
      - len:       The size of the buffer (msgLen).
      - flags:     Set to 0 (default, no special flags).
      - addr:      A pointer to the destination address (struct sockaddr), provided by dest.GetAddr().
      - addrlen:   The size of the address structure, provided by dest.GetStructLen().

    Returns:
      - The number of bytes successfully sent.

    Throws:
      - std::runtime_error if the sendto() call fails (returns -1).
*/
size_t UDPSocket::SendMsg(const SocketAddr& dest, const void* msg, size_t msgLen)
{
    std::cout << "[DEBUG] Sending UDP Message to: " << dest.GetIPStr()
              << ":" << dest.GetPortStr() << std::endl;

    ssize_t bytesSent = sendto(m_FD, msg, msgLen, 0, dest.GetAddr(),
                                                           dest.GetStructLen());

    if(CLIENT_ERROR == bytesSent)
    {
        perror("[ERROR] sendto failed"); // Print specific error message
        throw std::runtime_error("SEND MSG ERROR");
    }

    return bytesSent;
}

/* size_t UDPSocket::SendMsg(const SocketAddr& dest, const void* msg, size_t msgLen)
{
    std::cout << std::endl;
    std::cout << "UDPSocket::SendMsg : " << std::endl;
    std::cout << "m_FD : " << m_FD << std::endl;
    std::cout << "msg : " << msg << std::endl;
    std::cout << "msgLen : " << msgLen << std::endl;
    std::cout << "flags : " << 0 << std::endl;
    std::cout << "dest.GetAddr() : " << dest.GetAddr() << std::endl;
    std::cout << "dest.GetStructLen() : " << dest.GetStructLen() << std::endl;
    std::cout << std::endl;

    ssize_t bytesSent = sendto(m_FD, msg, msgLen, 0, dest.GetAddr(),
                                                           dest.GetStructLen());

    if(CLIENT_ERROR == bytesSent)
    {
        throw std::runtime_error("SEND MSG ERROR");
    }

    return bytesSent;
} */

/*
    RecvMsg() - Receives a message from the socket.

    Parameters:
      - bufferOut:   Pointer to the buffer where the received message will be stored.
      - buffSize:    The size of the buffer in bytes.
      - flags:       Flags for the recvfrom() call (e.g., 0 for no special flags).
      - srcAddrOut:  Optional pointer to a SocketAddr object to store the sender's address.
                    - If nullptr, the sender's address will be ignored.

    Returns:
      - The number of bytes received.

    Throws:
      - std::runtime_error if recvfrom() fails.
*/

/* size_t UDPSocket::RecvMsg(void *buffer_out, size_t buffSize, int flags,
                          SocketAddr *srcAddr_out)
{
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);

    struct sockaddr *addr = nullptr;
    socklen_t *ptr_Len = nullptr;

    if (nullptr != srcAddr_out)
    {
        addr = srcAddr_out->GetAddr();
        senderAddrLen = srcAddr_out->GetStructLen();
        ptr_Len = &senderAddrLen;
    }
    else
    {
        addr = reinterpret_cast<struct sockaddr *>(&senderAddr);
        ptr_Len = &senderAddrLen;
    }

    std::cout << "UDPSocket::RecvMsg - Receiving from " << addr << "\n";
    std::cout << "\nUDPSocket::RecvMsg : \n";
    std::cout << "m_FD : " << m_FD << std::endl;
    std::cout << "buffer_out : " << buffer_out << std::endl;
    std::cout << "buffSize : " << buffSize << std::endl;
    std::cout << "flags : " << flags << std::endl;
    std::cout << "addr : " << addr << std::endl;
    std::cout << "ptr_Len : " << ptr_Len << " (" << *ptr_Len << ")\n\n";

    ssize_t bytesReceived = recvfrom(m_FD, buffer_out, buffSize, flags, addr, ptr_Len);

    if (CLIENT_ERROR == bytesReceived)
    {
        throw std::runtime_error("recvfrom failed");
    }

    // **Update srcAddr_out with sender IP & Port**
    if (nullptr != srcAddr_out)
    {
        *srcAddr_out = SocketAddr(inet_ntoa(senderAddr.sin_addr),
                                  std::to_string(ntohs(senderAddr.sin_port)));
    }

    return bytesReceived;
} */

size_t UDPSocket::RecvMsg(void *buffer_out, size_t buffSize, int flags,
                          SocketAddr *srcAddr_out)
{
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);

    struct sockaddr *addr = reinterpret_cast<struct sockaddr *>(&senderAddr);
    socklen_t *ptr_Len = &senderAddrLen;

    ssize_t bytesReceived = recvfrom(m_FD, buffer_out, buffSize, flags, addr, ptr_Len);

    if (CLIENT_ERROR == bytesReceived)
    {
        throw std::runtime_error("recvfrom failed");
    }

    // 🔴 **Ensure the sender's IP and port are properly extracted**
    if (srcAddr_out != nullptr)
    {
        *srcAddr_out = SocketAddr(inet_ntoa(senderAddr.sin_addr),
                                  std::to_string(ntohs(senderAddr.sin_port)));

        // Debug output to confirm the received address
        std::cout << "[DEBUG] UDP Message received from: " << srcAddr_out->GetIPStr()
                  << ":" << srcAddr_out->GetPortStr() << std::endl;
    }

    return bytesReceived;
}

int UDPSocket::GetFD() const
{
    return m_FD;
}

                                                                                /*
________________________________________________________________________________

                                  TCP Manager
________________________________________________________________________________
                                                                                */

const int TCPServer::LISTEN_QUEUE_SIZE = 10;
const int TCPServer::CLIENT_DISCONNECTED = 0;

                                                                               /*
________________________________________________________________________________

                                  TCP Server
________________________________________________________________________________
                                                                                */

/*
    Constructor:
    Initializes a TCP server socket, binds it to the specified IP and port, 
    and sets up a listening queue for incoming client connections.

    If any step fails, the function throws an exception and performs cleanup.

    Parameters:
      - IP: IP address as a string.
      - port: Port number as a string.
*/
TCPServer::TCPServer(const std::string& IP, const std::string& port)
    : m_listenerAddr(IP, port)
{
    m_FDListener = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    
    if (CLIENT_ERROR == m_FDListener)
    {
        throw std::runtime_error("TCP socket creation failed");
    }

    if (CLIENT_ERROR == bind(m_FDListener, m_listenerAddr.GetAddr(),
                                                 m_listenerAddr.GetStructLen()))
    {
        close(m_FDListener);
        throw std::runtime_error("TCP bind failed");
    }

    if (CLIENT_ERROR == listen(m_FDListener, LISTEN_QUEUE_SIZE))
    {
        close(m_FDListener);
        throw std::runtime_error("TCP listen failed");
    }
}

/*
    Destructor:
    Closes the TCP listening socket file descriptor.
*/
TCPServer::~TCPServer()
{
    close(m_FDListener);
}

/*
    AcceptTCPClient() - Accepts an incoming client connection.

    Returns:
      - A new file descriptor for the accepted client connection.

    Throws:
      - std::runtime_error if `accept()` fails.
*/
int TCPServer::AcceptTCPClient()
{
    int newClientFD = accept(m_FDListener, NULL, NULL);
    if (CLIENT_ERROR == newClientFD)
    {
        throw std::runtime_error("TCP accept failed");
    }

    m_clients.insert(newClientFD);

    return newClientFD;
}

/*
    GetFD() - Returns the file descriptor of the TCP server socket.

    Returns:
      - The integer file descriptor associated with the listening socket.
*/
int TCPServer::GetFD() const
{
    return m_FDListener;
}

/*
    SendMsg() - Sends a message to a connected client.

    Parameters:
      - msg: Pointer to the buffer containing the message.
      - msgLen: Length of the buffer (in bytes).
      - clientFD: File descriptor of the target client.

    Returns:
      - The number of bytes successfully sent.

    Throws:
      - std::runtime_error if `send()` fails.
*/

size_t TCPServer::SendMsg(const void* msg, size_t msgLen, int clientFD)
{
    if (m_clients.find(clientFD) == m_clients.end()) 
    {
        throw std::runtime_error("TCPServer::SendMsg => Invalid client file descrIPtor");
    }

    std::cout << std::endl;
    std::cout << "TCPServer::SendMsg : " << std::endl;
    std::cout << "clientFD : " << clientFD << std::endl;
    std::cout << "msg : " << msg << std::endl;
    std::cout << "msgLen : " << msgLen << std::endl;
    std::cout << "NO_FLAGS : " << NO_FLAGS << std::endl;
    std::cout << std::endl;
    
    ssize_t bytesSent = send(clientFD, msg, msgLen, NO_FLAGS);
    if (CLIENT_ERROR == bytesSent)
    {
        throw std::runtime_error("TCP server send failed");
    }
    
    std::cout << "TCPServer::SendMsg THE END" << std::endl;
    
    return bytesSent;
}

/*
    RecvMsg() - Receives a message from a connected client.

    Parameters:
      - bufferOut: Pointer to the buffer where the received data will be stored.
      - buffSize: Maximum size of the buffer (in bytes).
      - clientFD: File descriptor of the client sending the message.

    Returns:
      - The number of bytes received.

    Throws:
      - std::runtime_error if `recv()` fails.
      - If the client disconnects, removes it from the active clients list.
*/

size_t TCPServer::RecvMsg(void* bufferOut, size_t buffSize, int clientFD)
{
    if (m_clients.find(clientFD) == m_clients.end()) 
    {
        throw std::runtime_error("TCPServer::RecvMsg -> Invalid client file descrIPtor");
    }

    std::cout << std::endl;
    std::cout << "TCPServer::RecvMsg : " << std::endl;
    std::cout << "clientFD : " << clientFD << std::endl;
    std::cout << "buffer_out : " << bufferOut << std::endl;
    std::cout << "buffSize : " << buffSize << std::endl;
    std::cout << "NO_FLAGS : " << NO_FLAGS << std::endl;
    std::cout << std::endl;

    ssize_t bytesReceived = recv(clientFD, bufferOut, buffSize, NO_FLAGS);
    
    if (CLIENT_ERROR == bytesReceived)
    {
        throw std::runtime_error("TCP server receive failed");
    }

    if (CLIENT_DISCONNECTED == bytesReceived)
    {
        // Remove client from active connections if it disconnected.
        m_clients.erase(clientFD);
    }

    return bytesReceived;
}

/*
    GetFD() - Returns the file descriptor of the TCP server socket.

    Returns:
      - The integer file descriptor associated with the listening socket.
*/

                                                                                /*
________________________________________________________________________________

                                  TCP Client
________________________________________________________________________________
                                                                                */

/*
    Constructor:
    Initializes a TCP client socket and connects to a specified server.

    - Creates a socket using the `socket()` system call.
    - Connects to the server using the `connect()` system call.

    If any step fails, the function throws an exception.

    Parameters:
      - IP: IP address of the server.
      - port: Port number of the server.
*/

TCPClient::TCPClient(const std::string& IP, const std::string& port)
    : m_serverAddr(IP, port)
{
    m_FDClient = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (CLIENT_ERROR == m_FDClient)
    {
        throw std::runtime_error("TCP client socket creation failed");
    }

    if (CLIENT_ERROR == connect(m_FDClient, m_serverAddr.GetAddr(), m_serverAddr.GetStructLen()))
    {
        std::cerr << "hello" << errno << std::endl;
        close(m_FDClient);
        throw std::runtime_error("TCP client connection failed");
    }
}

/*
    GetClients() - Returns a copy of the set containing all active client file descriptors.

    Returns:
      - An `unordered_set<int>` containing all currently connected clients.
*/
std::unordered_set<int> TCPServer::GetClients() 
{
    return m_clients; // Return a copy of the active client set
}


/*
    Destructor:
    Closes the TCP client socket to free system resources.
*/
TCPClient::~TCPClient()
{
    close(m_FDClient);
}

/*
    SendMsg() - Sends a message to the connected server.

    Parameters:
      - msg: Pointer to the buffer containing the message.
      - msgLen: Length of the buffer (in bytes).

    Returns:
      - The number of bytes successfully sent.

    Throws:
      - std::runtime_error if `send()` fails.
*/
size_t TCPClient::SendMsg(const void* msg, size_t msgLen)
{
    std::cout << std::endl;
    std::cout << "TCPClient::SendMsg : " << std::endl;
    std::cout << "m_FDClient : " << m_FDClient << std::endl;
    std::cout << "msg : " << msg << std::endl;
    std::cout << "msgLen : " << msgLen << std::endl;
    std::cout << "NO_FLAGS : " << NO_FLAGS << std::endl;
    std::cout << std::endl;

    ssize_t bytesSent = send(m_FDClient, msg, msgLen, NO_FLAGS);

    if (CLIENT_ERROR == bytesSent)
    {
        throw std::runtime_error("TCP client send failed");
    }

    return bytesSent;
}

/*
    RecvMsg() - Receives a message from the connected server.

    Parameters:
      - bufferOut: Pointer to the buffer where the received data will be stored.
      - buffSize: Maximum size of the buffer (in bytes).

    Returns:
      - The number of bytes received.

    Throws:
      - std::runtime_error if `recv()` fails.
*/
size_t TCPClient::RecvMsg(void* bufferOut, size_t buffSize)
{
    std::cout << std::endl;
    std::cout << "TCPClient::SendMsg : " << std::endl;
    std::cout << "m_FDClient : " << m_FDClient << std::endl;
    std::cout << "bufferOut : " << bufferOut << std::endl;
    std::cout << "buffSize : " << buffSize << std::endl;
    std::cout << "NO_FLAGS : " << NO_FLAGS << std::endl;
    std::cout << std::endl;

    ssize_t bytesReceived = recv(m_FDClient, bufferOut, buffSize, NO_FLAGS);

    if (CLIENT_ERROR == bytesReceived)
    {
        throw std::runtime_error("TCP client receive failed");
    }

    return bytesReceived;
}

/*
    GetFD() - Returns the file descriptor of the TCP client socket.

    Returns:
      - The integer file descriptor associated with the client socket.
*/
int TCPClient::GetFD() const
{
    return m_FDClient;
}
}