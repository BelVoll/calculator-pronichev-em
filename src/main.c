#include <ctype.h>   // Для isspace, isdigit
#include <stdbool.h> // Для bool
#include <stdio.h>   // Для ввода-вывода
#include <stdlib.h>  // Для exit
#include <string.h>  // Для strchr

#define INPUT_LIMIT 1024         // Максимальный размер ввода
#define TINY_DIV_THRESHOLD 1e-4f // Минимальный делитель в float-режиме
#define MAX_INT_RANGE 2000000000LL // Максимальное целое число
#define MIN_INT_RANGE -2000000000LL // Минимальное целое число

#ifdef GTEST
extern const char* expression_input; // Указатель на входную строку (для тестов)
extern bool use_float_mode;          // Режим вещественных чисел (для тестов)
#else
static const char* expression_input; // Указатель на входную строку
static bool use_float_mode = false;  // Режим вещественных чисел
#endif

// Функции для вычисления выражения
float parse_full_expression(void); // Парсит полное выражение (+, -)
float process_term(void);          // Обрабатывает термы (*, /)
float compute_factor(void);        // Вычисляет факторы (числа, скобки)

float parse_full_expression(void) {
    while (isspace(*expression_input)) expression_input++; // Пропуск пробелов
    float total = process_term(); // Начальное значение
    while (true) {
        while (isspace(*expression_input)) expression_input++; // Пропуск пробелов
        char current = *expression_input; // Текущий символ
        if (current == '+' || current == '-') {
            if (strchr("+-*/", *(expression_input + 1))) { // Проверка на последовательные операторы
                fprintf(stderr, "Ошибка: Последовательные операторы недопустимы!\n");
                exit(1);
            }
            expression_input++;
            total += (current == '+') ? process_term() : -process_term(); // Сложение или вычитание
        } else {
            break;
        }
    }
    return total;
}

float process_term(void) {
    while (isspace(*expression_input)) expression_input++; // Пропуск пробелов
    float value = compute_factor(); // Начальное значение
    while (true) {
        while (isspace(*expression_input)) expression_input++; // Пропуск пробелов
        char op = *expression_input; // Текущий оператор
        if (op == '*' || op == '/') {
            if (strchr("+-*/", *(expression_input + 1))) { // Проверка на последовательные операторы
                fprintf(stderr, "Ошибка: Последовательные операторы недопустимы!\n");
                exit(1);
            }
            expression_input++;
            float next_factor = compute_factor(); // Следующий фактор
            if (op == '*') {
                value *= next_factor; // Умножение
            } else {
                if (use_float_mode && next_factor < TINY_DIV_THRESHOLD) { // Проверка малого делителя
                    fprintf(stderr, "Ошибка: Делитель слишком мал (<10^-4)!\n");
                    exit(1);
                }
                if (!use_float_mode && next_factor == 0) { // Проверка деления на ноль
                    fprintf(stderr, "Ошибка: Деление на ноль запрещено!\n");
                    exit(1);
                }
                if (use_float_mode) {
                    value = value / next_factor; // Деление в float-режиме
                    value = (float)((int)(value * 10000 + (value >= 0 ? 0.5 : -0.5))) / 10000; // Округление
                } else {
                    value = (int)(value / next_factor); // Деление в int-режиме
                }
            }
        } else {
            break;
        }
        if (!use_float_mode && (value > MAX_INT_RANGE || value < MIN_INT_RANGE)) { // Проверка переполнения
            fprintf(stderr, "Ошибка: Выход за пределы целых чисел!\n");
            exit(1);
        }
    }
    return value;
}

float compute_factor(void) {
    while (isspace(*expression_input)) expression_input++; // Пропуск пробелов
    bool negative = false;
    if (*expression_input == '-') { // Обработка отрицательного числа
        negative = true;
        expression_input++;
        while (isspace(*expression_input)) expression_input++;
    }
    if (*expression_input == '(') { // Обработка скобок
        expression_input++;
        float nested_result = parse_full_expression();
        while (isspace(*expression_input)) expression_input++;
        if (*expression_input != ')') {
            fprintf(stderr, "Ошибка: Непарные скобки!\n");
            exit(1);
        }
        expression_input++;
        return negative ? -nested_result : nested_result;
    }

    long long whole_part = 0; // Целая часть числа
    float decimal_part = 0.0f; // Дробная часть числа
    int decimal_scale = 1;     // Множитель для дробной части
    bool has_decimal = false;  // Флаг точки

    while (isdigit(*expression_input) || (use_float_mode && *expression_input == '.')) {
        if (*expression_input == '.') {
            if (has_decimal) {
                fprintf(stderr, "Ошибка: Множественные точки в числе!\n");
                exit(1);
            }
            has_decimal = true;
        } else if (!has_decimal) {
            whole_part = whole_part * 10 + (*expression_input - '0');
            if (!use_float_mode && whole_part > MAX_INT_RANGE) {
                fprintf(stderr, "Ошибка: Число слишком велико!\n");
                exit(1);
            }
        } else {
            decimal_part = decimal_part + (float)(*expression_input - '0') / (decimal_scale *= 10);
        }
        expression_input++;
    }
    float result = whole_part + (use_float_mode ? decimal_part : 0); // Итоговое число
    return negative ? -result : result;
}

#ifndef GTEST
int main(int argc, char* argv[]) {
    use_float_mode = (argc > 1 && strcmp(argv[1], "--float") == 0); // Установка режима float
    char buffer[INPUT_LIMIT]; // Буфер для ввода
    int pos = 0, symbol;
    int brackets_open = 0, brackets_closed = 0; // Счётчики скобок
    bool operator_flag = false; // Флаг последнего оператора (изначально false)

    while ((symbol = getchar()) != EOF && pos < INPUT_LIMIT - 1) { // Чтение ввода
        if (!strchr("0123456789()+-*/ \t\n", symbol) && !(use_float_mode && symbol == '.')) {
            fprintf(stderr, "Ошибка: Недопустимый символ!\n");
            return 1;
        }
        if (symbol == '(') brackets_open++;
        else if (symbol == ')') brackets_closed++;
        if (strchr("+-*/", symbol)) {
            if (operator_flag && strchr("+-*/", buffer[pos - 1])) {
                fprintf(stderr, "Ошибка: Последовательные операторы недопустимы!\n");
                return 1;
            }
            operator_flag = true;
        } else {
            operator_flag = false;
        }
        buffer[pos++] = (char)symbol;
    }
    buffer[pos] = '\0';

    if (brackets_open != brackets_closed) { // Проверка баланса скобок
        fprintf(stderr, "Ошибка: Несбалансированные скобки!\n");
        return 1;
    }
    if (pos == 0) { // Проверка пустого ввода
        fprintf(stderr, "Ошибка: Ввод пуст!\n");
        return 1;
    }
    if (pos >= INPUT_LIMIT - 1) { // Проверка превышения лимита
        fprintf(stderr, "Ошибка: Превышен лимит ввода!\n");
        return 1;
    }

    expression_input = buffer; // Установка указателя на буфер
    float outcome = parse_full_expression(); // Вычисление результата

    while (isspace(*expression_input)) expression_input++; // Пропуск пробелов в конце
    if (*expression_input && *expression_input != '\n') {
        fprintf(stderr, "Ошибка: Лишние символы в выражении!\n");
        return 1;
    }

    if (use_float_mode) { // Вывод результата с округлением
        printf("%.4f\n", outcome);
    } else {
        printf("%lld\n", (long long)outcome);
    }
    return 0;
}
#endif
