#include <gtest/gtest.h>
#include "rhodeus/Message.hpp"

class Messages : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Messages, Message)
{
    EXPECT_EQ(1 + 1, 2);
}
