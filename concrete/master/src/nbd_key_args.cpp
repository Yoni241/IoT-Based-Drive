#include <cstring>           // For memcpy
#include <iostream>

#include "nbd_key_args.hpp"  // Forward decleration

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                                Read KeyArgs
________________________________________________________________________________*/
ReadKeyArgs::ReadKeyArgs(uint64_t type, uint64_t offset, uint64_t length)
: m_type(type), m_offset(offset), m_numOfBytes(length)
{
    // Empty
    std::cout << "ReadKeyArgs::ReadKeyArgs CTOR" << std::endl;
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
                                                                                /*
________________________________________________________________________________

                                Write KeyArgs
________________________________________________________________________________*/

WriteKeyArgs::WriteKeyArgs(uint64_t type, uint64_t offset, uint64_t length,
                                                 std::shared_ptr<char[]> buffer)
: m_type(type), 
  m_offset(offset),
  m_numOfBytes(length), 
  m_dataToWrite(buffer)
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

std::shared_ptr<char[]> WriteKeyArgs::GetDataToWrite()
{
    return m_dataToWrite;
}

} // namespace ilrd
