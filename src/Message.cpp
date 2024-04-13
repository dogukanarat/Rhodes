#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <../include/rhodeus/Message.hpp>

using namespace Rhodeus;

Message::Message(const void *data, size_t size)
    : _lastErrorCode{0}
    , _header{}
    , _data{}
{
    _lastErrorCode = parse(
        std::vector<uint8_t>(
            reinterpret_cast<const uint8_t*>(data),
            reinterpret_cast<const uint8_t*>(data) + size
            )
        );
}

Message& Message::build()
{
    build(Sync, Version, 0);
    return *this;
}

Message& Message::build(uint16_t sync, uint16_t version, uint16_t flags)
{
    _header.sync = sync;
    _header.version = version;
    _header.flags = flags;
    _header.size = _data.size() + sizeof(_header);
    return *this;
}

Message& Message::append(const uint8_t *data, size_t size)
{
    _data.insert(_data.end(), data, data + size);
    return *this;
}

Message& Message::append(const std::vector<uint8_t> &data)
{
    _data.insert(_data.end(), data.begin(), data.end());
    return *this;
}

Message& Message::append(const std::string &data)
{
    _data.insert(_data.end(), data.begin(), data.end());
    return *this;
}

const void *Message::data() const
{
    return _data.data();
}

size_t Message::size() const
{
    return _data.size() + sizeof(_header);
}

int32_t Message::serialize(std::vector<uint8_t> &buffer)
{
    int32_t status = 0;

    for (;;)
    {
        /**
         * @note This is a very important step, we need to build the header before
         * in case the user has modified the data and the size has changed.
         */
        build();

        buffer.clear();

        buffer.insert(
            buffer.end(),
            reinterpret_cast<const uint8_t*>(&_header),
            reinterpret_cast<const uint8_t*>(&_header) + sizeof(_header)
        );

        buffer.insert(buffer.end(), _data.begin(), _data.end());

        break;
    }

    return status;
}

int32_t Message::parse(const std::vector<uint8_t> &buffer)
{
    int32_t status = 0;

    for (;;)
    {
        if (buffer.size() < sizeof(_header))
        {
            status = -1;
            break;
        }

        _header = *reinterpret_cast<const Header*>(buffer.data());

        if (_header.sync != Sync)
        {
            status = -2;
            break;
        }

        if (buffer.size() < sizeof(_header) + _header.size)
        {
            status = -3;
            break;
        }

        _data.clear();
        _data.insert(
            _data.end(),
            buffer.begin() + sizeof(_header),
            buffer.begin() + sizeof(_header) + _header.size
        );

        break;
    }

    return status;
}

std::string Message::toString() const
{
    nlohmann::json json;
    json["sync"] = std::to_string(_header.sync);
    json["version"] = std::to_string(_header.version);
    json["size"] = std::to_string(_header.size);
    json["flags"] = std::to_string(_header.flags);
    json["data"] = std::string(_data.begin(), _data.end());

    return json.dump();
}
