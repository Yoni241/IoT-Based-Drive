#ifndef __CMD_HPP__
#define __CMD_HPP__

#include <functional>
#include <memory>
#include <chrono>

#include "key_args.hpp"

namespace ilrd
{

class ICmd
{

public:
    using BoolFuncAndDeltaPair = std::pair<std::function<bool()>,
                                               const std::chrono::milliseconds>;

    virtual BoolFuncAndDeltaPair Execute(std::shared_ptr<IKeyArgs>) = 0;
};


}



#endif /* __CMD_HPP__ */