#include <iostream>
#include <cstring>
#include "communication_lib.hpp"
#include <memory>

using namespace ilrd;

#define SERVER_IP "127.0.0.1"
#define SERVER_TCP_PORT "9090"
#define SERVER_UDP_PORT "9091"
#define BUFFER_SIZE 256

void RunUDPServer()
{
    UDPSocket server(SERVER_IP, SERVER_UDP_PORT, UDPSocket::ToBind::DO_BIND);
    std::cout << "UDP Server started. Waiting for messages...\n";

    for (int i = 0; i < 5; ++i)
    {
        std::shared_ptr<char[]> buffer(new char[BUFFER_SIZE]);
        SocketAddr srcAddr;

        size_t bytesReceived = server.RecvMsg(buffer.get(), BUFFER_SIZE, 0, &srcAddr);
        std::cout << "UDP Server received: " << buffer.get() << " (" << bytesReceived << " bytes)\n";

        // Modify response size
        size_t responseSize = 5 + (bytesReceived % 10); // Different response size

        std::memset(buffer.get(), '*', responseSize);
        buffer.get()[responseSize - 1] = '\0'; // Null-terminate response

        size_t bytesSent = server.SendMsg(srcAddr, buffer.get(), responseSize);
        std::cout << "UDP Server echoed: " << buffer.get() << " (" << bytesSent << " bytes) to "
                  << srcAddr.GetIPStr() << ":" << srcAddr.GetPortStr() << std::endl;
    }
}

void RunTCPServer()
{
    TCPServer server(SERVER_IP, SERVER_TCP_PORT);
    std::cout << "TCP Server started. Waiting for client...\n";

    int clientFD = server.AcceptTCPClient();
    std::cout << "TCP Client connected!\n";

    for (int i = 0; i < 5; ++i)
    {
        std::shared_ptr<char[]> buffer(new char[BUFFER_SIZE]);

        size_t bytesReceived = server.RecvMsg(buffer.get(), BUFFER_SIZE, clientFD);
        std::cout << "TCP Server received: " << buffer.get() << " (" << bytesReceived << " bytes)\n";

        // Modify response size
        size_t responseSize = 5 + (bytesReceived % 10); // Different response size

        std::memset(buffer.get(), '#', responseSize);
        buffer.get()[responseSize - 1] = '\0'; // Null-terminate response

        size_t bytesSent = server.SendMsg(buffer.get(), responseSize, clientFD);
        std::cout << "TCP Server echoed: " << buffer.get() << " (" << bytesSent << " bytes)\n";
    }
}

int main()
{
    std::cout << "Starting UDP & TCP Server..." << std::endl;
    RunUDPServer();
    RunTCPServer();
    return 0;
}
