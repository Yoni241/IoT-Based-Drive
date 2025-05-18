#ifndef __MEDIATOR_HPP__
#define __MEDIATOR_HPP__

#include "input_proxy.hpp"      // For input_proxy
#include "handleton.hpp"        // For handleton
#include "thread_pool.hpp"      // For thread pool 
#include "cmd.hpp"              // For cmd   
#include "factory.hpp"          // For factory 
#include "async_injection.hpp"  // For a_injection

/* 
       PSUEDO/DESIGN:

       MACRO:
       1) register reactor to listen fd's  
       2) getting pointer to cmd_argskey object for building the cmd.
       3) add the task building the cmd to thread pool.
       
       MICRO:
       1) we get fd that ready from Reactor so we need to get Reactor object.

          1.1) we need to create mediator class that will support MACRO(1).
               for that we need to

                - include reactor.hpp to get reactor API  
                - we get fd's and MODE(READ,WRITE) and inputproxy

                - fd -> int that will represent the fd that ready

                    - ready means tcp/udp call as been recieved    

                -  Mode its the kind of mission user want to make (READ,WRITE,(P&P?))
                -  each {fd,Mode} has an instance related that will contain
                   cmd_tools for the cmd. this instace `is a` IInputproxy

                    - include IInputProxy for that instace

                -  we use const vector reference to contain multiply tuples 
                   each tuple will contain (fd, mode, inputproxy). 

          1.2)  -  
                
       2)


            
        Questions:
        1) why vector is const ?
        2)  

    */

namespace ilrd
{


class InputMediator
{   
    public:
        using Tuple = std::tuple<uint64_t, Reactor::Mode, std::shared_ptr<IInputProxy>>;  
        
        InputMediator(Reactor& reactor, const std::vector<Tuple>& items);
        
        struct ReactorCallback
        {
            ReactorCallback(std::shared_ptr<IInputProxy> proxy);

            void operator()(int fd, Reactor::Mode mode);

            private:

            std::shared_ptr<IInputProxy> m_proxy;
        };

    private:

    
};

class CmdCreatorAndExeTask: public ThreadPool::ITPTask
{
    public:
        CmdCreatorAndExeTask(std::shared_ptr<IKeyArgs> keyArgsPtr);

        void operator()();

    private:
        std::shared_ptr<IKeyArgs> m_keyArgsPtr;
};



}


#endif /* __MEDIATOR_HPP__ */