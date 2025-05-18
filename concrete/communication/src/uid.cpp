#include <unistd.h>    // for getpid().
#include <functional>  // For std::hash
#include <iostream>

#include "uid.hpp"

namespace ilrd
{

UID::UID() 
: m_UID(UIDCreate())                         
{
    // MIL
}

bool operator==(const UID& lhs, const UID& rhs)
{
    return UIDIsSame(lhs.GetUID(), rhs.GetUID());
} 


size_t UID::HashUID::operator()(const UID& uid) const
{
    size_t timeHash = std::hash<time_t> ()(uid.m_UID.time_stamp);
    size_t pidHash  = std::hash<pid_t>  ()(uid.m_UID.pid_stamp);
    size_t keyHash  = std::hash<size_t> ()(uid.m_UID.counter);

    return (timeHash ^ (pidHash << 1) ^ (keyHash << 2));
}

uint64_t UID::GetBufferSize() const
{
    return sizeof(m_UID);  // 24 bytes
}

char* UID::ToBuffer(char* buffer) const
{
    *(ilrd_uid_t*) buffer = m_UID;
    buffer += sizeof(ilrd_uid_t);

    return buffer;
}

char* UID::FromBuffer(char* buffer)
{
    m_UID = *(ilrd_uid_t*) buffer;
    buffer += sizeof(ilrd_uid_t);
    
    return buffer;
}

ilrd_uid_t UID::GetUID() const
{
    return m_UID;
}

std::ostream& operator<<(std::ostream& out, const UID& uid)
{
    return out << "counter: " << uid.GetUID().counter    << "\n"
               << "pid: "     << uid.GetUID().pid_stamp  << "\n"
               << "time: "    << uid.GetUID().time_stamp << "\n";
}

} // namespace ilrd