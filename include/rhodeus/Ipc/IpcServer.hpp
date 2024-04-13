#ifndef INCLUDED_RHODEUS_IPC_SERVER_HPP
#define INCLUDED_RHODEUS_IPC_SERVER_HPP

#include "rhodeus/Ipc/Ipc.hpp"
#include "rhodeus/Message.hpp"

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

        IpcServer& setName(const std::string& name) { mEndPointName = name; return *this; }
        IpcServer& setId(uint32_t id) { mEndPointId = id; return *this; }

        int32_t initialize() override;
        int32_t finalize() override;

    protected:
        IpcServer() : IpcEndPoint("IpcServer")
            , mThread{nullptr}
            , mIsExitRequested{false}
        {}
        IpcServer(IpcServer const&) = delete;
        void operator=(IpcServer const&) = delete;
        ~IpcServer() {}

        void onMessage(const Message& message);

        static void task(IpcServer *server);

    private:
        std::thread* mThread;
        bool mIsExitRequested;
    };
}

#endif // INCLUDED_RHODEUS_IPC_SERVER_HPP
