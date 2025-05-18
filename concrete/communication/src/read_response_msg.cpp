#include <iostream>
#include "read_response_msg.hpp"

namespace ilrd
{
ReadResponseMsg::ReadResponseMsg(UID uid, uint64_t numOfBytes, 
                              uint8_t cmdStatus, std::shared_ptr<char[]> buffer)
: AMessage(READ, uid, (sizeof(uint64_t) + numOfBytes + sizeof(uint8_t))),
    m_cmdStatus(cmdStatus), 
    m_numOfBytes(numOfBytes)
{


    m_dataToRead = std::make_shared<char[]>(numOfBytes);
     
    std::memcpy(m_dataToRead.get(), buffer.get(), numOfBytes);
}                                                                

char* ReadResponseMsg::FromBuffer(char *buffer)
{
    buffer = AMessage::FromBuffer(buffer);

    std::cout << "ReadResponseMsg::FromBuffer" << std::endl;

    m_cmdStatus = *(int8_t*)buffer;
    buffer += sizeof(int8_t);

    if (m_cmdStatus == false)
    {
        std::cerr << "read operation has been failed" << std::endl;
    }

    m_numOfBytes = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    m_dataToRead = std::make_shared<char[]>(m_numOfBytes);

    std::memcpy(m_dataToRead.get(), buffer, m_numOfBytes);
    buffer += m_numOfBytes; 

    return buffer;
}

char* ReadResponseMsg::ToBuffer(char *buffer) const
{
    buffer = AMessage::ToBuffer(buffer);
    std::cout << "ReadResponseMsg::ToBuffer" << std::endl;
    *(int8_t*)buffer = m_cmdStatus;
    buffer += sizeof(int8_t);

    *(uint64_t*)buffer = m_numOfBytes;
    buffer += sizeof(uint64_t);

    if(false == m_cmdStatus)
    {
        return buffer;
    }

    std::memcpy(buffer, m_dataToRead.get(), m_numOfBytes);

    return buffer + m_numOfBytes;  
}

uint64_t ReadResponseMsg::GetBufferSize() const 
{
    return AMessage::GetBufferSize() + sizeof(int64_t) + sizeof(uint8_t) 
                                                                + m_numOfBytes;
}

uint8_t ReadResponseMsg::GetStatus() const
{
    return m_cmdStatus;
}

std::shared_ptr<char[]> ReadResponseMsg::GetDataToRead()
{
    return m_dataToRead;
}

uint64_t ReadResponseMsg::GetNumOfBytes() const
{
    return m_numOfBytes;
}


} // namespace ilrd