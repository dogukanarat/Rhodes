#include <iostream>
#include "rhodeus/Ipc/IpcClient.hpp"
#include "rhodeus/Message.hpp"

using namespace Rhodeus;

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

    zmq::socket_t socketTx(context, zmq::socket_type::pair);
    bool isConnected = false;

    // set timeouts
    socketTx.set(zmq::sockopt::rcvtimeo, 2000);
    socketTx.set(zmq::sockopt::sndtimeo, 2000);
    socketTx.set(zmq::sockopt::connect_timeout, 2000);

    while (true)
    {
        PLOGV << "IpcClient task running";

        for (;;)
        {
            try
            {
                if (isConnected == false)
                {
                    socketTx.connect("tcp://localhost:5555");
                    isConnected = true;
                    PLOGI << fmt::format("Connected to server!");
                }
                if (ZMQ_NULLPTR == socketTx.handle())
                {
                    PLOGE << fmt::format("Not connected to server!");
                    break;
                }
            }
            catch(const zmq::error_t& e)
            {
                PLOGE << fmt::format("Error occured while connecting socketTx: {}", e.what());
            }

            if (!isConnected)
            {
                break;
            }

            Message initialMessage;
            nlohmann::json json;
            json["id"] = client->m_id;
            json["name"] = "Rhodeus";
            json["request"] = "initial";
            initialMessage.append(json.dump()).build();

            zmq::message_t message = zmq::message_t(initialMessage.data(), initialMessage.size());

            socketTx.send(message, zmq::send_flags::none);

            PLOGI << fmt::format("Message sent! Message: {}", initialMessage.toString());

            zmq::message_t reply;
            zmq::recv_result_t result;

            result = socketTx.recv(reply);

            if (!result.has_value())
            {
                PLOGE << fmt::format("No reply received!");
                break;
            }

            std::string replyStr(static_cast<char*>(reply.data()), reply.size());

            PLOGI << fmt::format("Reply received! Reply: {}", replyStr);

            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (client->m_isExitRequested) { break; }
    }

    socketTx.close();

    PLOGD << "IpcClient task finished";
}

static void setup(void)
{
    IpcClient::getInstance().setId(0).setName("Launcher");
    std::cout << fmt::format("Setting up IpcClient\n");
}

INSTALL_COMPONENT(IpcClient::getInstance());
INSTALL_COMPONENT_INITIALIZER(setup);
