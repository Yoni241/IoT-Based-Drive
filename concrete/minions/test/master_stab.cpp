#include <iostream>
#include <cstring>
#include <memory>

#include "write_msg.hpp"
#include "write_response_msg.hpp"
#include "read_response_msg.hpp"
#include "read_msg.hpp"
#include "communication_lib.hpp"

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"


using namespace ilrd;

static void WriteToMinion(TCPClient& tcp_socket)
{
    std::shared_ptr<char[]> buffer = std::make_shared<char[]>(18);
    char str[] = "Alon Sharmit HOMO";

    std::memcpy(buffer.get(), str, strlen(str) + 1);

    UID uid;

    WriteMsg w_m(uid, buffer, 0, strlen(str) + 1);

    char* msgBuffer = new char[](w_m.GetBufferSize());

    w_m.ToBuffer(msgBuffer);

    tcp_socket.SendMsg(msgBuffer, w_m.GetBufferSize());

    delete [] msgBuffer;
}
 
static void GetRespAfterWrite(TCPClient& tcp_socket)
{
    
    char buffer[30];
    if (tcp_socket.RecvMsg(buffer, (sizeof(uint64_t) * 2)) <= 0)
    {
        std::cerr << "Error: RecvMsg failed" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < 30; ++i)
        {
            std::cout << buffer[i];
        }

        std::cout << std::endl;   
}

static void ReadFromMinion(TCPClient& tcp_socket, SocketAddr* minion)
{
    std::shared_ptr<char[]> buffer = std::make_shared<char[]>(18);
    
    UID uid;

    ReadMsg r_m(uid, 0, 18);

    char* msgBuffer = new char[](r_m.GetBufferSize());

    r_m.ToBuffer(msgBuffer);

    tcp_socket.SendMsg(*minion, msgBuffer, r_m.GetBufferSize());

    delete [] msgBuffer;
}

static void GetRespAfterRead(TCPClient& tcp_socket, SocketAddr* minion)
{
    uint64_t keyAndSize[2] = {0};
    
    if (tcp_socket.RecvMsg(keyAndSize, (sizeof(uint64_t) * 2), MSG_PEEK, nullptr) <= 0)
    {
        std::cerr << "Error: RecvMsg failed" << std::endl;
        return;
    }
    
    uint64_t size = keyAndSize[1];
    // uint64_t key = keyAndSize[0];
    
    ReadResponseMsg rrm;
 
    char* msgBuffer = new char[](size);

    if (tcp_socket.RecvMsg(msgBuffer, size, 0, nullptr) <= 0)
    {
        std::cerr << "Error: RecvMsg failed" << std::endl;
        return;
    }

    rrm.FromBuffer(msgBuffer);

    delete [] msgBuffer;

    if (rrm.GetStatus() == true)
    {
        std::cout << GREEN << "Massage Has been Successfully read " << RESET << std::endl;
        
        for (size_t i = 0; i < rrm.GetNumOfBytes(); ++i)
        {
            std::cout << rrm.GetDataToRead().get()[i];
        }

        std::cout << std::endl;
    }
    else
    {
        std::cout << RED << "Massage Fail to be read " << RESET << std::endl;
    }
}

int main()
{
    TCPClient tcp_socket("127.0.0.1", "5001"); 
    SocketAddr minion("127.0.0.1", "6011");

    WriteToMinion(tcp_socket);
    // std::cout << PURPLE << "Writing To Minion " << RESET << std::endl;
    
    // std::cout << std::endl;
    GetRespAfterWrite(tcp_socket);
    
    ReadFromMinion(tcp_socket, &minion);
    // std::cout << PURPLE << " Read From Minion " << RESET << std::endl;

    GetRespAfterRead(tcp_socket, &minion); 
   
    return 0;
}