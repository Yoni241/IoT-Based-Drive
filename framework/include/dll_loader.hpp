#ifndef __ILRD_RD161_DLL_LOADER_HPP__
#define __ILRD_RD161_DLL_LOADER_HPP__

#include <string>
#include <functional>

#include "dir_monitor.hpp"

namespace ilrd
{

class DllLoader
{
public:
    explicit DllLoader(DirMonitor* monitor, 
                                       std::function<void(void)> onLoadFailure);
    ~DllLoader() noexcept = default;
    DllLoader(const DllLoader& other) = delete;
    DllLoader& operator=(const DllLoader& other) = delete;
        
private:
    std::function<void(void)> m_onLoadFailure;

    void Load(const std::string& dllPath);
    void LoadAllPreExisingSharedObjects(DirMonitor* monitor);
    bool IsPathNew(const std::string& dllPath);
    static bool IsFileASharedObject(const std::string& file);

    class Plugin : public ICallback<std::string>
    {
    public:
        Plugin(DllLoader* loader);
        ~Plugin();
        
        void Update(const std::string* dllPath);
        std::unordered_map<std::string, void*>& GetSOHandlesMap();

    private:
        DllLoader* m_loader;
        std::unordered_map<std::string, void*> m_soHandlesMap;
    };

    Plugin plugin;
};

} // namespace ilrd

#endif // __ILRD_RD161_DLL_LOADER_HPP__
