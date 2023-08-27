#include <iostream>
#include "rhodeus/Ipc/IpcServer.hpp"
#include "rhodeus/Message.hpp"

using namespace Rhodeus;

int32_t IpcServer::initialize()
{
    int32_t status = 0;

    m_thread = new std::thread(&IpcServer::task, this);

    PLOGD << "IpcServer initialized";

    emit("initialized");

    return status;
}

int32_t IpcServer::finalize()
{
    int32_t status = 0;

    if (m_thread != nullptr)
    {
        m_isExitRequested = true;
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }

    PLOGD << "IpcServer finalized";

    emit("finalized");

    return status;
}

void IpcServer::task(IpcServer *server)
{
    PLOGD << "IpcServer task started";

    uint8_t targetId = 0;
    zmq::socket_t socket(context, zmq::socket_type::pair);

    try
    {
        socket.bind("tcp://*:5555");
    }
    catch(const zmq::error_t& e)
    {
        PLOGE << fmt::format("Error occured while binding socket: {}", e.what());
    }

    // set timeouts
    socket.set(zmq::sockopt::rcvtimeo, 2000);
    socket.set(zmq::sockopt::sndtimeo, 2000);
    socket.set(zmq::sockopt::connect_timeout, 2000);

    while (true)
    {
        PLOGD << "IpcServer task running";

        try
        {
            for (;;)
            {
                zmq::message_t rMsg;
                zmq::recv_result_t result;
                result = socket.recv(rMsg);
                if (!result.has_value())
                {
                    break;
                }

                Message message(rMsg.data(), rMsg.size());
                PLOGI << fmt::format("Message received! Message: {}", message.toString());

                std::string replyContent = fmt::format("OK");
                zmq::message_t reply = zmq::message_t(replyContent.c_str(), replyContent.size());
                socket.send(reply, zmq::send_flags::none);
                PLOGI << fmt::format("Reply sent! Reply: {}", replyContent);

                break;
            }
        }
        catch (std::exception &e)
        {
            PLOGE << fmt::format("Exception: {}", e.what());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (server->m_isExitRequested) { break; }
    }

    socket.close();

    PLOGD << "IpcServer task finished";
}

static void setup(void)
{
    IpcServer::getInstance().setId(1);
    std::cout << fmt::format("Setting up IpcServer...") << std::endl;
}

INSTALL_COMPONENT(IpcServer::getInstance());
INSTALL_COMPONENT_INITIALIZER(setup);
