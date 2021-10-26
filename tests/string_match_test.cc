/*
 * @Author: your name
 * @Date: 2021-10-20 14:52:29
 * @LastEditTime: 2021-10-25 11:29:40
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\tests\temp_engine_test.cc
 */

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Test1
TEST(SuiteName, TestName1) {
    int expected = 3;
    int actual = 3.0;
    ASSERT_EQ(expected, actual);
}

TEST(SuiteName, TestName2) {
    int expected = 3;
    int actual = 3;
    ASSERT_EQ(expected, actual);
}
