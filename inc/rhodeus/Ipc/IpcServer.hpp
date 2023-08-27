#ifndef INCLUDED_RHODEUS_IPC_SERVER_HPP
#define INCLUDED_RHODEUS_IPC_SERVER_HPP

#include "rhodeus/Ipc/Ipc.hpp"

namespace Rhodeus
{
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
}

#endif // INCLUDED_RHODEUS_IPC_SERVER_HPP
