#include "uid.hpp"
#include <iostream>

using namespace ilrd;

int main()
{
    UID x;
    UID y;

    std::cout << "counter " << x.GetUID().counter << std::endl;
    std::cout << "pid " << x.GetUID().pid_stamp << std::endl;
    std::cout << "timestamp " << x.GetUID().time_stamp << std::endl;
    std::cout << "size " << x.GetBufferSize() << std::endl;

    if ((x == y))
    {
        std::cout << "operator== SUCCESS" << std::endl;
    }

    return 0;
}