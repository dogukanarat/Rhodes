#ifndef INCLUDED_RHODEUS_COMMUNICATION_HPP
#define INCLUDED_RHODEUS_COMMUNICATION_HPP

#include <vector>
#include <thread>

#include <rhodeus/Common.hpp>

namespace Rhodeus
{
    class Communication
    {
    public:
        typedef struct
        {
            std::queue<uint8_t> rxQueue;
        } ListenerContext;

        static Communication& getInstance();

        Communication& setName(const std::string& name);
        Communication& setId(uint32_t id);

        int32_t initialize();
        int32_t destroy();

    protected:
        Communication() = default;
        Communication(Communication const&) = delete;
        void operator=(Communication const&) = delete;
        ~Communication() {}

        static void task(Communication *client);

    private:
        std::vector<std::thread> mListenerThreads;
        std::vector<ListenerContext> mListenerContexts;
        bool mIsExitRequested = false;
        std::string mName;
        uint32_t mId = 0;

    };
}
#endif /* INCLUDED_RHODEUS_COMMUNICATION_HPP */
