#include <iostream>

#include "write_response_msg.hpp"

namespace ilrd
{
    WriteResponseMsg::WriteResponseMsg(UID uid, uint8_t cmdStatus)
    : AMessage(WRITE, uid, sizeof(uint8_t)),
      m_cmdStatus(cmdStatus)
    {
        // Empty
    }                                                                

    char* WriteResponseMsg::FromBuffer(char *buffer) 
    {
        buffer = AMessage::FromBuffer(buffer);

        std::cout << "WriteResponseMsg::FromBuffer" << std::endl;
        m_cmdStatus = (bool)(*(uint8_t*)buffer);
        buffer += sizeof(uint8_t);

        if (m_cmdStatus == false)
        {
            std::cerr << "write operation has been failed" << std::endl;
        }

        return buffer;
    }

    char* WriteResponseMsg::ToBuffer(char *buffer) const
    {
        buffer = AMessage::ToBuffer(buffer);

        std::cout << "WriteResponseMsg::ToBuffer" << std::endl;


        *(uint8_t*)buffer = m_cmdStatus;
        buffer += sizeof(uint8_t);
    
        return buffer;  
    }

    uint64_t WriteResponseMsg::GetBufferSize() const 
    {
        return AMessage::GetBufferSize() + sizeof(uint8_t);
    }

    uint8_t WriteResponseMsg::GetStatus() const
    {
        return m_cmdStatus;
    }


} // namespace ilrd