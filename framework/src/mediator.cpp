#include "mediator.hpp"

namespace ilrd
{

/*
________________________________________________________________________________

                                Input Mediator
________________________________________________________________________________*/

InputMediator::InputMediator(Reactor& reactor, const std::vector<Tuple>& items)

{
    for (auto& item : items)
    {
        reactor.Register(std::get<0>(item), std::get<1>(item), 
                             InputMediator::ReactorCallback(std::get<2>(item)));
    }
}
/*
________________________________________________________________________________

                              Reactor Callback
________________________________________________________________________________*/

InputMediator::ReactorCallback::
                             ReactorCallback(std::shared_ptr<IInputProxy> proxy)
    : m_proxy(proxy)
{
}

void InputMediator::ReactorCallback::operator()(int fd, Reactor::Mode mode)
{
    std::shared_ptr<IKeyArgs> keyArgsPtr = m_proxy->GetKeyArgs(fd, mode);

    if (nullptr != keyArgsPtr)
    {
        Singleton<ThreadPool>::
         GetInstance()->Add(std::make_shared<CmdCreatorAndExeTask>(keyArgsPtr));
    }
}

                                                                                /*
________________________________________________________________________________

                                CmdCreatorAndExe
================================================================================
                                                                                
        Command Creator is responsible to create a cmd using factory
        and command 

________________________________________________________________________________
                                                                                */



CmdCreatorAndExeTask::CmdCreatorAndExeTask(std::shared_ptr<IKeyArgs> keyArgsPtr)
: m_keyArgsPtr(keyArgsPtr)
{

}

void CmdCreatorAndExeTask::operator()()
{
    try
    {
        // Step 1: Create a command using the factory
        std::shared_ptr<ICmd> cmd = Singleton<Factory<uint64_t, ICmd>>::
                                  GetInstance()->Create(m_keyArgsPtr->GetKey());

        // Step 2: Execute the command
        ICmd::BoolFuncAndDeltaPair ai = cmd->Execute(m_keyArgsPtr);

        // Step 3: Handle Async Injection if requested
        if (ai.first != nullptr)
        {
            new AsyncInjection(ai.first, ai.second);
        }                                                   
    }
    catch (const std::exception& e)
    {
        // Log any exceptions that occur
        Singleton<Logger>::GetInstance()->
                               Log(e.what(), __FILE__, __LINE__, Logger::ERROR);
        std::cerr << "CmdCreatorAndExeTask Error: " << e.what() << std::endl;
    }
}

}
