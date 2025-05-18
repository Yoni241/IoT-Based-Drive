#ifndef __READ_CMD_HPP__
#define __READ_CMD_HPP__

#include <memory>
#include <chrono>

#include "cmd.hpp"
#include "raid_manager.hpp"
#include "nbd_key_args.hpp"
#include "handleton.hpp"

namespace ilrd
{

class ReadCmd : public ICmd
{
public:
    explicit ReadCmd() = default;
    ICmd::BoolFuncAndDeltaPair
                            Execute(std::shared_ptr<IKeyArgs> keyArgs) override;

    static std::shared_ptr<ICmd> ReadCmdCreator();

private:
    
};

} // namespace ilrd

#endif /* __READ_CMD_HPP__ */
