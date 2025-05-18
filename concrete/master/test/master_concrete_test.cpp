/*
    The Master Side.

    We want to Init the system. 
    1) We have a config file that having all relevant data
    for starting up the system.   

    2) Run system 
*/

#include "master_concrete.hpp" 

using namespace ilrd;

int main()
{
    MasterSide* masterSide = Singleton<MasterSide>::GetInstance();

    if (masterSide->Config() == false)
    {
        return -1;
    }

    masterSide->RunMaster(); // need to be taking care of. if failed

    return 0; 
}