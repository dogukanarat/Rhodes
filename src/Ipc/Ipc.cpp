#include "../../include/rhodeus/Ipc/Ipc.hpp"
#include "../../include/rhodeus/Message.hpp"

using namespace Rhodeus;

zmq::context_t IpcEndPoint::Context(1);

void IpcContext::clearResources()
{
    PLOGD << fmt::format("Clearing resources...");

    IpcEndPoint::Context.shutdown();
    IpcEndPoint::Context.close();
}


