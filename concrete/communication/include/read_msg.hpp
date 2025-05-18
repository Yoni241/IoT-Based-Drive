#ifndef __READ_REQUEST_HPP__
#define __READ_REQUEST_HPP__

#include "amessage.hpp"

namespace ilrd
{
    class ReadMsg : public AMessage
    {
    public:
        ReadMsg() = default;
        ReadMsg(UID uid, uint64_t offset, uint64_t numOfBytes);
        ~ReadMsg() = default;

        char* FromBuffer(char *buffer) override;
        char* ToBuffer(char *buffer) const override;
        uint64_t GetBufferSize() const override;

        uint64_t GetOffset() const;
        uint64_t GetNumOfBytes() const;
    
    private:
        uint64_t m_offset;
        uint64_t m_numOfBytes;
    };
} // namespace ilrd

#endif /* __READ_REQUEST_HPP__ */