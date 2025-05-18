#ifndef __FRAMEWORK_HPP__
#define __FRAMEWORK_HPP__

#include <memory>
#include <vector>

#include "mediator.hpp"
#include "dir_monitor.hpp"
#include "dll_loader.hpp"
#include "reactor_listener_select.hpp"

/**
 * @file framework.hpp
 * @brief Framework for managing reactor-based systems, dynamic plugin loading, and command handling.
 */

namespace ilrd
{

/**
 * @class Framework
 * @brief A versatile framework for managing event-driven systems and dynamic plugins.
 * 
 * The Framework class integrates multiple components:
 * - Reactor for handling I/O events.
 * - DirMonitor for monitoring directory changes.
 * - DllLoader for dynamically loading shared libraries.
 * - Command handling for executing operations based on user-defined logic.
 * 
 * This class is designed to be extensible and portable, supporting various platform-specific implementations.
 */
class Framework
{
public:
    /**
     * @typedef FdCallbacks
     * @brief A tuple representing file descriptor callbacks.
     * 
     * Contains:
     * - `int`: File descriptor.
     * - `Reactor::Mode`: Mode (e.g., READ or WRITE).
     * - `std::shared_ptr<IInputProxy>`: Proxy for handling input.
     */
    using FdCallbacks = 
              std::tuple<uint64_t, Reactor::Mode, std::shared_ptr<IInputProxy>>;  

    /**
     * @typedef CmdCreators
     * @brief A pair representing command creators.
     * 
     * Contains:
     * - `int`: Command ID.
     * - `std::function<std::shared_ptr<ICmd>()>`: Function to create a command.
     */
    using CmdCreators = 
                    std::pair<uint64_t, std::function<std::shared_ptr<ICmd>()>>;

    /**
     * @brief Constructs a Framework instance.
     * 
     * @param fdCallbacks Vector of file descriptor callbacks.
     * @param creators Vector of command creators for handling commands.
     * @param pluginDirPath Path to the directory containing plugins.
     * @param listen Optional listener for reactor events. 
     *        Defaults to `SelectListener`, which is suitable for Linux systems.
     *        **Note:** For portability across different systems, users can provide their 
     *        own listener by implementing the `Ilistener` interface.
     */
    Framework(std::vector<FdCallbacks> fdCallbacks,
              std::vector<CmdCreators> creators, 
              const std::string& pluginDirPath,
              std::shared_ptr<Reactor::Ilistener> listen =
                                            std::make_shared<SelectListener>()); 

    /**
     * @brief Starts the reactor event loop.
     * 
     * The reactor continuously monitors and processes registered events
     * until the system is shut down.
     */
    void RunReactor();

private:
    Reactor m_reactor; ///< Reactor for managing I/O events.
    DirMonitor m_dirMonitor; ///< Directory monitor for plugin changes.
    DllLoader m_dllLoader; ///< Dynamic library loader for plugins.
};
      
} // namespace ilrd

#endif // __FRAMEWORK_HPP__
