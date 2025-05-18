#ifndef __KEY_ARGS_HPP__
#define __KEY_ARGS_HPP__

#include <cstdint>          // For fixed-width integer types 

namespace ilrd
{

class IKeyArgs
{
    public:
        virtual ~IKeyArgs() = default;

        virtual uint64_t GetKey() const = 0;
};

}

#endif  /*__KEY_ARGS_HPP__*/
