#include <iostream>
#include "raid_manager.hpp"

using namespace ilrd;

int main()
{
    uint64_t offset = 4194304 * 3 + 4096;

    /* RAIDManager::MinionAndOffset res = 
                   Singleton<RAIDManager>::GetInstance()->OffsetToProxy(offset);

    std::cout << "Prime minion number: " << res.first.first << std::endl;
    std::cout << "Prime minion offset: " << res.first.second << std::endl;
    std::cout << "BackUp minion number: " << res.second.first << std::endl;
    std::cout << "BackUp minion offset: " << res.second.second << std::endl;
 */
    return 0;
}