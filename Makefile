# Компиляторы и архиватор
CC_COMPILER  ?= gcc
CXX_COMPILER ?= g++
ARCHIVER     ?= ar

# Пути к исходникам и сборке
SOURCE_PATH   ?= src
TEST_PATH     ?= tests
OUTPUT_PATH   ?= build

# Подкаталоги сборки
APP_OUT_DIR=$(OUTPUT_PATH)/app
TEST_OUT_DIR=$(OUTPUT_PATH)/test
TEST_APP_OBJS_DIR=$(TEST_OUT_DIR)/app
TEST_UNIT_OBJS_DIR=$(TEST_OUT_DIR)/unit-tests

# Путь к Google Test
GOOGLE_TEST_DIR ?= googletest/googletest
CXX_FLAGS += -g -Wall -Wextra -pthread -std=c++17 -isystem $(GOOGLE_TEST_DIR)/include # Флаги C++
C_FLAGS   += -g -Wall -Wextra -Wpedantic -Werror -std=c11 # Флаги C

# Поиск исходных файлов
APP_SOURCES := $(shell find $(SOURCE_PATH) -name '*.c')
TEST_SOURCES := $(shell find $(TEST_PATH)/unit -name '*.cpp')

# Диагностика сборки
$(info --- Диагностика сборки ---)
$(info APP_SOURCES = $(APP_SOURCES))
$(info TEST_SOURCES = $(TEST_SOURCES))

# Проверка наличия исходников
ifeq ($(strip $(APP_SOURCES)),)
$(error Нет исходных файлов .c в каталоге $(SOURCE_PATH))
endif
ifeq ($(strip $(TEST_SOURCES)),)
$(warning Нет тестовых файлов .cpp в каталоге $(TEST_PATH)/unit)
endif

# Объектные файлы
APP_OBJECTS := $(APP_SOURCES:$(SOURCE_PATH)/%.c=$(APP_OUT_DIR)/%.o)
TEST_UNIT_OBJECTS := $(TEST_SOURCES:$(TEST_PATH)/unit/%.cpp=$(TEST_UNIT_OBJS_DIR)/%.o)

# Диагностика объектов
$(info APP_OBJECTS = $(APP_OBJECTS))
$(info TEST_UNIT_OBJECTS = $(TEST_UNIT_OBJECTS))

# Цели сборки
.PHONY: all clean run-int run-float run-unit-test venv run-integration-tests

all: $(OUTPUT_PATH)/app.exe $(OUTPUT_PATH)/unit-tests.exe # Сборка app.exe и unit-tests.exe

clean: # Очистка всех артефактов
	rm -rf $(OUTPUT_PATH)

run-int: $(OUTPUT_PATH)/app.exe # Запуск app.exe в int-режиме
	@$<

run-float: $(OUTPUT_PATH)/app.exe # Запуск app.exe в float-режиме
	@$< --float

run-unit-test: $(OUTPUT_PATH)/unit-tests.exe # Запуск юнит-тестов
	@$<

# Компиляция объектных файлов приложения
$(APP_OUT_DIR)/%.o: $(SOURCE_PATH)/%.c
	@mkdir -p $(@D)
	$(CC_COMPILER) $(C_FLAGS) -MMD -MP -c $< -o $@

# Сборка приложения
$(OUTPUT_PATH)/app.exe: $(APP_OBJECTS)
	$(info Сборка $(OUTPUT_PATH)/app.exe из $(APP_OBJECTS))
	$(CC_COMPILER) $(C_FLAGS) $(APP_OBJECTS) -o $(OUTPUT_PATH)/app.exe

# Компиляция main_test.o для тестов
$(TEST_APP_OBJS_DIR)/main_test.o: $(SOURCE_PATH)/main.c
	@mkdir -p $(@D)
	$(CC_COMPILER) $(C_FLAGS) -MMD -MP -DGTEST -c $< -o $@

# Компиляция тестов
$(TEST_UNIT_OBJS_DIR)/%.o: $(TEST_PATH)/unit/%.cpp
	@mkdir -p $(@D)
	$(CXX_COMPILER) $(CXX_FLAGS) -MMD -MP -c $< -o $@

# Сборка юнит-тестов
$(OUTPUT_PATH)/unit-tests.exe: $(TEST_UNIT_OBJECTS) $(TEST_APP_OBJS_DIR)/main_test.o $(TEST_OUT_DIR)/gtest_library.a
	$(info Сборка $(OUTPUT_PATH)/unit-tests.exe из $^)
	$(CXX_COMPILER) $(CXX_FLAGS) -lpthread $(TEST_UNIT_OBJECTS) $(TEST_APP_OBJS_DIR)/main_test.o $(TEST_OUT_DIR)/gtest_library.a -o $(OUTPUT_PATH)/unit-tests.exe

# Компиляция Google Test
$(TEST_OUT_DIR)/gtest_core.o: $(GOOGLE_TEST_DIR)/src/gtest-all.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -I$(GOOGLE_TEST_DIR) -c $< -o $@

$(TEST_OUT_DIR)/gtest_entry.o: $(GOOGLE_TEST_DIR)/src/gtest_main.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -I$(GOOGLE_TEST_DIR) -c $< -o $@

$(TEST_OUT_DIR)/gtest_library.a: $(TEST_OUT_DIR)/gtest_core.o $(TEST_OUT_DIR)/gtest_entry.o
	$(ARCHIVER) $(ARFLAGS) $@ $^

venv: # Создание виртуального окружения и установка pytest
	python3 -m venv venv && . venv/bin/activate && pip install pytest

# Запуск интеграционных тестов с отладкой
run-integration-tests: $(OUTPUT_PATH)/app.exe
	@echo "Запуск интеграционных тестов..."
	@if [ -f $(TEST_PATH)/integration/integr_test.py ]; then \
		echo "Обнаружен integr_test.py, запускаю pytest..."; \
		pytest $(TEST_PATH)/integration/; \
	else \
		echo "Файл integr_test.py не найден, проверяю наличие integration_tests.py..."; \
		if [ -f $(TEST_PATH)/integration/integration_tests.py ]; then \
			echo "Обнаружен integration_tests.py, запускаю python3..."; \
			python3 $(TEST_PATH)/integration/integration_tests.py 2>&1; \
			if [ $$? -eq 0 ]; then \
				echo "Все тесты в integration_tests.py успешно пройдены."; \
			else \
				echo "Ошибка: Некоторые тесты в integration_tests.py не пройдены."; \
				exit 1; \
			fi; \
		else \
			echo "Ошибка: в $(TEST_PATH)/integration/ нет ожидаемых файлов тестов (integr_test.py или integration_tests.py)."; \
			exit 1; \
		fi; \
	fi

# Подключение зависимостей
-include $(APP_OBJECTS:.o=.d)
-include $(TEST_UNIT_OBJECTS:.o=.d)
