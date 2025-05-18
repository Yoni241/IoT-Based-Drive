#ifndef __NBD_KEY_ARGS_HPP__
#define __NBD_KEY_ARGS_HPP__

#include <memory>        // For std::shared_ptr

#include "key_args.hpp"  // For IKeyArgs

namespace ilrd
{

class ReadKeyArgs : public IKeyArgs
{
public:
    ReadKeyArgs(uint64_t type, uint64_t from, uint64_t length);
    
    uint64_t GetKey() const override;
    uint64_t GetOffset() const;
    uint64_t GetNumOfBytes() const;
    
private:
    uint64_t m_type;   
    uint64_t m_offset;   
    uint64_t m_numOfBytes; 
};

class WriteKeyArgs : public IKeyArgs
{
public:
    WriteKeyArgs(uint64_t type, uint64_t from, uint64_t length,
                                                std::shared_ptr<char[]> buffer);
    
    uint64_t GetKey() const override;
    uint64_t GetOffset() const;
    uint64_t GetNumOfBytes() const;
    std::shared_ptr<char[]> GetDataToWrite();
                                        
private:
    uint64_t m_type;  
    uint64_t m_offset;  
    uint64_t m_numOfBytes; 
    std::shared_ptr<char[]> m_dataToWrite; // Pointer to the data buffer
};

} // namespace ilrd

#endif /* __NBD_KEY_ARGS_HPP__ */
