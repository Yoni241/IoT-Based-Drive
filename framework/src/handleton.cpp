#define __HANDLETON__

#include "handleton.hpp"
#include "thread_pool.hpp"
#include "factory.hpp"
#include "logger.hpp"
#include "scheduler.hpp"
#include "cmd.hpp"


namespace ilrd
{

template class Singleton<Scheduler>;
template class Singleton<ThreadPool>;
template class Singleton<Factory<int, ThreadPool::ITPTask>>;
template class Singleton<Factory<uint64_t, ICmd>>;
template class Singleton<Logger>;

}

