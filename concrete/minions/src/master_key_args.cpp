#include "master_key_args.hpp"
#include <cstring> 

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                                Read KeyArgs
________________________________________________________________________________*/
ReadKeyArgs::ReadKeyArgs(uint64_t type, UID uid, uint64_t offset,
                                                        uint64_t numBytesToRead)
: m_type(type), m_uid(uid), m_offset(offset), m_numOfBytes(numBytesToRead)
{
    std::cout << "ReadKeyArgs::CTOR" << std::endl;
    std::cout << "type" << type << std::endl;
    std::cout << "offset" << offset << std::endl;
    std::cout << "numBytesToRead" << numBytesToRead << std::endl;
}

uint64_t ReadKeyArgs::GetKey() const
{
    return m_type; // type is playing the key
}

uint64_t ReadKeyArgs::GetOffset() const
{
    return m_offset; 
}

uint64_t ReadKeyArgs::GetNumOfBytes() const
{
    return m_numOfBytes;
}

UID ReadKeyArgs::GetUID() const
{
    return m_uid;
}

                                                                                /*
________________________________________________________________________________

                                Write KeyArgs
________________________________________________________________________________*/

WriteKeyArgs::WriteKeyArgs(uint64_t type, UID uid, uint64_t offset,
                   uint64_t numBytesToRead, std::shared_ptr<char[]> dataToWrite)
: m_type(type), m_uid(uid), m_offset(offset), m_numOfBytes(numBytesToRead),
  m_dataToWrite(dataToWrite)
{
    // Empty
}

uint64_t WriteKeyArgs::GetKey() const
{
return m_type; // type is playing the key
}

uint64_t WriteKeyArgs::GetOffset() const
{
return m_offset; // type is playing the key
}

uint64_t WriteKeyArgs::GetNumOfBytes() const
{
return m_numOfBytes;
}

UID WriteKeyArgs::GetUID() const
{
    return m_uid;
}

std::shared_ptr<char[]> WriteKeyArgs::GetDataToWrite()
{
    return m_dataToWrite;
}

} // namespace ilrd