#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../include/rhodeus/Message.hpp"

TEST_CASE("message can be serialize with headers and data consecutively", "[Message]")
{
    std::vector<uint8_t> buffer;
    Rhodeus::Message message;

    buffer.clear();

    SECTION("empty message serialize returns header data only")
    {
        REQUIRE(message.serialize(buffer) == 0);
        REQUIRE(message.size() == 8);
        REQUIRE(buffer.size() == 8);

        REQUIRE(buffer[0] == 0x5A);
        REQUIRE(buffer[1] == 0x5A);
        REQUIRE(buffer[2] == 0x00);
        REQUIRE(buffer[3] == 0x01);
        REQUIRE(buffer[4] == 0x00);
        REQUIRE(buffer[5] == 0x00);
        REQUIRE(buffer[6] == 0x00);
        REQUIRE(buffer[7] == 0x00);
    }

    SECTION("message with data")
    {
        message.append("Hello World");
        REQUIRE(message.size() == 8 + 11);
        REQUIRE(message.serialize(buffer) == 0);
        REQUIRE(buffer.size() == 11);
        REQUIRE(buffer[0] == 'H');
        REQUIRE(buffer[10] == 'd');
    }
}
