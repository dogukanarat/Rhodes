#ifndef INCLUDED_RHODEUS_CONFIGURATION_HPP
#define INCLUDED_RHODEUS_CONFIGURATION_HPP

#include <string>

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

        int32_t initialize(const std::string& configFilePath);

        template <typename T>
        T get(const std::string& key, T defaultValue)
        {
            return defaultValue;
        }

        template <typename T>
        int32_t set(const std::string& key, T value)
        {
            return 0;
        }

        protected:
            Configuration() {}
            Configuration(Configuration const&) = delete;
            void operator=(Configuration const&) = delete;

        private:
    };
}

#endif
