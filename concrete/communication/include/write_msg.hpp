#ifndef __WRITE_REQUEST_HPP__
#define __WRITE_REQUEST_HPP__

#include <memory>   // For shared_ptr

#include "amessage.hpp"

namespace ilrd
{
    class WriteMsg : public AMessage
    {
    public:
        WriteMsg() = default;
        WriteMsg(UID uid, std::shared_ptr<char[]> dataToWrite, uint64_t offset,
                                                           uint64_t numOfBytes);
                                             
        virtual ~WriteMsg() = default;

        virtual char *FromBuffer(char *buffer) override;
        virtual char *ToBuffer(char *buffer) const override;
        virtual uint64_t GetBufferSize() const override;

        uint64_t GetOffset() const;
        uint64_t GetNumOfBytes() const;
        std::shared_ptr<char[]> GetDataToWrite() const;

    private:
        std::shared_ptr<char[]> m_dataToWrite;
        uint64_t m_offset;
        uint64_t m_numOfBytes;
    };       
} // namespace ilrd

#endif /* __WRITE_REQUEST_HPP__ */