#include "../include/rhodeus/Communication.hpp"

using namespace Rhodeus;

#define CONFIG_PARAM_LISTENER_THREAD_COUNT           (2u)

Communication& Communication::getInstance()
{
    static Communication instance;
    return instance;
}

Communication& Communication::setName(const std::string& name)
{
    mName = name;
    return *this;
}

Communication& Communication::setId(uint32_t id)
{
    mId = id;
    return *this;
}

int32_t Communication::initialize()
{
    int32_t status = 0;

    for (uint32_t i = 0; i < CONFIG_PARAM_LISTENER_THREAD_COUNT; i++)
    {
        mListenerThreads.push_back(std::thread(&Communication::task, this));
    }

    PLOGD << "Communication initialized";

    return status;
}
