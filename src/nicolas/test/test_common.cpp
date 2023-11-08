#include "gtest/gtest.h"

class CommonTest : public ::testing::Test {};

TEST_F(CommonTest, TestTest) {
    ASSERT_EQ(1, 1);
}
