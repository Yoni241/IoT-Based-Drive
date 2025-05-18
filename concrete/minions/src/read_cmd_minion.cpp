#include "read_cmd_minion.hpp"
#include "read_response_msg.hpp"

namespace ilrd
{

std::shared_ptr<ICmd> ReadCmd::ReadCmdCreator()
{
    return std::make_shared<ReadCmd>();
}

ICmd::BoolFuncAndDeltaPair ReadCmd::Execute(std::shared_ptr<IKeyArgs> keyArgs)
{
    auto masterArgs = std::dynamic_pointer_cast<ReadKeyArgs>(keyArgs);
    if (!masterArgs)
    {
        throw std::invalid_argument("Invalid key args for ReadCmd minion");
    }

    // Step 1: PerformRead
    std::shared_ptr<char[]> dataRequested = PerformRead(masterArgs);

    for (size_t i = 0; i < masterArgs->GetNumOfBytes() ; ++i)
    {
        std::cout << dataRequested[i]; 
    }
    std::cout << std::endl;

    uint8_t readStatus = true;
    if(nullptr == dataRequested)
    {
        readStatus = false;
    }

    // Step 2: Create Read Msg to send master back
    ReadResponseMsg readRMsg(masterArgs->GetUID(), masterArgs->GetNumOfBytes(),
                                                     readStatus, dataRequested);

    Singleton<MinionSide>::
                  GetInstance()->GetMasterUDP()->SendResponseToMaster(readRMsg);

    // Step 3: Retuen the async function and delta time with nullptr
    return std::make_pair(nullptr, static_cast<std::chrono::milliseconds>(0));

}

std::shared_ptr<char[]> ReadCmd::
                            PerformRead(std::shared_ptr<ReadKeyArgs> masterArgs)
{
    MinionSide* minion = Singleton<MinionSide>::GetInstance();
    
    uint64_t memOffset = masterArgs->GetOffset();
    std::fstream& memFile = minion->GetMemFile();
    uint64_t memSize = minion->GetMinionMemSize();
   
    if(memOffset >= memSize)
    {
        return nullptr;
    }

    uint64_t numOfBytesToRead = masterArgs->GetNumOfBytes();

    std::cout << "ReadCmd::PerformRead " << std::endl;
    std::cout << "numOfBytesToRead: " << numOfBytesToRead << std::endl;
    std::cout << "memOffset: " << memOffset << std::endl;

    std::shared_ptr<char[]> dataRequested(new char[numOfBytesToRead]);
    
    {
        std::scoped_lock<std::mutex> lock(m_fileLock);
        memFile.seekg(memOffset, std::ios::beg);
        memFile.read(dataRequested.get(), numOfBytesToRead);
    }

    return dataRequested;
}

} // namespace ilrd