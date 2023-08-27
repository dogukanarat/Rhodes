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
}

#endif // INCLUDED_RHODEUS_IPC_HPP
