#include <iostream>

#include "read_cmd.hpp"

namespace ilrd
{

std::shared_ptr<ICmd> ReadCmd::ReadCmdCreator()
{
    return std::make_shared<ReadCmd>();
}

ICmd::BoolFuncAndDeltaPair ReadCmd::Execute(std::shared_ptr<IKeyArgs> keyArgs)
{
    auto nbdArgs = std::dynamic_pointer_cast<ReadKeyArgs>(keyArgs);
    if (!nbdArgs)
    {
        throw std::invalid_argument("Invalid key args for ReadCmd");
    }

    // Step 1: Call RAIDManager::Read() and get the async function + delta time
    auto asyncHandler = Singleton<RAIDManager>::GetInstance()->Read(
        nbdArgs->GetKey(), 
        nbdArgs->GetOffset(), 
        nbdArgs->GetNumOfBytes()
    );

    // Step 2: Retuen the async function and delta time
    return std::make_pair(asyncHandler.first, asyncHandler.second);
}

} // namespace ilrd