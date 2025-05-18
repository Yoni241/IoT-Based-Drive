#include "response_udp.hpp"
#include "read_response_msg.hpp"    
#include "write_response_msg.hpp"
#include "response_manager.hpp"
#include "factory.hpp"      
#include "master_concrete.hpp"

namespace ilrd
{
                                                                                /*
________________________________________________________________________________

                               NBDResponseCreators 
________________________________________________________________________________
                                                                                */
std::shared_ptr<INBDResponse> ResponseUDP::
                ReadNBDResponseCreator(std::shared_ptr<AMessage> nbdResponseMsg)
{
    auto readNBDResponseMsg =
        std::dynamic_pointer_cast<ReadResponseMsg>(nbdResponseMsg);
    if (nullptr == readNBDResponseMsg)
    {
        std::cout << "ERROR readNBDResponseMsg creator " << std::endl;
    }

    if (!readNBDResponseMsg->GetDataToRead())
    {
        std::cerr << "ERROR: GetDataToRead() returned nullptr!" << std::endl;
    }
    return std::make_shared<ReadNBDResponse>(readNBDResponseMsg->GetDataToRead()
                                           , readNBDResponseMsg->GetNumOfBytes()
                                           , readNBDResponseMsg->GetStatus());
}

std::shared_ptr<INBDResponse> ResponseUDP::WriteNBDResponseCreator
                                      (std::shared_ptr<AMessage> nbdResponseMsg)
{
    auto writeNBDResponseMsg = 
              std::dynamic_pointer_cast<WriteResponseMsg>(nbdResponseMsg);
    if(nullptr == writeNBDResponseMsg)  
    {
        std::cout << "ERROR ReadNBDResponse creator " << std::endl;
    }  

    return std::make_shared<WriteNBDResponse>(writeNBDResponseMsg->GetStatus());
}

                                                                                /*
________________________________________________________________________________

                               ResponseCreators 
________________________________________________________________________________
                                                                                */
    
std::shared_ptr<AMessage> ResponseUDP::ReadResponseCreator()
{
    return std::make_shared<ReadResponseMsg>();
}
std::shared_ptr<AMessage> ResponseUDP::WriteResponseCreator()
{
    return std::make_shared<WriteResponseMsg>();
}

                                                                                /*
________________________________________________________________________________

                                 ResponseUDP 
________________________________________________________________________________
                                                                                */

ResponseUDP::ResponseUDP(UDPSocket& masterUDPSocket)
: m_masterUDPSocket(masterUDPSocket)
{
    Factory<uint64_t, INBDResponse, std::shared_ptr<AMessage>>* 
    nbdResponseFactory = Singleton<Factory<uint64_t,INBDResponse, 
                                std::shared_ptr<AMessage>>>::GetInstance();

    nbdResponseFactory->Add(AMessage::Type::READ, ReadNBDResponseCreator);
    nbdResponseFactory->Add(AMessage::Type::WRITE, WriteNBDResponseCreator);

    Factory<uint64_t, AMessage>* resMsgFactory =
                          Singleton<Factory<uint64_t, AMessage>>::GetInstance();

    resMsgFactory->Add(AMessage::Type::READ, ReadResponseCreator); 
    resMsgFactory->Add(AMessage::Type::WRITE, WriteResponseCreator); 
}

std::shared_ptr<IKeyArgs> ResponseUDP::GetKeyArgs(int fd, Reactor::Mode mode)
{

    (void)(fd);
    if (mode != Reactor::READ)
    {
        throw std::runtime_error("ResponseProxy only supports READ mode");
    }

    std::shared_ptr<AMessage> response = Receive();

    /*bool isDone = Singleton<ResponseManager>::GetInstance()->
                                             HandleResponse(response->GetUID());
                                    
    if (isDone == true)
    { 
        auto responseNBD = 
        Singleton<Factory<uint64_t, INBDResponse, 
                               std::shared_ptr<AMessage>>>::GetInstance()->
                                           Create(response->GetKey(), response); 

        Singleton<MasterSide>::GetInstance()->GetNBDProxy()->
                                                 SendResponseToNBD(responseNBD); // its falling exactly here
    } */

    auto responseNBD = 
        Singleton<Factory<uint64_t, INBDResponse, 
                               std::shared_ptr<AMessage>>>::GetInstance()->
                                           Create(response->GetKey(), response); 

        Singleton<MasterSide>::GetInstance()->GetNBDProxy()->
                                                 SendResponseToNBD(responseNBD);

    std::cout << "\nSHIRA\n" << std::endl;        
    return nullptr;
}

std::shared_ptr<AMessage> ResponseUDP::Receive()
{
    uint64_t buffSizeAndKey[2] = {0};
    
    m_masterUDPSocket.RecvMsg(&buffSizeAndKey, (sizeof(uint64_t) * 2),
                              MSG_PEEK, nullptr);

    uint64_t key = buffSizeAndKey[0];
    uint64_t buffSize = buffSizeAndKey[1];

    std::cout << "ResponseUDP::Receive() key: " << key << std::endl;
    std::cout << "ResponseUDP::Receive() buffSize: " << buffSize << std::endl;

    std::shared_ptr<char[]> msgBuffer(new char[buffSize]);

    m_masterUDPSocket.RecvMsg(msgBuffer.get(), buffSize, 0, nullptr);

    std::shared_ptr<AMessage> resMsg =
        Singleton<Factory<uint64_t, AMessage>>::GetInstance()->Create(key);
        
    resMsg->FromBuffer(msgBuffer.get());

    return resMsg;
}


ReadResponseChecker::ReadResponseChecker(UID uid, uint64_t threshold)
: m_uid(uid), m_threshold(threshold)
{
    // Empty
}

bool ReadResponseChecker::operator()()
{
    /* === Step 1: Check if command already completed === */
    if (Singleton<ResponseManager>::GetInstance()->IsCmdFinished(m_uid) == true)
    {
        return true;
    }

    --m_threshold;

    /* === Step 3: If retries exceed threshold, HandleFailure === */
    if (m_threshold == 0)
    {
        Singleton<ResponseManager>::GetInstance()->HandleFailure(m_uid);

        std::cout << "Error getting Response from cmd: "
                  << m_uid
                  << std::endl;

        return true;
    }

    /* === Step 4: Command still pending, reinject task === */
    return false;
}

WriteResponseChecker::WriteResponseChecker(UID uid, uint64_t threshold)
: m_uid(uid), m_threshold(threshold)
{
    // Empty
}

bool WriteResponseChecker::operator()()
{
    /* === Step 1: Check if command already completed === */
    if (Singleton<ResponseManager>::GetInstance()->IsCmdFinished(m_uid) == true)
    {
        return true;
    }

    --m_threshold;

    std::cout << "THRESHOLD" << m_threshold << std::endl;

    /* === Step 3: If retries exceed threshold, HandleFailure === */
    if (m_threshold == 0)
    {
        Singleton<ResponseManager>::GetInstance()->HandleFailure(m_uid);

        std::cout << "Error getting Response from cmd: "
                  << m_uid
                  << std::endl;

        return true;
    }

    /* === Step 4: Command still pending, reinject task === */
    return false;
}

} // namespace ilrd