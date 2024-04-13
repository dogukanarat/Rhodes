#include <fstream>

#include "../include/rhodeus/Configuration/Configuration.hpp"
#include "../include/rhodeus/Configuration/DefaultConfiguration.hpp"

using namespace Rhodeus;

Configuration& Configuration::initialize(const std::string& configFilePath)
{
    for (;;)
    {
        PLOGI << fmt::format("Configuration initialized with file {}", configFilePath);

        if (false == std::filesystem::is_regular_file(configFilePath))
        {
            PLOGW << fmt::format("Configuration file not found");

            createDefaultConfigFile(configFilePath);
        }

        loadConfigFile(configFilePath);

        break;
    }

    return *this;
}

void Configuration::createDefaultConfigFile(const std::string& configFilePath)
{
    for (;;)
    {
        PLOGD << fmt::format("Creating default configuration file");

        std::ofstream configFile(configFilePath);

        if (false == configFile.is_open())
        {
            PLOGE << fmt::format("Failed to create configuration file");
            break;
        }

        mConfig = defaultConfiguration();

        configFile << mConfig.dump(4);

        configFile.close();

        break;
    }
}

void Configuration::loadConfigFile(const std::string& configFilePath)
{
    for (;;)
    {
        PLOGD << fmt::format("Loading configuration file");

        std::ifstream configFile(configFilePath);

        if (false == configFile.is_open())
        {
            PLOGE << fmt::format("Failed to open configuration file");
            break;
        }

        auto fileJson = json::json::parse(configFile);

        if (!fileJson.contains(Application::getInstance().data().name))
        {
            PLOGE << fmt::format("Configuration file does not contain application section");
            break;
        }

        mConfig = fileJson[Application::getInstance().data().name];

        configFile.close();

        break;
    }
}


