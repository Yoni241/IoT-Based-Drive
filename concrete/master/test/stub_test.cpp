
#include <iostream>
#include <thread>

#include <cstring>
#include "communication_lib.hpp"

using namespace ilrd;

int main()
{
    SocketAddr nbdAddr("127.0.0.1", "4952");
    SocketAddr masterAddr("127.0.0.1", "4950");

    int nbdFd = socket(AF_INET, SOCK_STREAM, 0);

    bind(nbdFd, nbdAddr.GetAddr(), nbdAddr.GetStructLen());

    connect(nbdFd, masterAddr.GetAddr(), masterAddr.GetStructLen());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    /*
    NBD stub sends msg whos first byte says if its a read or a write request.
    0 - read, 1 - write. 

    byte:       0       1-9         9-17            17-... 
    content:    w/r     offset      numBytes        data to write
    */



   /* Write Requst 1 */
    char buffer[50] = {0};
    buffer[0] = 1;                 // write cmd
    *((size_t*)(&buffer[1])) = 21; // offset 21
    *((size_t*)(&buffer[9])) = 11; // num bytes of message
    buffer[17] = 'Y';              // msg
    buffer[18] = 'O';              // msg
    buffer[19] = 'N';              // msg
    buffer[20] = 'I';              // msg
    buffer[21] = ' ';              // msg
    buffer[22] = 'Z';              // msg
    buffer[23] = 'B';              // msg
    buffer[24] = 'A';              // msg
    buffer[25] = 'B';              // msg
    buffer[26] = 'I';              // msg
    buffer[27] = 'R';              // msg

    std::cout << "NBD Stub - Sending write request." << std::endl;
    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 30, 0);
    std::cout << "NBD Stub - Received Write response (1 = success, 0 = fail):" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl << std::endl;

    char pause;
    std::cin >> pause; 

    /* Read Request 1 */
    std::cout << "NBD Stub - Sending read request." << std::endl;
    std::memset(buffer, 0, 50);
    buffer[0] = 0;                 // read cmd
    *((size_t*)(&buffer[1])) = 21;  // offset 21
    *((int*)(&buffer[9])) = 11;    // num bytes of message

    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 50, 0);
    std::cout << "NBD Stub - Received response:" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;


    std::cin >> pause; 

    // Write Request 2
    std::memset(buffer, 0, 50);
    buffer[0] = 1;                 // write cmd
    *((size_t*)(&buffer[1])) = 10 + (8388608/2);  // offset 21
    *((size_t*)(&buffer[9])) = 11;    // num bytes of message
    buffer[17] = 'Y';              // msg
    buffer[18] = 'O';              // msg
    buffer[19] = 'N';              // msg
    buffer[20] = 'I';              // msg
    buffer[21] = ' ';              // msg
    buffer[22] = 'b';              // msg
    buffer[23] = 'O';              // msg
    buffer[24] = 'l';              // msg
    buffer[25] = 'U';              // msg
    buffer[26] = 'd';              // msg
    buffer[27] = 'O';              // msg

    std::cout << "NBD Stub - Sending write request." << std::endl;
    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 50, 0);
    std::cout << "NBD Stub - Received Write response (1 = success, 0 = fail):" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << static_cast<bool>(buffer[i]);
    }
    std::cout << std::endl << std::endl;


    std::cin >> pause; 

    // Read Request 2
    std::cout << "NBD Stub - Sending read request." << std::endl;
    std::memset(buffer, 0, 50);
    buffer[0] = 0;                 // read cmd
    *((size_t*)(&buffer[1])) = 10 + (8388608/2);  // offset 
    *((int*)(&buffer[9])) = 11;    // num bytes of message

    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 50, 0);

    std::cout << "NBD Stub - Received response:" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;


    std::cin >> pause; 

    // Write Request 3
    std::memset(buffer, 0, 50);
    buffer[0] = 1;                 // write cmd
    *((size_t*)(&buffer[1])) = 3 + (8388608);
    *((int*)(&buffer[9])) = 11;    // num bytes of message
    buffer[17] = 'Y';              // msg
    buffer[18] = 'O';              // msg
    buffer[19] = 'N';              // msg
    buffer[20] = 'I';              // msg
    buffer[21] = '_';              // msg
    buffer[22] = 'W';              // msg
    buffer[23] = 'A';              // msg
    buffer[24] = 'K';              // msg
    buffer[25] = 'U';              // msg
    buffer[26] = 'L';              // msg
    buffer[27] = 'O';              // msg

    std::cout << "NBD Stub - Sending write request." << std::endl;
    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 50, 0);
    std::cout << "NBD Stub - Received Write response (1 = success, 0 = fail):" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << static_cast<bool>(buffer[i]);
    }
    std::cout << std::endl << std::endl;;


    std::cin >> pause; 

    // Read Request 3
    std::cout << "NBD Stub - Sending read request." << std::endl;
    std::memset(buffer, 0, 50);
    buffer[0] = 0;                 // read cmd
    *((size_t*)(&buffer[1])) = 3 + (8388608);  // offset 
    *((int*)(&buffer[9])) = 11;    // num bytes of message

    send(nbdFd, buffer, 50, 0);
    std::memset(buffer, 0, 50);
    recv(nbdFd, buffer, 50, 0);
    std::cout << "NBD Stub - Received response:" << std::endl;
    for (size_t i = 0; i < strlen(buffer); ++i)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;
    std::cin >> pause;

    close(nbdFd);

    return 0;
}
