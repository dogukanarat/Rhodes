#ifndef INCLUDED_RHODEUS_IPC_CLIENT_HPP
#define INCLUDED_RHODEUS_IPC_CLIENT_HPP

#include "rhodeus/Ipc/Ipc.hpp"

namespace Rhodeus
{
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

#endif // INCLUDED_RHODEUS_IPC_CLIENT_HPP
