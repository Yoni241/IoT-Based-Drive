#define __HANDLETON__

#include "master_concrete.hpp"
#include "raid_manager.hpp"
#include "response_manager.hpp"
#include "factory.hpp"
#include "nbd_proxy.hpp"
#include "amessage.hpp"

namespace ilrd
{
template class Singleton<ResponseManager>;
template class Singleton<RAIDManager>;
template class Singleton<MasterSide>;
template class Singleton<Factory<uint64_t, IKeyArgs, const char*>>;
template class Singleton<Factory<uint64_t, INBDResponse, std::shared_ptr<AMessage>>>;
template class Singleton<Factory<uint64_t, AMessage>>;
}
