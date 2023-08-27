#ifndef INCLUDED_RHODEUS_DEFAULT_CONFIGURATION_HPP
#define INCLUDED_RHODEUS_DEFAULT_CONFIGURATION_HPP

#include "rhodeus/Common.hpp"
#include "rhodeus/Application.hpp"

namespace Rhodeus
{
    static json::json defaultConfiguration()
    {
        json::json config;

        config[Application::getInstance().data().name]["description"] = "Rhodeus is a template for C++ applications.";
        config[Application::getInstance().data().name]["version"] = "0.0.1";

        return config;
    }
}

#endif // INCLUDED_RHODEUS_DEFAULT_CONFIGURATION_HPP
