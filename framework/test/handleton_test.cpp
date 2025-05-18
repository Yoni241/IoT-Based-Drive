#include <iostream>
#include <dlfcn.h>

#include "handleton.hpp"
#include "thread_pool.hpp"

using namespace ilrd;


int main(void)
{

    // Load the shared library
    void *Handler = dlopen("bin/debug/libSingleton.so", RTLD_LAZY);
    if (NULL == Handler)
    {
        std::cerr << "Failed to open libSingleton.so: " << dlerror() << std::endl;
        return 1;
    }

    // Load the symbol
    void* GetTpFromLib = dlsym(Handler, "GetTp");
    if (NULL == GetTpFromLib)
    {
        std::cerr << "Failed to get symbol GetTp: " << dlerror() << std::endl;
        dlclose(Handler);
        return 1;
    }

    // Cast and call the function
    ThreadPool* (*local_func)(void) = reinterpret_cast<ThreadPool*(*)(void)>(GetTpFromLib);
    ThreadPool* tp = local_func();

    // Use the ThreadPool (example usage)
    if (tp)
    {
        std::cout << "ThreadPool successfully retrieved!" << std::endl;
    }
    else
    {
        std::cerr << "Failed to retrieve ThreadPool." << std::endl;
    }

    // Close the library
    dlclose(Handler);

    return 0;
}




