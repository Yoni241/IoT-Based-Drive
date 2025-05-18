#ifndef __WRITE_CMD_HPP__
#define __WRITE_CMD_HPP__

#include "cmd.hpp"
#include "raid_manager.hpp"

namespace ilrd
{

class WriteCmd : public ICmd
{
public:
    WriteCmd() = default;
    BoolFuncAndDeltaPair Execute(std::shared_ptr<IKeyArgs> keyArgs) override;

    static std::shared_ptr<ICmd> WriteCmdCreator();
private:

};

} // namespace ilrd

#endif /* __WRITE_CMD_HPP__ */
