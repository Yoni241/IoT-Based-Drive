#include <unordered_map>
#include <dlfcn.h> // for dlopen

#include "dll_loader.hpp"

namespace ilrd
{

/********************** inheritance calss from ICallback **********************/

DllLoader::Plugin::Plugin(DllLoader* loader)
: m_loader(loader)
{
    // empty
}

DllLoader::Plugin::~Plugin()
{
    Unsubscribe();
}

void DllLoader::Plugin::Update(const std::string* dllPath)
{
    if (IsFileASharedObject(*dllPath))
    {
        m_loader->Load(*dllPath);
    }
}

std::unordered_map<std::string, void*>& DllLoader::Plugin::GetSOHandlesMap()
{
    return m_soHandlesMap;
}

/************************** DllLoader API functions ***************************/

DllLoader::DllLoader(DirMonitor* monitor,
                                        std::function<void(void)> onLoadFailure)
: m_onLoadFailure(onLoadFailure), plugin(this)
{
    monitor->Register(&plugin, DirMonitor::DM_CREATE);
    LoadAllPreExisingSharedObjects(monitor);
}

void DllLoader::Load(const std::string& dllPath)
{
    if (IsPathNew(dllPath))
    {
        void* handle = dlopen(dllPath.c_str(), RTLD_NOW);
        if (NULL == handle)
        {
            m_onLoadFailure();
        }
        else
        {
            plugin.GetSOHandlesMap()[dllPath] = handle;
        }
    }
}

void DllLoader::LoadAllPreExisingSharedObjects(DirMonitor* monitor)
{
    for (const auto& file: monitor->GetExistingFiles())
    {
        if (IsFileASharedObject(file))
        {
            Load(file);
        }
    }
}

bool DllLoader::IsPathNew(const std::string& dllPath)
{
    return plugin.GetSOHandlesMap().find(dllPath) 
                                            == plugin.GetSOHandlesMap().end();
}

bool DllLoader::IsFileASharedObject(const std::string& file)
{
    FILE* fp = fopen(file.c_str(), "rb");
    if (NULL == fp) 
    {
        return false;
    }
  
    unsigned char buf[20] = {'\0'};

    fread(buf, 20, 1, fp);

    if ((buf[0] != 0x7F) || (buf[1] != 0x45) || (buf[2] != 0x4c) 
                                                            || (buf[3] != 0x46))
    {
        return false; // not an ELF file
    }
    else if (buf[16] == 3)
    {
        return true;
    }

    fclose(fp);

    return false;
}

} // namespace ilrd
