#include "amessage.hpp"
#include <iostream>
namespace ilrd
{
AMessage::AMessage(uint64_t type, UID uid, uint64_t size)
: m_key(type), 
    m_size(size + (sizeof(uint64_t) * 2) + uid.GetBufferSize()),
    m_uid(uid)
{
    // Empty
}

AMessage::~AMessage(){} // dtor definition for linker stage

char* AMessage::FromBuffer(char *buffer)
{
    m_key = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    m_size = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    buffer = m_uid.FromBuffer(buffer);
    
    return buffer;
}

char* AMessage::ToBuffer(char *buffer) const
{
    *(uint64_t*)buffer = m_key;
    buffer += sizeof(uint64_t);

    *(uint64_t*)buffer = m_size;
    buffer += sizeof(uint64_t);

    buffer = m_uid.ToBuffer(buffer);

    return buffer;
}

uint64_t AMessage::GetBufferSize() const
{
    return  (sizeof(uint64_t) * 2) + m_uid.GetBufferSize(); // (16 +  m_uid.GetBufferSize() = 24) = 40 bytes;
}

uint64_t AMessage::GetSize() const
{
    return m_size;
}

uint64_t AMessage::GetKey() const 
{
    return m_key;
}

UID AMessage::GetUID() const 
{
    return m_uid;
}


} // namespace ilrd

