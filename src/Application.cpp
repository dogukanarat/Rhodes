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

Application& Application::getInstance()
{
    static Application instance;
    return instance;
}

int32_t Application::run(int argc, char** argv)
{
    int32_t status = 0;

    mDataFolder = std::filesystem::current_path().string();

    Application::getInstance().registerSignalHandlers();
    Application::getInstance().initializeDataFolder();
    Application::getInstance().initializeLoggers();

    Configuration::getInstance().initialize(
        std::filesystem::path(mDataFolder).append(Data.configFile).string()
    );

    std::cout << fmt::format("Welcome to {} v{}", Data.name, Data.version) << std::endl;
    std::cout << fmt::format("Data folder: {}", mDataFolder) << std::endl;

    ComponentManager::getInstance().initializeComponents();

    std::cout << fmt::format("Press Ctrl+C to exit...") << std::endl;

    linenoisecli::cli::getInstance().setPrompt(Data.prompt);
    linenoisecli::cli::getInstance().setHistoryFile(
        std::filesystem::path(mDataFolder).append(Data.historyFile).string()
    );
    linenoisecli::cli::getInstance().run(argc, argv);

    while ((false == linenoisecli::cli::getInstance().isExitRequested()) && (false == mIsExitRequested))
    {
        // Do nothing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    linenoisecli::cli::getInstance().destroy();

    std::cout << fmt::format("Exiting safely...") << std::endl;

    ComponentManager::getInstance().destroyComponents();

    IpcContext::getInstance().clearResources();

    std::cout << fmt::format("Process status: {}", status) << std::endl;

    return status;
}

void Application::initializeDataFolder()
{
    bool isDirCreated = false;
    std::filesystem::path dataFolder = std::filesystem::path(DATA_FOLDER_PATH);

    try
    {
        if (!std::filesystem::exists(dataFolder))
        {
            isDirCreated = std::filesystem::create_directory(dataFolder);

            if (!isDirCreated)
            {
                throw std::runtime_error(
                    fmt::format("Failed to create data folder: {}",
                        dataFolder.string()
                    )
                );
            }
        }

        mDataFolder = std::filesystem::canonical(dataFolder).string();
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
    std::filesystem::create_directory(std::filesystem::path(mDataFolder).append("logs"));
    std::string logFile = std::filesystem::path(mDataFolder)
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
