#include <gtest/gtest.h>

// Пример простой функции
int add(int a, int b) {
    return a + b;
}

// Тест для функции add
TEST(AdditionTests, HandlesPositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
    EXPECT_EQ(add(10, 20), 50);
}

TEST(AdditionTests, HandlesNegativeNumbers) {
    EXPECT_EQ(add(-1, -1), -2);
    EXPECT_EQ(add(-5, 5), -5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
