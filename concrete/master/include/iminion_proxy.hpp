/*
    Developer: Yehonatan Peleg
    File:      iminion_proxy.hpp
    Reviewer:  
    Date: 	   30.1.2025
    Status:    CR in progress
    Version:   1.0

    Description: 
    This file defines an interface for Minion Proxy classes that handle 
    communication with minions using different protocols (TCP, UDP, Bluetooth).
*/

#ifndef __IMINION_PROXY_HPP__
#define __IMINION_PROXY_HPP__

#include <memory>
#include <cstdint>

#include "uid.hpp"

namespace ilrd
{

class IMinionProxy
{
public:
    virtual ~IMinionProxy() {}  // Virtual destructor for proper cleanup

    /**
     * Reads data from the minion at the given offset.
     * 
     * @param offset The starting position for the read operation.
     * @param numBytesToRead The number of bytes to read.
     */
    virtual void Read(UID uid, uint64_t offset, uint64_t numBytesToRead) = 0;
  
    /**
     * Writes data to the minion at the given offset.
     * 
     * @param data A shared pointer to the data to be written.
     * @param offset The starting position for the write operation.
     * @param numBytesToWrite The number of bytes to write.
     */
    virtual void Write(std::shared_ptr<char[]> dataToWrite, UID uid, 
                                uint64_t offset,  uint64_t numBytesToWrite) = 0;
};

} // namespace ilrd

#endif // __IMINION_PROXY_HPP__
