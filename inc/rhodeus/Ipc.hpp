#ifndef INCLUDED_RHODEUS_IPC_HPP
#define INCLUDED_RHODEUS_IPC_HPP

#include <string>
#include <thread>
#include <zmq.hpp>
#include "rhodeus/AbstractComponent.hpp"

namespace Rhodeus
{
    class IpcContext
    {
    public:
        static IpcContext& getInstance()
        {
            static IpcContext instance;
            return instance;
        }

        void clearResources();

    protected:
        IpcContext() {}
        IpcContext(IpcContext const&) = delete;
        void operator=(IpcContext const&) = delete;
        ~IpcContext() {}
    private:
    };

    class IpcEndPoint : public AbstractComponent
    {
        friend class IpcContext;
    public:

    protected:
        IpcEndPoint(std::string name) : AbstractComponent(name) {}

    protected:
        uint32_t m_id;
        std::string m_name;
        static zmq::context_t context;
    };

    class IpcServer : public IpcEndPoint
    {
    public:
        static IpcServer& getInstance()
        {
            static IpcServer instance;
            return instance;
        }

        IpcServer& setName(std::string name) { m_name = name; return *this; }
        IpcServer& setId(uint32_t id) { m_id = id; return *this; }

        int32_t initialize() override;
        int32_t finalize() override;

    protected:
        IpcServer() : IpcEndPoint("IpcServer") {}
        IpcServer(IpcServer const&) = delete;
        void operator=(IpcServer const&) = delete;
        ~IpcServer() {}

        static void task(IpcServer *server);

    private:
        std::thread *m_thread;
        bool m_isExitRequested;
    };

    class IpcClient : public IpcEndPoint
    {
    public:
        static IpcClient& getInstance()
        {
            static IpcClient instance;
            return instance;
        }

        IpcClient& setName(std::string name) { m_name = name; return *this; }
        IpcClient& setId(uint32_t id) { m_id = id; return *this; }

        int32_t initialize() override;
        int32_t finalize() override;

    protected:
        IpcClient() : IpcEndPoint("IpcClient") {}
        IpcClient(IpcClient const&) = delete;
        void operator=(IpcClient const&) = delete;
        ~IpcClient() {}

        static void task(IpcClient *client);

    private:
        std::thread *m_thread;
        bool m_isExitRequested;

    };
}

#endif // INCLUDED_RHODEUS_IPC_HPP
