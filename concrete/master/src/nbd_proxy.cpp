#include <stdexcept>         // For std::runtime_error

#include "nbd_proxy.hpp"     // Forward decleration
#include "nbd_key_args.hpp"  // For KeyArgs symbols
#include "factory.hpp"       // For Creating KeyArgs for cmd data

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                                From Master To NBD
________________________________________________________________________________
                                                                                */

ReadNBDResponse::
         ReadNBDResponse(std::shared_ptr<char[]> responseBuffer, 
                                              uint64_t numBytes, uint8_t status)
: m_responseBuffer(responseBuffer), m_numBytes(numBytes), m_status(status)         
{
    if (!m_responseBuffer)
    {
        throw std::runtime_error("ERROR: ReadNBDResponse received a nullptr buffer!");
    }
}

void ReadNBDResponse::ProcessResponse(TCPServer& tcpServer, int clientFD)
{
    std::cout << "ReadNBDResponse::ProcessResponse begin\n" << std::endl;
    
    if (m_status == false)
    {
        BadStatusHandler();
        
        return;
    }
    
    std::cout << "ReadNBDResponse::ProcessResponse middle\n" << std::endl;

    for (size_t i = 0; i < m_numBytes; ++i)
    {
        std::cout << "m_responseBuffer.get()" << *(m_responseBuffer.get() + i) <<std::endl;
    }


    
    tcpServer.SendMsg(m_responseBuffer.get(), m_numBytes, clientFD);
    std::cout << "ReadNBDResponse::ProcessResponse end\n" << std::endl;
}

WriteNBDResponse::WriteNBDResponse(uint8_t status)
:  m_status(status)         
{
    // Empty
}

void WriteNBDResponse::ProcessResponse(TCPServer& tcpServer, int clientFD)
{
    if (!m_status)
    {
        BadStatusHandler();

        return;
    }

    const char* tmp = "MSG HAS BEEN SENT SUCCESSFULLY"; 

    std::shared_ptr<char[]> buffer = std::make_shared<char[]>(strlen(tmp)); 

    std::memcpy(buffer.get(), tmp, strlen(tmp));

    tcpServer.SendMsg(buffer.get(), strlen(tmp), clientFD);
    std::cout << "WriteNBDResponse::ProcessResponse after sending msg" << std::endl;
}

void ReadNBDResponse::BadStatusHandler()
{
    // TODO

    std::cout << "FAIL: ReadNBDResponse::ProcessResponse " << std::endl;
}


void WriteNBDResponse::BadStatusHandler()
{
    // TODO

    std::cout << "FAIL: WriteNBDResponse::ProcessResponse " << std::endl;
}

/*
________________________________________________________________________________

                                From NBD To Master
________________________________________________________________________________
                                                                                */

/* KeyArgs creators for factory */
std::shared_ptr<IKeyArgs> NBDProxy::ReadKeyArgsCreator(const char* buffer)
{
    std::cout << "NBDProxy::ReadKeyArgsCreator" << std::endl;
    uint64_t type = *(char*)buffer;
    buffer += sizeof(char);
    std::cout << "type" << type << std::endl;
    
    uint64_t offset = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);
    std::cout << "offset" << offset << std::endl;
    
    uint64_t numOfBytes = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);
    std::cout << "numOfBytes" << numOfBytes << std::endl;

    std::cout << "ReadKeyArgs done" << std::endl;

    return std::make_shared<ReadKeyArgs>(type, offset, numOfBytes);
}

std::shared_ptr<IKeyArgs> NBDProxy:: WriteKeyArgsCreator(const char* buffer)
{
    uint64_t type = *(char*)buffer;
    buffer += sizeof(char);
    
    uint64_t offset = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    uint64_t numOfBytes = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    std::shared_ptr<char[]>dataToWrite = std::make_shared<char[]>(numOfBytes); // numOfBytes = 11 bytes
    std::memcpy(dataToWrite.get(), buffer, numOfBytes);

    return std::make_shared<WriteKeyArgs>
                                        (type, offset, numOfBytes, dataToWrite);
}

NBDProxy::NBDProxy(std::string ip ,std::string port) : m_tcpServer(ip, port)
{
    m_tcpClientFD = m_tcpServer.AcceptTCPClient();
}
 
std::shared_ptr<IKeyArgs> NBDProxy::GetKeyArgs(int fd, Reactor::Mode mode)
{
    (void)(fd);
    (void)(mode);
    /*
    NBD stub sends msg whos first byte says if its a read or a write request.
    0 - read, 1 - write. 

    byte:       0       1-9         9-17            17-... 
    content:    w/r     offset      numBytes        data to write
    */

    char* request = new char[50];
    memset(request, 0, 50);

    ssize_t bytesRead = m_tcpServer.RecvMsg(request, sizeof(char) * 50, m_tcpClientFD);


    if (bytesRead < static_cast<ssize_t>(sizeof(request)))
    {
        std::cout << "Error Reading" << std::endl;
    }

    char key = static_cast<char>(request[0]);

    std::shared_ptr<IKeyArgs> keyArgs = 
            Singleton<Factory<uint64_t, IKeyArgs, const char*>>::GetInstance()->
                                                           Create(static_cast<uint64_t>(key), request);

    delete [] request;

    return keyArgs;

    /* struct nbd_request request;
    ssize_t bytesRead = read(m_tcpServer.GetFD(), &request, REQUEST_SIZE);

    if (bytesRead != REQUEST_SIZE)
    {
        throw std::runtime_error("Failed to read a full NBD request from the socket");
    }

    uint32_t magic = safeNtohl(request.magic);
    uint32_t type = safeNtohl(request.type);
    uint64_t from = safeNtohll(request.from);
    uint32_t len = safeNtohl(request.len);

    // Validate the request magic
    if (magic != NBD_REQUEST_MAGIC)
    {
        throw std::runtime_error("Invalid request magic received in NBDProxy");
    }

    // Create and return the appropriate KeyArgs instance
    if (type == NBD_CMD_READ)
    {
        return std::make_shared<ReadKeyArgs>(type, from, len);
    }
    else if (type == NBD_CMD_WRITE)
    {
        // Create a buffer for the write operation
        auto buffer = std::shared_ptr<char[]>(new char[len]);
        char input_buffer[] = "kalif is sharmit";
        size_t len =  strlen(input_buffer);

        memcpy(&buffer, &input_buffer, len);
        // Read the data into the buffer
        

        return std::make_shared<WriteKeyArgs>(type, from, len, buffer);
    }

    throw std::runtime_error("Unsupported NBD command type"); */
}

/* Methods for Little|Big Endian Systems */

int NBDProxy::GetTCPClientFD() const
{
    return m_tcpClientFD;
}

void NBDProxy::SendResponseToNBD(std::shared_ptr<INBDResponse> nbdResponse)
{
    nbdResponse->ProcessResponse(m_tcpServer, m_tcpClientFD);
    std::cout << "after sending response to NBD" << std::endl;
}

bool NBDProxy::IsBigEndian()
{
    uint16_t num = 0x0100;
    uint8_t *byte = (uint8_t *)&num;

    // If the first byte is 0x1, it's big-endian.
    return byte[0] == 0x01;
}


uint32_t NBDProxy::safeNtohl(uint32_t value)
{
    if (IsBigEndian())
    {
        return value;
    }
    else
    {
        return ntohl(value);
    }
}

uint64_t NBDProxy::safeNtohll(uint64_t value)
{
    if (IsBigEndian())
    {
        return value;
    }
    else
    {
        return ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) |
               (uint64_t)ntohl(value >> 32);
    }
}
} // namespace ilrd

