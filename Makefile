CC = gcc
FLAG_C = -c
ASAN = -g -fsanitize=address
FLAG_ER = -Wall -Werror -Wextra -std=c11
VALGRIND_FLAGS =  --leak-check=full --track-origins=yes

TET_LIB_C = ./brick_game/tetris/fsm.c ./brick_game/tetris/backend.c ./brick_game/tetris/api.c
TET_LIB_O = $(BUILD_DIR)/fsm.o $(BUILD_DIR)/backend.o $(BUILD_DIR)/api.o
TET_LIB_H =  ./brick_game/tetris/backend.h ./brick_game/tetris/api.h
CLI_C = ./gui/cli/frontend.c ./gui/cli/cli_main.c
CLI_H = ./gui/cli/frontend.h
SUITE_CASES_C = suite_*.c
SUITE_CASES_O = suite_*.o
FLAG_TESTS = -lcheck
TESTS_C = ./tests/test.c
BIN_DIR = bin
BUILD_DIR = build

ifeq ($(shell uname), Linux)
	FLAG_TESTS += -lsubunit -lm -lrt
endif

ifeq ($(shell uname),Darwin)
OPEN_CMD=open
endif

ifeq ($(shell uname),Linux)
OPEN_CMD=xdg-open
ADD_LDFLAGS=-lm -lsubunit
endif

all: clean uninstall install 

install: tetris_lib.a | $(BIN_DIR)
	$(CC) -Wall -Wextra -Werror -Wpedantic -o $(BIN_DIR)/Tetris $(CLI_C) $(BUILD_DIR)/tetris_lib.a -lncurses
	make clean

uninstall:
	-rm -f $(BIN_DIR)/Tetris
	-rmdir $(BIN_DIR) 2>/dev/null || true

tetris_lib.a: $(TET_LIB_O) | $(BUILD_DIR)
	ar rc $(BUILD_DIR)/tetris_lib.a $(TET_LIB_O) -lm
	ranlib $(BUILD_DIR)/tetris_lib.a

$(BUILD_DIR)/%.o: ./brick_game/tetris/%.c | $(BUILD_DIR)
	$(CC) $(FLAG_C) $(FLAG_ER) -o $@ $<

test: clean tetris_lib.a | $(BUILD_DIR)
	$(CC) -g --coverage $(FLAG_ER) $(TESTS_C) -o $(BUILD_DIR)/test $(BUILD_DIR)/tetris_lib.a $(FLAG_TESTS)
	./$(BUILD_DIR)/test

gcov_report: test | $(BUILD_DIR)
	$(CC) --coverage $(TET_LIB_C) $(TESTS_C) -o $(BUILD_DIR)/greport $(BUILD_DIR)/tetris_lib.a $(FLAG_TESTS)
	./$(BUILD_DIR)/greport
	lcov -t "test" -o $(BUILD_DIR)/tetris.info -c -d .
	genhtml -o ./report $(BUILD_DIR)/tetris.info --exclude "tests/test.c"
	$(G)$(OPEN_CMD) report/index.html

valgrind_test: tetris_lib.a | $(BUILD_DIR)
	$(CC) -g $(FLAG_ER) -o $(BUILD_DIR)/valgrind_test_bin $(TESTS_C) $(BUILD_DIR)/tetris_lib.a $(FLAG_TESTS)
	valgrind $(VALGRIND_FLAGS) ./$(BUILD_DIR)/valgrind_test_bin > valgrind_output.txt 2>&1

dvi:
	$(G)$(OPEN_CMD) docs/dvi.html

dist:
	tar -czf tetris.install.tar.gz ./* 

cpp_check:
	cppcheck --enable=all --force --suppress=missingIncludeSystem --inconclusive $(TET_LIB_C) $(CLI_C) 

style_check:
	cp ../materials/linters/.clang-format ./
	clang-format -n gui/cli/* brick_game/tetris/* *.h tests/* 
	clang-format -i gui/cli/* brick_game/tetris/* *.h tests/* 
	rm -rf .clang-format

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	-rm -rf *.html *.gcda *.gcno *.css *.gcov *.info *.out *.cfg *.txt *.o
	-rm -rf report
	-rm -rf $(BUILD_DIR)
	find . -type d -name '*.dSYM' -exec rm -r {} +