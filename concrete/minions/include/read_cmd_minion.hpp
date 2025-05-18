#ifndef __READ_CMD_MINION_HPP__
#define __READ_CMD_MINION_HPP__

#include <memory>
#include <chrono>

#include "cmd.hpp"
#include "master_key_args.hpp"
#include "minion_concrete.hpp"


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

    std::mutex m_fileLock;

    std::shared_ptr<char[]> 
                           PerformRead(std::shared_ptr<ReadKeyArgs> masterArgs);

};

} // namespace ilrd

#endif /* __READ_CMD_MINION_HPP__ */