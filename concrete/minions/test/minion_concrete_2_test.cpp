#include <nlohmann/json.hpp>  // To handle json config file

#include "minion_concrete.hpp"

using namespace ilrd;

int main()
{
    MinionSide* minion = Singleton<MinionSide>::GetInstance(); 
    std::string plugAndPlay;
    std::string memFilePath;
    std::string minionIp;
    std::string minionPort;
    size_t memorySize;

    std::ifstream configFile("/home/yoni/git/projects/final_project/concrete/config.json");
    if (!configFile)
	{
        std::cerr << "Error: Could not open config.json\n";
        return false;
    }

	nlohmann::json config;
    configFile >> config;
    
    try 
	{
        plugAndPlay = config.at("MinionsData").at("plug_and_play_path").get<std::string>();

        const nlohmann::json& minionUDPConfig = config.at("MinionsData").at("udp_minion");

        minionIp = minionUDPConfig["data"][1]["ip"];
        minionPort = minionUDPConfig["data"][1]["port"];
        memFilePath = minionUDPConfig["data"][1]["memory_file"];
        memorySize = minionUDPConfig["data"][1]["size"];

		std::string runScriptCmd = "/home/yoni/git/projects/final_project/concrete/scripts/mem_setup.sh \"" + memFilePath + "\"";

		system(runScriptCmd.c_str());
    }
	catch (const std::exception& e)
	{
        std::cerr << "Config error minion 1: " << e.what() << "\n";
        return false;
    }

    minion->Config(plugAndPlay, memFilePath, minionIp, minionPort, memorySize);
   
    minion->RunMinion();
    
    return 0;
}
