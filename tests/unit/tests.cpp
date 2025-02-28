#include <climits>   // Для INT_MAX, INT_MIN
#include <gtest/gtest.h> // Фреймворк Google Test

#define MAX_INT_BOUND 2000000000LL // Максимальное целое число
#define MIN_INT_BOUND -2000000000LL // Минимальное целое число

extern "C" { // Объявление функций из C-кода
extern float parse_full_expression();
extern float process_term();
extern float compute_factor();
}

const char* expression_input = nullptr; // Указатель на входную строку
bool use_float_mode = false;           // Режим вещественных чисел

// Фикстура для инициализации перед каждым тестом
struct CalcTestFixture : public ::testing::Test {
    void SetUp() override {
        expression_input = nullptr;
        use_float_mode = false;
    }
};

// Тест сложения
TEST_F(CalcTestFixture, AddNumbers) {
    expression_input = "5 + 12";
    EXPECT_EQ(parse_full_expression(), 17);
}

// Тест вычитания
TEST_F(CalcTestFixture, SubtractNumbers) {
    expression_input = "20 - 7";
    EXPECT_EQ(parse_full_expression(), 13);
}

// Тест умножения
TEST_F(CalcTestFixture, MultiplyNumbers) {
    expression_input = "6 * 4";
    EXPECT_EQ(parse_full_expression(), 24);
}

// Тест деления
TEST_F(CalcTestFixture, DivideNumbers) {
    expression_input = "15 / 3";
    EXPECT_EQ(parse_full_expression(), 5);
}

// Тест скобок
TEST_F(CalcTestFixture, HandleBrackets) {
    expression_input = "(3 + 5) * 2";
    EXPECT_EQ(parse_full_expression(), 16);
}

// Тест деления на ноль
TEST_F(CalcTestFixture, ZeroDivision) {
    expression_input = "12 / 0";
    ASSERT_EXIT(parse_full_expression(), ::testing::ExitedWithCode(1),
                ".*Ошибка: Деление на ноль.*");
}

// Тест некорректных операторов
TEST_F(CalcTestFixture, InvalidOperatorSequence) {
    expression_input = "4 +* 6";
    ASSERT_EXIT(parse_full_expression(), ::testing::ExitedWithCode(1),
                ".*Ошибка: Последовательные операторы.*");
}

// Тест максимального числа
TEST_F(CalcTestFixture, MaxIntegerLimit) {
    char test_input[64];
    snprintf(test_input, sizeof(test_input), "%lld + 0", MAX_INT_BOUND);
    expression_input = test_input;
    EXPECT_NEAR(parse_full_expression(), MAX_INT_BOUND, 512);
}

// Тест минимального числа
TEST_F(CalcTestFixture, MinIntegerLimit) {
    char test_input[64];
    snprintf(test_input, sizeof(test_input), "%lld + 0", MIN_INT_BOUND);
    expression_input = test_input;
    EXPECT_NEAR(parse_full_expression(), MIN_INT_BOUND, 512);
}

// Тест сложения в float-режиме
TEST_F(CalcTestFixture, FloatAddition) {
    use_float_mode = true;
    expression_input = "2.5 + 1.7";
    EXPECT_FLOAT_EQ(parse_full_expression(), 4.2f);
}

// Тест деления в float-режиме
TEST_F(CalcTestFixture, FloatDivision) {
    use_float_mode = true;
    expression_input = "10 / 3";
    EXPECT_FLOAT_EQ(parse_full_expression(), 3.3333f);
}

// Тест малого делителя в float-режиме
TEST_F(CalcTestFixture, TinyDivisorInFloat) {
    use_float_mode = true;
    expression_input = "7 / 0.00003";
    ASSERT_EXIT(parse_full_expression(), ::testing::ExitedWithCode(1),
                ".*Ошибка: Делитель слишком мал.*");
}

// Тест вложенных отрицаний
TEST_F(CalcTestFixture, NestedNegatives) {
    expression_input = "-(-6)";
    EXPECT_EQ(parse_full_expression(), 6);
}

// Тест приоритета операций
TEST_F(CalcTestFixture, PrecedenceCheck) {
    expression_input = "2 + 4 * 3";
    EXPECT_EQ(parse_full_expression(), 14);
}
