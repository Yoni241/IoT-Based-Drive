#ifndef __WRITE_RESPONSE_MSG_HPP__
#define __WRITE_RESPONSE_MSG_HPP__

#include <memory>        // For shared_ptr

#include "amessage.hpp"  // For Amessage

namespace ilrd
{
    class WriteResponseMsg : public AMessage
    {
    public:
        WriteResponseMsg() = default;

        WriteResponseMsg(UID uid, uint8_t cmdStatus);

        virtual char* FromBuffer(char *buffer) override;
        virtual char* ToBuffer(char *buffer) const override;
        virtual uint64_t GetBufferSize() const override;

        uint8_t GetStatus() const;
    private:
    
        uint8_t m_cmdStatus;
    };  

} // namespace ilrd

#endif /* __WRITE_RESPONSE_MSG_HPP__ */

