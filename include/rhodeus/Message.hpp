#ifndef INCLUDED_RHODEUS__MESSAGE_HPP
#define INCLUDED_RHODEUS__MESSAGE_HPP

#include <vector>
#include <string>
#include "rhodeus/Common.hpp"

namespace Rhodeus
{
    class Message
    {
        const static uint16_t Sync = 0x5A5A;
        const static uint16_t Version = 0x0001;

        typedef struct PACKED _Header
        {
            /** @note sync word  to classify the raw data */
            uint16_t sync;
            /** @note version of the message */
            uint16_t version;
            /** @note size of the message frame (header + data) */
            uint16_t size;
            /** @note bitwise flags */
            uint16_t flags;
        } Header;

    public:
        /**
         * @brief Construct a new Message object
         *
         * @param data
         * @param size
         */
        Message(const void *data = nullptr, size_t size = 0);

        /**
         * @brief Destroy the Message object
         *
         */
        virtual ~Message() = default;

        /**
         * @brief Clear the message
         *
         * @return Message&
         */
        Message& clear();

        /**
         * @brief Append data to the message
         *
         * @param data
         * @param size
         * @return Message&
         */
        Message& append(const uint8_t *data, size_t size);
        Message& append(const std::vector<uint8_t> &data);
        Message& append(const std::string &data);

        template <typename T>
        Message& append(const T &data)
        {
            append(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
            return *this;
        }

        /**
         * @brief Get the data
         *
         * @return const void*
         */
        const void *data() const;

        /**
         * @brief Get the size of the frame (header + data)
         *
         * @return size_t
         */
        size_t size() const;

        /**
         * @brief Serialize the message frame
         *
         * @note returned error codes are: 0: success -1: error
         *
         * @param buffer
         * @return int32_t
         */
        int32_t serialize(std::vector<uint8_t> &buffer);

        /**
         * @brief Parse the message frame
         *
         * @note returned error codes are: 0: success -1: error
         *
         * @param buffer
         * @return int32_t
         */
        int32_t parse(const std::vector<uint8_t> &buffer);

        /**
         * @brief Build the message
         *
         * @return Message&
         */
        Message& build();

        /**
         * @brief Build the message
         *
         * @param sync
         * @param version
         * @param flags
         * @return Message&
         */
        Message& build(uint16_t sync, uint16_t version, uint16_t flags);

        /**
         * @brief Get the header
         *
         * @return std::string
         */
        std::string toString() const;

        /**
         * @brief Get the last error code
         *
         * @return int32_t
         */
        int32_t lastErrorCode() const { return _lastErrorCode; }

    protected:
        // no member function

    private:
        int32_t _lastErrorCode;
        Header _header;
        std::vector<uint8_t> _data;
    };
}

#endif // INCLUDED_RHODEUS__MESSAGE_HPP
