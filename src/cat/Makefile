FLAGS = -Wall -Wextra -Werror -std=c11 -pedantic
TARGET = s21_cat
TEST_SCRIPT = test/test_s21_cat.py
TEST_DIR = test

all: $(TARGET)
	@echo "test, format, rebuild, clean"

$(TARGET): Makefile s21_cat.h s21_cat.c
	gcc $(FLAGS) s21_cat.c -o $(TARGET)

test: $(TARGET)
	@echo "Running tests..."
	@command -v python3 >/dev/null 2>&1 || { echo "Python3 is not installed. Please install Python3 to run the tests."; exit 1; }
	python3 $(TEST_SCRIPT)

format:
	clang-format -style=Google -n *.c
	clang-format -style=Google -n *.h

rebuild:
	make clean
	make all

clean:
	rm -f $(TARGET)
	rm -rf $(TEST_DIR)/*.out
	rm commands.testing
