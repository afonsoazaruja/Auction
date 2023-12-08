CC = gcc
CFLAGS = # -Wall -Werror

# Source files (all .c files in the current directory)
SRCS = $(wildcard *.c) ../validations.c

# Executable name
TARGET = AS

# Default rule
all: $(TARGET)

# Rule for creating the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
