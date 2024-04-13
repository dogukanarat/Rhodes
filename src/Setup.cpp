#include "../include/rhodeus/Application.hpp"

using namespace Rhodeus;

const ApplicationData Application::Data =
{
    .name = "Rhodeus",
    .description = "Rhodeus is a template for C++ applications.",
    .version = "0.0.1",
    .logFile = "log.txt",
    .prompt = "Rhodeus> ",
    .historyFile = "history.txt",
    .configFile = "config.json",
    .commandFile = "command.json",
    .logFileMaxSize = 1000000,
    .logFileMaxCount = 3
};

