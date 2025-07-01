//
// Created by Hayden Chalin on 6/30/25.
//
#include <gtest/gtest.h>

// Basic test to verify addition
TEST(BasicTest, AdditionWorks) {
    int a = 2;
    int b = 3;
    EXPECT_EQ(a + b, 5); // Check if 2 + 3 equals 5
}

// Another basic test to verify subtraction
TEST(BasicTest, SubtractionWorks) {
    int a = 10;
    int b = 4;
    EXPECT_EQ(a - b, 6); // Check if 10 - 4 equals 6
}

TEST(BasicTest, MultiplicationWorks) {
    int a = 10;
    int b = 4;
    EXPECT_EQ(a * b, 40);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}