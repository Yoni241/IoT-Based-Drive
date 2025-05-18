#define __HANDLETON__

#include "handleton.hpp"
#include "minion_concrete.hpp"

namespace ilrd
{
    
template class Singleton<Factory<uint64_t, IKeyArgs, std::shared_ptr<AMessage>>>;
template class Singleton<Factory<uint64_t, AMessage>>;
template class Singleton<MinionSide>;

}
