#include "rhodeus/Ipc.hpp"

using namespace Rhodeus;

zmq::context_t IpcEndPoint::context(1);

void IpcContext::clearResources()
{
    PLOGD << fmt::format("Clearing resources...");

    IpcEndPoint::context.shutdown();
}

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

    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    uint8_t targetId = 0;

    // set timeout
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
                zmq::message_t message;
                zmq::recv_result_t result;

                result = socket.recv(message);

                if (!result.has_value())
                {
                    break;
                }

                std::string messageContent(static_cast<char*>(message.data()), message.size());

                PLOGI << fmt::format("Message received! Message: {}", messageContent);

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

int32_t IpcClient::initialize()
{
    int32_t status = 0;

    m_thread = new std::thread(&IpcClient::task, this);

    PLOGD << "IpcClient initialized";

    emit("initialized");

    return status;
}

int32_t IpcClient::finalize()
{
    int32_t status = 0;

    if (m_thread != nullptr)
    {
        m_isExitRequested = true;
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }

    PLOGD << "IpcClient finalized";

    emit("finalized");

    return status;
}

void IpcClient::task(IpcClient *client)
{
    PLOGD << "IpcClient task started";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    zmq::socket_t socket(context, ZMQ_REQ);
    bool isConnected = false;

    socket.set(zmq::sockopt::rcvtimeo, 2000);
    socket.set(zmq::sockopt::sndtimeo, 2000);
    socket.set(zmq::sockopt::connect_timeout, 2000);

    while (true)
    {
        PLOGD << "IpcClient task running";

        try
        {
            for (;;)
            {
                if (isConnected == false)
                {
                    socket.connect("tcp://localhost:5555");
                    isConnected = true;
                    PLOGI << fmt::format("Connected to server!");
                }
                if (ZMQ_NULLPTR == socket.handle())
                {
                    PLOGE << fmt::format("Not connected to server!");
                    break;
                }

                PLOGI << fmt::format("Sending message...");

                std::string messageContent = fmt::format("Actual Message!");
                zmq::message_t message = zmq::message_t(messageContent.c_str(), messageContent.size());

                socket.send(message, zmq::send_flags::none);

                PLOGI << fmt::format("Message sent! Message: {}", messageContent);

                zmq::message_t reply;
                zmq::recv_result_t result;

                result = socket.recv(reply);

                if (!result.has_value())
                {
                    PLOGE << fmt::format("No reply received!");
                    break;
                }

                std::string replyStr(static_cast<char*>(reply.data()), reply.size());

                PLOGI << fmt::format("Reply received! Reply: {}", replyStr);

                break;
            }
        }
        catch (zmq::error_t &e)
        {
            PLOGE << fmt::format("Exception: {}", e.what());
            PLOGE << fmt::format("Error code: {}", zmq_errno());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (client->m_isExitRequested) { break; }
    }

    socket.close();

    PLOGD << "IpcClient task finished";
}
