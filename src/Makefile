CC = gcc
CFLAGS = -Wall -pedantic -Werror -g 

AS_DIR = AS_code
USER_DIR = User_code

AS_SRCS = $(wildcard $(AS_DIR)/*.c) validations.c
USER_SRCS = $(wildcard $(USER_DIR)/*.c) validations.c

AS_TARGET = server
USER_TARGET = user

all: server user

server: $(AS_TARGET)
$(AS_TARGET): $(AS_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

user: $(USER_TARGET)
$(USER_TARGET): $(USER_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(AS_TARGET)
	rm -f $(USER_TARGET)
