#ifndef __AMESSAGE_HPP__
#define __AMESSAGE_HPP__

#include <cstring>

#include "imessage.hpp"
#include "uid.hpp"

namespace ilrd
{
    class AMessage : public IMessage
    {
    public:
        AMessage() = default;
        AMessage(uint64_t type, UID uid, uint64_t size);
        virtual ~AMessage() = 0; // making AMessage abstract

        virtual char *FromBuffer(char *buffer) override;
        virtual char *ToBuffer(char *buffer) const override;
        virtual uint64_t GetBufferSize() const override;

        uint64_t GetSize() const;
        uint64_t GetKey() const;
        UID GetUID() const;

        enum Type
        {
            READ,
            WRITE
        };

    private:
        uint64_t m_key;
        uint64_t m_size;
        UID m_uid;
    };

  
} // namespace ilrd

#endif /* __AMESSAGE_HPP__ */