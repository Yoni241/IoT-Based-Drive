#include "write_cmd_minion.hpp"    // Forward decleration
#include "write_response_msg.hpp"  // For write response msg
#include "minion_concrete.hpp"     // For MinionSide symbol

namespace ilrd
{

std::shared_ptr<ICmd> WriteCmd::WriteCmdCreator()
{
    std::cout << "ICmd::BoolFuncAndDeltaPair WriteCmd:: AM I HERE ????" << std::endl;


    return std::make_shared<WriteCmd>();
}

ICmd::BoolFuncAndDeltaPair WriteCmd::Execute(std::shared_ptr<IKeyArgs> keyArgs)
{
    std::shared_ptr<WriteKeyArgs> masterArgs =
                               std::dynamic_pointer_cast<WriteKeyArgs>(keyArgs);
    if (!masterArgs)
    {
        throw std::invalid_argument("Invalid key args for WriteCmd minion");
    }
    
    // Step 1: PerformWrite
    uint8_t cmdStatus = PerformWrite(masterArgs);

    // Step 2: Create Write Msg to send master back
    WriteResponseMsg writeResMsg(masterArgs->GetUID(), cmdStatus);

    //masterArgs->GetMasterUDP()->SendResponseToMaster(writeRMsg);
    Singleton<MinionSide>::
               GetInstance()->GetMasterUDP()->SendResponseToMaster(writeResMsg);


    std::cout << "ICmd::BoolFuncAndDeltaPair WriteCmd:: AM I HERE ????" << std::endl;
    
    // Step 3: Retuen the async function and delta time with nullptr
    return std::make_pair(nullptr, static_cast<std::chrono::milliseconds>(0));  
}

uint8_t WriteCmd::PerformWrite(std::shared_ptr<WriteKeyArgs> masterArgs)
{
    MinionSide* minion = Singleton<MinionSide>::GetInstance();
    
    size_t memOffset = masterArgs->GetOffset();
    std::fstream& memFile = minion->GetMemFile();
    uint64_t memSize = minion->GetMinionMemSize();

    if(memOffset >= memSize)
    {
        return false;
    }

    for (size_t i = 0; i < masterArgs->GetNumOfBytes(); ++i)
    {
        std::cout << masterArgs->GetDataToWrite().get()[i]; 
    }

    std::cout << std::endl;

    {
        std::scoped_lock<std::mutex> lock(m_fileLock);
        memFile.seekp(memOffset, std::ios::beg);
        memFile.write(masterArgs->GetDataToWrite().get(), 
                                                   masterArgs->GetNumOfBytes());
    }

    return true;
}

} // namespace ilrd