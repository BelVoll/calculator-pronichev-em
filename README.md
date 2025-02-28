# Calculator-pronichev-em

Этот проект представляет собой консольный калькулятор, написанный на языке C, который выполняет арифметические вычисления выражений, введённых через стандартный ввод (`stdin`), и выводит результат в стандартный вывод (`stdout`). Калькулятор поддерживает как целочисленные, так и вещественные операции (с флагом `--float`), обрабатывает скобки, приоритет операций и проверяет корректность ввода. Проект включает юнит-тесты на C++ (с использованием Google Test), интеграционные тесты на Python (с использованием `pytest` для моего теста и прямого запуска для партнёрского), а также `Makefile` для автоматизации сборки и запуска.

Основные возможности:
- Поддержка операций: `+`, `-`, `*`, `/`.
- Целочисленный режим: результат округляется к меньшему целому при делении.
- Вещественный режим (`--float`): результат выводится с точностью до 4 знаков после запятой.
- Обработка ошибок: деление на ноль, малый делитель (< 10⁻⁴ в `--float`), некорректные символы, последовательные операторы, переполнение чисел.
- Ограничение ввода: не более 1024 байт.

Проект включает:
- Исходный код калькулятора: `src/main.c`.
- Юнит-тесты: `tests/unit/tests.cpp`.
- Интеграционные тесты: `tests/integration/integr_test.py` (мой тест) и `tests/integration/integration_tests.py` (партнёрский тест).

## Как запустить/использовать?

### Требования
- Установленные компиляторы: `gcc`, `g++`.
- Python 3.10+ с модулем `venv` (рекомендуется установить `python3-venv` на Debian/Ubuntu: `sudo apt install python3-venv`).
- Библиотека Google Test (включена в скрытую папку `.googletest/`).

### Сборка проекта
1. Склонируйте репозиторий:
   ```bash
   git clone <repository_url>
   cd calculator-pronichev-em
   ```
2. Скомпилируйте проект:
   ```bash
   make all
   ```
   - Создаются исполняемые файлы в скрытой папке `.build/`:
     - `.build/app.exe` (калькулятор).
     - `.build/unit-tests.exe` (юнит-тесты).

### Запуск калькулятора
- В целочисленном режиме:
  ```bash
  echo "5 + 12" | ./.build/app.exe
  # Вывод: 17
  ```
  Или через `make`:
  ```bash
  make run-int
  ```
  Введите выражение (например, `5 + 12`) и нажмите `Ctrl+D` (Unix) или `Ctrl+Z` (Windows).

- В вещественном режиме:
  ```bash
  echo "2.5 + 1.7" | ./.build/app.exe --float
  # Вывод: 4.2000
  ```
  Или:
  ```bash
  make run-float
  ```

### Запуск тестов
- Юнит-тесты:
  ```bash
  make run-unit-test
  # Вывод: отчёт Google Test о 14 тестах
  ```

- Интеграционные тесты:
  1. Установите виртуальное окружение:
     ```bash
     make venv
     source .venv/bin/activate
     ```
     - Создаётся скрытая папка `.venv/` с Python-окружением и `pytest`.
  2. Запустите тесты:
     - С моим тестом (`integr_test.py`):
       ```bash
       rm tests/integration/integration_tests.py  # Удалите чужой тест, если есть
       # Убедитесь, что integr_test.py есть в tests/integration/
       make run-integration-tests
       # Вывод: отчёт pytest о 14 тестах
       ```
     - С партнёрским тестом (`integration_tests.py`):
       ```bash
       rm tests/integration/integr_test.py  # Удалите мой тест, если есть
       # Убедитесь, что integration_tests.py есть в tests/integration/
       make run-integration-tests
       # Вывод: "Все тесты в integration_tests.py успешно пройдены" при успехе
       ```

### Очистка
Удалите все скомпилированные файлы:

- Удаляет скрытую папку `.build/`.

## Как это сделано?

### Калькулятор (`src/main.c`)
- **Реализация**: Рекурсивный парсер выражений с использованием трёх функций:
  - `parse_full_expression`: Обрабатывает сложение и вычитание.
  - `process_term`: Обрабатывает умножение и деление.
  - `compute_factor`: Парсит числа и скобки, включая отрицательные значения.
- **Ввод**: Читается через `getchar()` с ограничением 1024 байта.
- **Вывод**: Форматируется через `printf` (`%lld` для целых, `%.4f` для вещественных).
- **Ошибки**: Проверяются условия (деление на ноль, некорректные символы, последовательные операторы) с выводом в `stderr` и завершением с кодом 1.

### Юнит-тесты (`tests/unit/tests.cpp`)
- Написаны на C++ с использованием Google Test (из `.googletest/`).
- Покрывают 14 сценариев: базовые операции, скобки, ошибки, краевые значения, приоритет.
- Компилируются в `.build/unit-tests.exe` и запускаются через `make run-unit-test`.

### Интеграционные тесты
- **Мой тест (`integr_test.py`)**:
  - Использует `pytest` (из `.venv/`) для запуска 14 тестов.
  - Проверяет `stdout`, `stderr` и код возврата `.build/app.exe` через `subprocess.run`.
- **Партнёрский тест (`integration_tests.py`)**:
  - Запускается через `python3` без изменений.
  - Проверяет 5 сценариев (сложение, вычитание, умножение, деление, float-режим) через `assert`.
- Хранятся в `tests/integration/`, заменяются вручную перед запуском через `make run-integration-tests`.

### `Makefile`
- **Сборка**:
  - `make all`: Компилирует `.build/app.exe` и `.build/unit-tests.exe`, управляя зависимостями через `.o` и `.d` файлы.
- **Запуск**:
  - `run-int`, `run-float`: Запускают `.build/app.exe` с/без флага `--float`.
  - `run-unit-test`: Запускает юнит-тесты из `.build/unit-tests.exe`.
  - `venv`: Создаёт скрытую папку `.venv/` и устанавливает `pytest`.
  - `run-integration-tests`: Условно запускает `pytest` для `integr_test.py` или `python3` для `integration_tests.py`.
- **Очистка**: `make clean` удаляет `.build/`.
- Зависимости автоматически собираются благодаря правилам для `.o` файлов и включению `.d` файлов.

### Структура проекта
calculator-pronichev-em/
├── .build/                 # Скрытая папка с скомпилированными файлами (создаётся при сборке)
│   ├── app.exe             # Исполняемый файл калькулятора
│   └── unit-tests.exe      # Исполняемый файл юнит-тестов
├── src/
│   └── main.c              # Исходный код калькулятора
├── tests/
│   ├── integration/        # Интеграционные тесты
│   │   ├── integr_test.py  # Мой тест (заменяемый)
│   │   └── __pycache__/    # Кэш Python (игнорируется)
│   └── unit/
│       └── tests.cpp       # Юнит-тесты
├── .googletest/            # Скрытая папка с библиотекой Google Test
├── .venv/                  # Скрытая папка с виртуальным окружением Python (создаётся при make venv)
├── Makefile                # Скрипт сборки и запуска
└── README.md               # Описание проекта


Проект полностью соответствует требованиям DZ-3, включая поддержку замены интеграционных тестов и управление зависимостями.
