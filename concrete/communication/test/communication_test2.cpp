#include <iostream>
#include <cstring>
#include <unistd.h> // sleep()
#include "communication_lib.hpp"
#include <memory>

using namespace ilrd;

#define SERVER_IP "127.0.0.1"
#define SERVER_TCP_PORT "9090"
#define SERVER_UDP_PORT "9091"
#define CLIENT_UDP_PORT "9094"
#define BUFFER_SIZE 256

void RunUDPClient()
{
    UDPSocket client(SERVER_IP, CLIENT_UDP_PORT, UDPSocket::ToBind::DO_BIND);
    SocketAddr dest(SERVER_IP, SERVER_UDP_PORT);

    std::cout << "UDP Client started. Sending messages...\n";

    for (int i = 0; i < 5; ++i)
    {
        size_t messageSize = 5 + (i * 10); // Varying size

        std::shared_ptr<char[]> buffer(new char[BUFFER_SIZE]);
        std::memset(buffer.get(), 'A' + i, messageSize); // Fill with 'A', 'B', etc.
        buffer.get()[messageSize - 1] = '\0'; // Null-terminate

        size_t bytesSent = client.SendMsg(dest, buffer.get(), messageSize);
        std::cout << "UDP Client sent: " << buffer.get() << " (" << bytesSent << " bytes)\n";

        // Receive response
        size_t bytesReceived = client.RecvMsg(buffer.get(), BUFFER_SIZE, 0, nullptr);
        std::cout << "UDP Client received: " << buffer.get() << " (" << bytesReceived << " bytes)\n";

        sleep(1); // Simulate delay
    }
}

void RunTCPClient()
{
    TCPClient client(SERVER_IP, SERVER_TCP_PORT);
    std::cout << "TCP Client connected. Sending messages...\n";

    for (int i = 0; i < 5; ++i)
    {
        size_t messageSize = 5 + (i * 10); // Varying size

        std::shared_ptr<char[]> buffer(new char[BUFFER_SIZE]);
        std::memset(buffer.get(), 'Z' - i, messageSize); // Fill with 'Z', 'Y', etc.
        buffer.get()[messageSize - 1] = '\0'; // Null-terminate

        size_t bytesSent = client.SendMsg(buffer.get(), messageSize);
        std::cout << "TCP Client sent: " << buffer.get() << " (" << bytesSent << " bytes)\n";

        // Receive response
        size_t bytesReceived = client.RecvMsg(buffer.get(), BUFFER_SIZE);
        std::cout << "TCP Client received: " << buffer.get() << " (" << bytesReceived << " bytes)\n";

        sleep(1); // Simulate delay
    }
}

int main()
{
    std::cout << "Starting UDP & TCP Client..." << std::endl;
    RunUDPClient();
    RunTCPClient();
    return 0;
}
