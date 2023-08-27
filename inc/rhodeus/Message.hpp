#ifndef INCLUDED_RHODEUS__MESSAGE_HPP
#define INCLUDED_RHODEUS__MESSAGE_HPP

#include <vector>
#include <string>

namespace Rhodeus
{
    class Message
    {
        const static uint16_t Sync = 0x5A5A;
        const static uint16_t Version = 0x0001;

        typedef struct __attribute__((packed)) _Header
        {
            uint16_t sync;
            uint16_t version;
            uint16_t size;
            uint16_t flags;
        } Header;
    public:
        Message(const void *data = nullptr, size_t size = 0);
        virtual ~Message() = default;

        Message& clear();

        Message& append(const uint8_t *data, size_t size);
        Message& append(const std::vector<uint8_t> &data);
        Message& append(const std::string &data);

        template <typename T>
        Message& append(const T &data)
        {
            append(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
            return *this;
        }

        const void *data() const;
        size_t size() const;

        int32_t serialize(std::vector<uint8_t> &buffer) const;
        int32_t parse(const std::vector<uint8_t> &buffer);

        Message& build();
        Message& build(uint16_t sync, uint16_t version, uint16_t flags);

        std::string toString() const;

    protected:

    private:
        Header m_header;
        std::vector<uint8_t> m_data;
    };
}

#endif // INCLUDED_RHODEUS__MESSAGE_HPP
