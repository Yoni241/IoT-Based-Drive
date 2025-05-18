#ifndef __READ_RESPONSE_MSG_HPP__
#define __READ_RESPONSE_MSG_HPP__

#include <memory>   // For shared_ptr
#include "amessage.hpp"

namespace ilrd
{
    class ReadResponseMsg : public AMessage
    {
    public:
        ReadResponseMsg() = default;
        ReadResponseMsg(UID uid, uint64_t numOfBytes, uint8_t cmdStatus,
                                                std::shared_ptr<char[]> buffer);
                                                  
        virtual char* FromBuffer(char *buffer) override;
        virtual char* ToBuffer(char *buffer) const override;
        virtual uint64_t GetBufferSize() const override;

        uint8_t GetStatus() const;
        uint64_t GetNumOfBytes() const;
        std::shared_ptr<char[]> GetDataToRead();

    private:
        uint8_t m_cmdStatus;
        uint64_t m_numOfBytes;
        std::shared_ptr<char[]> m_dataToRead;
    };  

} // namespace ilrd

#endif /* __READ_RESPONSE_MSG_HPP__ */