#ifndef INCLUDED_RHODEUS_CONFIGURATION_HPP
#define INCLUDED_RHODEUS_CONFIGURATION_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "rhodeus/Common.hpp"

namespace Rhodeus
{
    class Configuration
    {

        public:
            static Configuration& getInstance()
            {
                static Configuration instance;
                return instance;
            }

        Configuration& initialize(const std::string& configFilePath);

        template <typename T>
        T get(const std::string& key, T defaultValue)
        {
            T value = defaultValue;

            if (mConfig.contains(key))
            {
                value = mConfig[key].get<T>();
            }

            return value;
        }

        template <typename T>
        int32_t set(const std::string& key, T value)
        {
            mConfig[key] = value;

            return 0;
        }

        protected:
            Configuration() {}
            Configuration(Configuration const&) = delete;
            void operator=(Configuration const&) = delete;

            void createDefaultConfigFile(const std::string& configFilePath);
            void loadConfigFile(const std::string& configFilePath);

        private:
            json::json mConfig;

    };
}

#endif
