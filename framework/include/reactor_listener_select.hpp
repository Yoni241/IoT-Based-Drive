#ifndef __REACTOR_LISTENER_SELECT_HPP__
#define __REACTOR_LISTENER_SELECT_HPP__

#include "reactor.hpp"

namespace ilrd
{

class SelectListener : public Reactor::Ilistener
{
    virtual std::vector<Reactor::ListenPair> Listen
                             (const std::vector<Reactor::ListenPair>& listenTo);
};

} // namespace ilrd

#endif // __REACTOR_LISTENER_SELECT_HPP__
