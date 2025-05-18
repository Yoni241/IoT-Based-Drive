#include "reactor.hpp"
#include "reactor_listener_select.hpp"
#include <iostream> 

namespace ilrd
{

std::vector<Reactor::ListenPair> SelectListener::Listen
                              (const std::vector<Reactor::ListenPair>& listenTo)
{
    fd_set readfds;
    fd_set writefds;
    int maxFD = 0;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    for (auto pair: listenTo)
    {        
        switch (pair.second)
        {
            case Reactor::READ:
                FD_SET(pair.first, &readfds);
                break;

            case Reactor::WRITE:
                FD_SET(pair.first, &writefds);
                break;
        }

        if (pair.first > maxFD)
        {
            maxFD = pair.first;
        }
    }

    int readyFDs = select(maxFD + 1, &readfds, &writefds, NULL, NULL);

    if (readyFDs < 0)
    {
        std::cerr << "ERROR: select() failed! " << strerror(errno) << std::endl;
    }

    std::vector<Reactor::ListenPair> vectorReadyFDs;

    for (int i = 0; (i < maxFD + 1) && (readyFDs > 0); ++i)
    {
        if (FD_ISSET(i, &readfds))
        {
            vectorReadyFDs.push_back({i, Reactor::READ});
            --readyFDs;
        }
        else if (FD_ISSET(i, &writefds))
        {
            vectorReadyFDs.push_back({i, Reactor::WRITE});
            --readyFDs;
        }
    }   

    return vectorReadyFDs; 
}

} // namespace ilrd
