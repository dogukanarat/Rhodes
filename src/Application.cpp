#include <signal.h>
#include <chrono>
#include <csignal>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <fmt/format.h>
#include <fmt/color.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <linenoisecli/linenoisecli.hpp>

#include <../include/rhodeus/Application.hpp>
#include <../include/rhodeus/Ipc/Ipc.hpp>
#include <../include/rhodeus/Ipc/IpcClient.hpp>
#include <../include/rhodeus/Ipc/IpcServer.hpp>
#include <../include/rhodeus/Configuration/Configuration.hpp>

using namespace Rhodeus;

#define DATA_FOLDER_PATH       "/opt/rhodeus/"

std::vector<AbstractComponent*> Application::Components{};

int32_t Application::run(int argc, char** argv)
{
    int32_t status = 0;

    mDataFolder = fs::current_path().string();

    getInstance().registerSignalHandlers();
    getInstance().initializeDataFolder();
    getInstance().initializeLoggers();
    Configuration::getInstance().initialize(
        fs::path(mDataFolder).append(Data.configFile).string()
    );

    PLOGI << fmt::format("Welcome to {} v{}", Data.name, Data.version);
    PLOGI << fmt::format("Data folder: {}", mDataFolder);

    for (;;)
    {
        for (auto component : Components)
        {
            status = component->initialize();
            if (status != 0)
            {
                PLOGE << fmt::format(
                    "Component {} initialization failed with status {}",
                     component->name(),
                    status
                );
            }
        }

        PLOGI << fmt::format("Press Ctrl+C to exit...");

        linenoisecli::cli::getInstance().run(argc, argv);

        for (;;)
        {
            if (mIsExitRequested || linenoisecli::cli::getInstance().isExitRequested()) { break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        PLOGI << fmt::format("Exiting safely...");

        linenoisecli::cli::getInstance().destroy();

        for (auto component : Components)
        {
            status = component->finalize();
            if (status != 0)
            {
                PLOGE << fmt::format(
                    "Component {} finalization failed with status {}",
                     component->name(),
                     status
                );
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
    bool isDirCreated = false;
    fs::path dataFolder(DATA_FOLDER_PATH);

    try
    {
        if (!fs::exists(dataFolder))
        {
            isDirCreated = fs::create_directory(dataFolder);

            if (!isDirCreated)
            {
                throw std::runtime_error(
                    fmt::format("Failed to create data folder: {}",
                        dataFolder.string()
                    )
                );
            }
        }

        mDataFolder = fs::canonical(dataFolder).string();
    }
    catch(const std::exception& e)
    {
        std::cerr
            << CERR_LOG_RED "Exception: "
            << e.what()
            << CERR_LOG_CLR
            << std::endl;
    }
}

void Application::initializeLoggers()
{
    fs::create_directory(fs::path(mDataFolder).append("logs"));
    std::string logFile = fs::path(mDataFolder)
        .append("logs")
        .append(Data.logFile)
        .string();

    static plog::ColorConsoleAppender<CustomLogFormatter> consoleAppender;

    static plog::RollingFileAppender<CustomLogFormatter> fileAppender(
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

    std::cerr << "\r";
    std::cerr
        << CERR_LOG_RED "Interrupt signal ("
        << signalName
        << ") received" CERR_LOG_CLR
        << std::endl;
    std::cerr << "Application will be terminated safely!" << std::endl;
    std::cerr << std::flush;
    Application::getInstance().mIsExitRequested = true;
}

void sigActionSegFault(int signal, siginfo_t *si, void *arg)
{
    UNUSED(signal);
    UNUSED(arg);
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
