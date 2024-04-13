#ifndef INCLUDED_RHODEUS_IPC_HPP
#define INCLUDED_RHODEUS_IPC_HPP

#include <string>
#include <thread>

#include <zmq.hpp>

#include <rhodeus/Component.hpp>

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
    protected:
        static zmq::context_t Context;

    protected:
        IpcEndPoint(const std::string& name) : AbstractComponent(name)
            , _endPointId{0}
            , _endPointName{}
        {}

    protected:
        uint32_t _endPointId;
        std::string _endPointName;
    };
}

#endif // INCLUDED_RHODEUS_IPC_HPP
