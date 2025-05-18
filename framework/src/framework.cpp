#include "framework.hpp"

namespace ilrd
{

void OnLoadFailure()
{
    std::cout << "Error loading shared object"  << std::endl;
}

Framework::
         Framework(std::vector<FdCallbacks> fdCallbacks,
           std::vector<CmdCreators> creators, const std::string& pluginDirPath,
                                     std::shared_ptr<Reactor::Ilistener> listen)
: m_reactor(Reactor(listen)),
  m_dirMonitor(pluginDirPath),
  m_dllLoader(&m_dirMonitor, &OnLoadFailure)
{
    InputMediator(m_reactor, fdCallbacks);
    for (auto iter : creators)
    {
        (Singleton<Factory<uint64_t, ICmd>>::GetInstance())->Add(iter.first,
                                                                   iter.second);
    }
}  

void Framework::RunReactor()
{
    m_reactor.Run();
}

}

 