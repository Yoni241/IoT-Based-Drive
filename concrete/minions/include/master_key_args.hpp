#ifndef __MASTER_KEY_ARGS_HPP__
#define __MASTER_KEY_ARGS_HPP__

#include <memory>      // For std::shared_ptr

#include "master_udp.hpp"

namespace ilrd
{
    // class MasterUDP;

class ReadKeyArgs : public IKeyArgs
{
public:
    ReadKeyArgs(uint64_t type, UID uid, uint64_t offset,
                                                       uint64_t numBytesToRead);

    uint64_t GetKey() const override;
    uint64_t GetOffset() const;
    uint64_t GetNumOfBytes() const;
    UID GetUID() const;

private:
    uint64_t m_type;   
    uint64_t m_offset;   
    uint64_t m_numOfBytes; 
    UID m_uid;
};

class WriteKeyArgs : public IKeyArgs
{
public:
    WriteKeyArgs(uint64_t type, UID uid, uint64_t offset,
        uint64_t numBytesToRead, std::shared_ptr<char[]> dataToWrite);
    
    uint64_t GetKey() const override;
    uint64_t GetOffset() const;
    uint64_t GetNumOfBytes() const;
    UID GetUID() const;
    std::shared_ptr<char[]> GetDataToWrite();
                                        
private:
    uint64_t m_type;  
    uint64_t m_offset;  
    uint64_t m_numOfBytes; 
    UID m_uid;
    std::shared_ptr<char[]> m_dataToWrite; // Pointer to the data buffer to write
};

} // namespace ilrd

#endif /* __MASTER_KEY_ARGS_HPP__ */
