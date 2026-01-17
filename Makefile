PROJECT_NAME = mirlang
STD = c99

CC = gcc
LDFLAGS = -fsanitize=address,undefined
CFLAGS = -std=$(STD) $(LDFLAGS) -fno-omit-frame-pointer -Wall -Wextra

OUT = build
# common core files used in main and tests and could be shared
CORE = utils.c lexer.c

# main binary building source files
SRCS = main.c $(CORE)

# common core source files
CORE_OBJS = $(CORE:%.c=$(OUT)/%.o)

# main binary object files
OBJS = $(SRCS:%.c=$(OUT)/%.o)

all: $(OUT)/$(PROJECT_NAME)

$(OUT):
	mkdir -p $(OUT)

$(OUT)/$(PROJECT_NAME): $(OUT) $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/tests: $(OUT) $(CORE_OBJS) tests.c
	$(CC) $(CORE_OBJS) tests.c -o $@ $(LDFLAGS) $(CFLAGS)

check: $(OUT)/tests

debug: all
	gdb ./$(OUT)/$(PROJECT_NAME)

run: all
	./$(OUT)/$(PROJECT_NAME)

test: check
	./$(OUT)/tests

clean: 
	rm -rf $(OUT)

.PHONY: all debug run clean
