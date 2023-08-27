#include "rhodeus/Ipc/Ipc.hpp"
#include "rhodeus/Message.hpp"

using namespace Rhodeus;

zmq::context_t IpcEndPoint::context(1);

void IpcContext::clearResources()
{
    PLOGD << fmt::format("Clearing resources...");

    IpcEndPoint::context.shutdown();
    IpcEndPoint::context.close();
}


