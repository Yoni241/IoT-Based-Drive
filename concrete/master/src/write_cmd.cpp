#include "write_cmd.hpp"
#include "nbd_key_args.hpp"
#include <iostream>

namespace ilrd
{

std::shared_ptr<ICmd> WriteCmd::WriteCmdCreator()
{
    return std::make_shared<WriteCmd>();
}

ICmd::BoolFuncAndDeltaPair WriteCmd::Execute(std::shared_ptr<IKeyArgs> keyArgs)
{
    auto nbdArgs = std::dynamic_pointer_cast<WriteKeyArgs>(keyArgs);
    if (!nbdArgs)
    {
        throw std::invalid_argument("Invalid key args for WriteCmd");
    }

    auto asyncHandler = Singleton<RAIDManager>::GetInstance()->Write
                    (nbdArgs->GetDataToWrite(),
                     nbdArgs->GetKey(), 
                     nbdArgs->GetOffset(), 
                     nbdArgs->GetNumOfBytes());

    // Step 2: Retuen the async function and delta time
    return std::make_pair(asyncHandler.first, asyncHandler.second);
}

} // namespace ilrd
