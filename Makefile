CC = clang
CCFLAGS = -O0 -std=c11 -Wall -Wextra -Wpedantic 
LDFLAGS = 

SRCS = $(shell find src -type f -name '*.c')
OBJS = $(SRCS:.c=.o)
DEPENDS := $(SRCS:.c=.d)
EXEC = main

all: compiler

compiler: $(OBJS)
	$(CC) $^ -o $(EXEC) $(LDFLAGS)
	@echo "Finshed"

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MP

.PHONY clean:
	@rm -f $(OBJS) $(EXEC) $(DEPENDS)

-include $(DEPENDS)
