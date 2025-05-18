#ifndef __INPUT_PROXY_HPP__
#define __INPUT_PROXY_HPP__

#include <memory>    // for std::shared_ptr

#include "reactor.hpp"
#include "key_args.hpp" // for ITools Interface 

namespace ilrd
{
class IInputProxy
{
    public:
        virtual std::shared_ptr<IKeyArgs> GetKeyArgs(int fd, Reactor::Mode) = 0;
        virtual ~IInputProxy() = default;
};

} // namespace ilrd

#endif /* __INPUT_PROXY_HPP__ */