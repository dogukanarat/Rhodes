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


#if __APPLE__
#define DATA_FOLDER_PATH       "/opt/rhodeus/"
#endif /* __APPLE__ */

#ifdef __linux__
#define DATA_FOLDER_PATH       "/opt/rhodeus/"
#endif /* __linux__ */

#ifdef _WIN32
#define DATA_FOLDER_PATH       "C:\\Rhodeus\\"
#endif /* _WIN32 */

Application& Application::getInstance()
{
    static Application instance;
    return instance;
}

int32_t Application::run(int argc, char** argv)
{
    int32_t status = 0;

    for (;;)
    {
        Application::getInstance().registerSignalHandlers();

        status = Application::getInstance().initializeFirstStage();
        if (0 != status)
        {
            CONSOLE_LOG(fmt::format("Failed to initialize first stage!"));
            break;
        }

        status = Application::getInstance().initializeSecondStage();
        if (0 != status)
        {
            CONSOLE_LOG(fmt::format("Failed to initialize second stage!"));
            break;
        }

        ComponentManager::getInstance().initializeComponents();

        CONSOLE_LOG(fmt::format("Press Ctrl+C to exit..."));

        linenoisecli::cli::getInstance().setPrompt(Data.prompt);
        linenoisecli::cli::getInstance().setHistoryFile(mHistoryFile);
        linenoisecli::cli::getInstance().run(argc, argv);

        while ((false == linenoisecli::cli::getInstance().isExitRequested()) && (false == mIsExitRequested))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        CONSOLE_LOG(fmt::format("Exiting safely..."));

        linenoisecli::cli::getInstance().destroy();

        ComponentManager::getInstance().destroyComponents();

        IpcContext::getInstance().clearResources();

        break;
    }

    CONSOLE_LOG(fmt::format("Process status: {}", status));

    return status;
}

int32_t Application::initializeFirstStage()
{
    int32_t status = 0;
    bool isDirCreated = false;
    std::filesystem::path dataFolder = std::filesystem::path(DATA_FOLDER_PATH);

    try
    {
        if (!std::filesystem::exists(dataFolder))
        {
            isDirCreated = std::filesystem::create_directory(dataFolder);
            if (!isDirCreated)
            {
                throw std::runtime_error(fmt::format("Failed to create data folder: {}", dataFolder.string()));
            }
        }

        mDataFolder = std::filesystem::canonical(dataFolder).string();
        mConfigurationFile = std::filesystem::path(mDataFolder).append(Data.configFile).string();
        mHistoryFile = std::filesystem::path(mDataFolder).append(Data.historyFile).string();

        CONSOLE_LOG(fmt::format("Data folder: {}", mDataFolder));
        CONSOLE_LOG(fmt::format("Configuration file: {}", mConfigurationFile));
        CONSOLE_LOG(fmt::format("History file: {}", mHistoryFile));

    }
    catch(const std::exception& e)
    {
        CONSOLE_ERR(fmt::format("Exception: {}", e.what()));
        status = -1;
    }

    return status;
}

int32_t Application::initializeSecondStage()
{
    int32_t status = 0;

    try
    {
        Configuration::getInstance().initialize(mConfigurationFile);

        status = Application::getInstance().initializeLoggers();
        if (0 != status)
        {
            throw std::runtime_error(fmt::format("Failed to initialize loggers!"));
        }
        status = Application::getInstance().initializeCommands();
        if (0 != status)
        {
            throw std::runtime_error(fmt::format("Failed to initialize commands!"));
        }
    }
    catch(const std::exception& e)
    {
        CONSOLE_ERR(fmt::format("Exception: {}", e.what()));
        status = -1;
    }

    return status;
}

int32_t Application::initializeLoggers()
{
    int32_t status = 0;
    bool isLogFolderCreated = false;
    plog::Severity logLevel = plog::debug;
    std::filesystem::path logFolder = std::filesystem::path(mDataFolder).append("logs");
    std::filesystem::path logFile = std::filesystem::path(mDataFolder).append("logs").append(Data.logFile);
    static plog::ColorConsoleAppender<CustomLogFormatterConsole> consoleAppender;
    static plog::RollingFileAppender<CustomLogFormatterFile> fileAppender(logFile.string().c_str(), Data.logFileMaxSize, Data.logFileMaxCount);


    for (;;)
    {
        isLogFolderCreated = std::filesystem::exists(logFile);
        if (!isLogFolderCreated)
        {
            isLogFolderCreated = std::filesystem::create_directory(logFolder);
            if (!isLogFolderCreated)
            {
                PLOGE << fmt::format("Failed to create log folder: {}", logFolder.string());
                status = -1;
                break;
            }
        }

        plog::init(logLevel, &consoleAppender).addAppender(&fileAppender);

        break;
    }

    return status;
}

int32_t Application::initializeCommands()
{
    int32_t status = 0;

    auto callbackVersion = [](linenoisecli::cli::ArgumentMap& args) -> int32_t
    {
        int32_t status = 0;
        UNUSED(args);
        std::cout << fmt::format("Version: {}", Data.version) << std::endl;
        return status;
    };
    auto callbackLogLevel = [](linenoisecli::cli::ArgumentMap& args) -> int32_t
    {
        int32_t status = 0;
        std::string argLogLevel;
        std::string currentLogLevel;
        bool isSet = false;
        bool isFound = false;
        std::map<plog::Severity, std::string> logLevelMap =
        {
            { plog::none, "none" },
            { plog::fatal, "fatal" },
            { plog::error, "error" },
            { plog::warning, "warning" },
            { plog::info, "info" },
            { plog::debug, "debug" },
            { plog::verbose, "verbose" },
        };

        for (;;)
        {
            isSet = args.contains("logLevel");
            if (isSet)
            {
                // set plog severity
                argLogLevel = args["logLevel"];
                for (auto& [severity, level] : logLevelMap)
                {
                    if (level == argLogLevel)
                    {
                        plog::get()->setMaxSeverity(severity);
                        isFound = true;
                        break;
                    }
                }
                if (!isFound)
                {
                    CONSOLE_ERR(std::format("Invalid log level: {}", argLogLevel));
                    CONSOLE_ERR(std::format("Valid log levels: none, fatal, error, warning, info, debug, verbose"));
                    status = -1;
                    break;
                }
            }
            currentLogLevel = logLevelMap[plog::get()->getMaxSeverity()];
            CONSOLE_LOG(std::format("Current log level: {}", currentLogLevel));
            break;
        }

        return status;
    };

    for (;;)
    {
        linenoisecli::cli::getInstance().registerCommand("version", callbackVersion);
        linenoisecli::cli::getInstance().registerCommand("logLevel [<logLevel>]", callbackLogLevel);
        break;
    }

    return status;

}

void Application::signalHandler(int32_t signum)
{
    std::string signalName = "UNKNOWN";
    static std::map<int32_t, std::string> signalMap
    {
        { SIGINT, "SIGINT" },
        { SIGABRT, "SIGABRT" },
        { SIGTERM, "SIGTERM" },
    };

    if (signalMap.find(signum) != signalMap.end())
    {
        signalName = signalMap[signum];
    }

    CONSOLE_ERR(fmt::format("Interrupt signal ({}) received", signalName));

    Application::getInstance().mIsExitRequested = true;
}

void Application::registerSignalHandlers()
{
    // Register signal and signal handler
    signal(SIGINT, Application::signalHandler);
    signal(SIGABRT, Application::signalHandler);
    signal(SIGTERM, Application::signalHandler);
}
