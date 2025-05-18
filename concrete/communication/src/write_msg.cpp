#include "write_msg.hpp"
#include <iostream>

namespace ilrd
{
WriteMsg::WriteMsg(UID uid, std::shared_ptr<char[]> dataToWrite, 
                                           uint64_t offset, uint64_t numOfBytes)
: AMessage(WRITE, uid, numOfBytes + sizeof(uint64_t) * 2),
  m_offset(offset), m_numOfBytes(numOfBytes)
{
    m_dataToWrite = std::make_shared<char[]>(numOfBytes); // numOfBytes = 11 

    std::memcpy(m_dataToWrite.get(), dataToWrite.get(), numOfBytes); 
}                                                             

char* WriteMsg::FromBuffer(char *buffer)
{
    buffer = AMessage::FromBuffer(buffer);
    
    std::cout << "WriteMsg::FromBuffer" << std::endl;

    m_offset = *(uint64_t*) buffer;
    buffer += sizeof(uint64_t);
    
    m_numOfBytes = *(uint64_t*) buffer;
    buffer += sizeof(uint64_t);

    m_dataToWrite = std::make_shared<char[]>(m_numOfBytes);

    std::memcpy(m_dataToWrite.get(), buffer, m_numOfBytes);
    buffer += m_numOfBytes;

    return buffer;
}

char* WriteMsg::ToBuffer(char *buffer) const
{
    buffer = AMessage::ToBuffer(buffer);
    std::cout << "WriteMsg::ToBuffer" << std::endl;
    *(uint64_t*) buffer = m_offset;
    buffer += sizeof(uint64_t);

    *(uint64_t*) buffer = m_numOfBytes;
    buffer += sizeof(uint64_t);

    std::memcpy(buffer, m_dataToWrite.get(), m_numOfBytes);
    buffer += m_numOfBytes;

    return buffer;
}

uint64_t WriteMsg::GetBufferSize() const
{
    return AMessage::GetBufferSize() + sizeof(uint64_t) * 2 + m_numOfBytes; //  (AMessage::GetBufferSize() = 40 + 11 + 16) = 67
}

uint64_t WriteMsg::GetOffset() const
{
    return m_offset;
}

uint64_t WriteMsg::GetNumOfBytes() const
{
    return m_numOfBytes;
}

std::shared_ptr<char[]> WriteMsg::GetDataToWrite() const
{
    return m_dataToWrite;
}

} // namespace ilrd