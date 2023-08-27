#include "rhodeus/Message.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using namespace Rhodeus;

Message::Message(const void *data, size_t size)
{
    parse(std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + size));
}

Message& Message::build()
{
   build(Sync, Version, 0);
   return *this;
}

Message& Message::build(uint16_t sync, uint16_t version, uint16_t flags)
{
    m_header.sync = sync;
    m_header.version = version;
    m_header.flags = flags;
    m_header.size = m_data.size();
    return *this;
}

Message& Message::append(const uint8_t *data, size_t size)
{
    m_data.insert(m_data.end(), data, data + size);
    return *this;
}

Message& Message::append(const std::vector<uint8_t> &data)
{
    m_data.insert(m_data.end(), data.begin(), data.end());
    return *this;
}

Message& Message::append(const std::string &data)
{
    m_data.insert(m_data.end(), data.begin(), data.end());
    return *this;
}

const void *Message::data() const
{
    return m_data.data();
}

size_t Message::size() const
{
    return m_data.size();
}

int32_t Message::serialize(std::vector<uint8_t> &buffer) const
{
    int32_t status = 0;

    for (;;)
    {
        buffer.clear();

        buffer.insert(
            buffer.end(),
            reinterpret_cast<const uint8_t*>(&m_header),
            reinterpret_cast<const uint8_t*>(&m_header) + sizeof(m_header));

        buffer.insert(buffer.end(), m_data.begin(), m_data.end());

        break;
    }

    return status;
}

int32_t Message::parse(const std::vector<uint8_t> &buffer)
{
    int32_t status = 0;

    for (;;)
    {
        if (buffer.size() < sizeof(m_header))
        {
            status = -1;
            break;
        }

        m_header = *reinterpret_cast<const Header*>(buffer.data());

        if (m_header.sync != 0x55AA)
        {
            status = -2;
            break;
        }

        if (buffer.size() < sizeof(m_header) + m_header.size)
        {
            status = -3;
            break;
        }

        m_data.clear();
        m_data.insert(m_data.end(), buffer.begin() + sizeof(m_header), buffer.begin() + sizeof(m_header) + m_header.size);

        break;
    }

    return status;
}

std::string Message::toString() const
{
    nlohmann::json json;
    json["sync"] = std::to_string(m_header.sync);
    json["version"] = std::to_string(m_header.version);
    json["size"] = std::to_string(m_header.size);
    json["flags"] = std::to_string(m_header.flags);
    json["data"] = std::string(m_data.begin(), m_data.end());

    return json.dump();
}
