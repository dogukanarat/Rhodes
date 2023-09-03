#include <chrono>
#include <csignal>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <signal.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "rhodeus/Application.hpp"
#include "rhodeus/Ipc/Ipc.hpp"
#include "rhodeus/Ipc/IpcClient.hpp"
#include "rhodeus/Ipc/IpcServer.hpp"
#include "rhodeus/Configuration/Configuration.hpp"

using namespace Rhodeus;

#define DATA_FOLDER_PATH       "/opt/rhodeus/"

std::string Application::DataFolder = fs::current_path().string();

std::vector<AbstractComponent*> Application::m_components{};

int32_t Application::run(int32_t argc, char** argv)
{
    int32_t status = 0;

    getInstance().registerSignalHandlers();
    getInstance().initializeDataFolder();
    getInstance().initializeLoggers();
    Configuration::getInstance().initialize(fs::path(DataFolder).append(Data.configFile).string());

    PLOGI << fmt::format("Welcome to {} v{}", Data.name, Data.version);
    PLOGI << fmt::format("Data folder: {}", DataFolder);

    int componentCount = ((long)&InstalledComponentAddrStop - (long)&InstalledComponentAddrStart) / sizeof(void*);

    PLOGI << fmt::format("Found {} components", componentCount);

    for (uint32_t componentIndex = 0; componentIndex < componentCount; componentIndex++)
    {
        AbstractComponent* component = (AbstractComponent*)*((long*)&InstalledComponentAddrStart + componentIndex);
        PLOGI << fmt::format("Component {} found", component->name());
        m_components.push_back(component);
    }

    int componentInitializedCount = ((long)&InstalledComponentInitializerStop - (long)&InstalledComponentInitializerStart) / sizeof(void*);

    PLOGI << fmt::format("Found {} component initializers", componentInitializedCount);

    for (uint32_t componentIndex = 0; componentIndex < componentInitializedCount; componentIndex++)
    {
        void(*initializer)(void) = (void(*)(void))*((long*)&InstalledComponentInitializerStart + componentIndex);
        initializer();
    }

    for (;;)
    {
        for (auto component : m_components)
        {
            status = component->initialize();
            if (status != 0)
            {
                PLOGE << fmt::format("Component {} initialization failed with status {}", component->name(), status);
            }
        }

        PLOGI << fmt::format("Press Ctrl+C to exit...");

        for (;;)
        {
            if (m_isExitRequested) { break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        PLOGI << fmt::format("Exiting safely...");

        for (auto component : m_components)
        {
            status = component->finalize();
            if (status != 0)
            {
                PLOGE << fmt::format("Component {} finalization failed with status {}", component->name(), status);
            }
        }

        break;
    }

    IpcContext::getInstance().clearResources();

    PLOGI << fmt::format("Process status: {}", status);
    return status;
}

void Application::initializeDataFolder()
{
    try
    {
        fs::path dataFolder(DATA_FOLDER_PATH);

        if (!fs::exists(dataFolder))
        {
            fs::create_directory(dataFolder);
        }

        DataFolder = fs::canonical(dataFolder).string();
    }
    catch(const std::exception& e)
    {
        // no logging here, because logging is not initialized yet
    }
}

void Application::initializeLoggers()
{
    fs::create_directory(fs::path(DataFolder).append("logs"));
    std::string logFile = fs::path(DataFolder).append("logs").append(Data.logFile).string();

    static plog::ColorConsoleAppender<Rhodeus::CustomLogFormatter> consoleAppender;

    static plog::RollingFileAppender<Rhodeus::CustomLogFormatter> fileAppender(
            logFile.c_str(),
            Data.logFileMaxSize,
            Data.logFileMaxCount);

    plog::init(plog::debug, &consoleAppender).addAppender(&fileAppender);

    PLOGV << fmt::format("Log file: {}", logFile);
}

void Application::signalHandler(int32_t signum)
{
    std::string signalName = "UNKNOWN";
    std::map<int32_t, std::string> signalMap
    {
        { SIGINT, "SIGINT" },
        { SIGABRT, "SIGABRT" },
        { SIGTERM, "SIGTERM" },
    };

    if (signalMap.find(signum) != signalMap.end())
    {
        signalName = signalMap[signum];
    }

    std::cerr << CERR_LOG_RED "Interrupt signal (" << signalName << ") received" CERR_LOG_CLR << std::endl;
    std::cerr << "Application will be terminated safely!" << std::endl;
    std::cerr << std::flush;
    Application::getInstance().m_isExitRequested = true;
}

void sigActionSegFault(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    exit(0);
}

void Application::registerSignalHandlers()
{
    // Register signal and signal handler
    signal(SIGINT, Application::signalHandler);
    signal(SIGABRT, Application::signalHandler);
    signal(SIGTERM, Application::signalHandler);

    // Register segfault handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigActionSegFault;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
}
