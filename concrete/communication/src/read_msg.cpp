#include <iostream>
#include "read_msg.hpp"

namespace ilrd
{
ReadMsg::ReadMsg(UID uid, uint64_t offset, uint64_t numOfBytes)
: AMessage(READ, uid, sizeof(uint64_t) * 2), 
  m_offset(offset), m_numOfBytes(numOfBytes)
{
    // Empty
}                                                                

char* ReadMsg::FromBuffer(char *buffer)
{
    buffer = AMessage::FromBuffer(buffer);
    std::cout << "ReadMsg::FromBuffer" << std::endl;

    m_offset = *(uint64_t*) buffer;
    buffer += sizeof(uint64_t);

    m_numOfBytes = *(uint64_t*) buffer;
    buffer += sizeof(uint64_t);

    return buffer;
}

char* ReadMsg::ToBuffer(char *buffer) const
{
    buffer = AMessage::ToBuffer(buffer);
    std::cout << "ReadMsg::ToBuffer" << std::endl;
    *(uint64_t*) buffer = m_offset;
    buffer += sizeof(uint64_t);

    *(uint64_t*) buffer = m_numOfBytes;
    buffer += sizeof(uint64_t);

    return buffer;
}

uint64_t ReadMsg::GetBufferSize() const 
{
    return AMessage::GetBufferSize() + sizeof(uint64_t) * 2;
}

uint64_t ReadMsg::GetOffset() const
{
    return m_offset;
}
uint64_t ReadMsg::GetNumOfBytes() const
{
    return m_numOfBytes;
}

} // namespace ilrd

