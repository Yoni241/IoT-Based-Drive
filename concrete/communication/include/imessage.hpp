#ifndef __IMESSAGE_HPP__
#define __IMESSAGE_HPP__

#include <cstdint>          // For fixed-width integer types 

namespace ilrd
{
    class IMessage
    {
        public:
            virtual char* FromBuffer(char* buffer) = 0;
            virtual char* ToBuffer(char* buffer) const = 0;
            virtual uint64_t GetBufferSize() const = 0;
    };

} // namespace ilrd

#endif /* __IMESSAGE_HPP__ */
