#ifndef __WRITE_CMD_MINION_HPP__
#define __WRITE_CMD_MINION_HPP__

#include <memory>
#include <chrono>

#include "cmd.hpp"             // For Icmd 
#include "master_key_args.hpp" // For IKeyArgs

namespace ilrd
{

class WriteCmd : public ICmd
{
public:
    explicit WriteCmd() = default;
    ICmd::BoolFuncAndDeltaPair
                            Execute(std::shared_ptr<IKeyArgs> keyArgs) override;

    static std::shared_ptr<ICmd> WriteCmdCreator();

private:

    std::mutex m_fileLock;

    uint8_t PerformWrite(std::shared_ptr<WriteKeyArgs> masterArgs);
};

} // namespace ilrd

#endif /* __WRITE_CMD_MINION_HPP__ */