PROJECT_NAME = mirlang
STD = c99

CC = gcc
LDFLAGS = -fsanitize=address,undefined -I./deps/
# INFO: remove -DDEBUG_PRINTS on release 
CFLAGS = -std=$(STD) $(LDFLAGS) -g -fno-omit-frame-pointer -Wall -Wextra

OUT = build
# common core files used in main and tests and could be shared
CORE = utils.c lexer.c repl.c parser.c ast.c arrays.c

# main binary building source files
SRCS = main.c $(CORE)

# common core source files
CORE_OBJS = $(CORE:%.c=$(OUT)/%.o)

# main binary object files
OBJS = $(SRCS:%.c=$(OUT)/%.o)

all: $(OUT)/$(PROJECT_NAME)-debug

deps:
	if [ ! -d "deps" ]; then \
	mkdir -p deps; \
	cd deps; \
	git clone https://github.com/mirsahebali/cstring.h; \
	fi

$(OUT):
	mkdir -p $(OUT)

$(OUT)/$(PROJECT_NAME)-debug: $(OUT) $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/tests: $(OUT) $(CORE_OBJS) tests.c
	$(CC) $(CORE_OBJS) tests.c -o $@ $(LDFLAGS) $(CFLAGS)

check: $(OUT)/tests

debug: all
	gdb ./$(OUT)/$(PROJECT_NAME)-debug

run: all
	./$(OUT)/$(PROJECT_NAME)-debug

test: check
	./$(OUT)/tests

clean: 
	rm -rf $(OUT) deps

.PHONY: all debug run clean deps
