CC = gcc
CFLAGS = -Wall -pedantic -Werror -g 

# Source files (all .c files in the current directory)
SRCS = $(wildcard *.c) ../validations.c

# Executable name
TARGET = user

# Default rule
all: $(TARGET)

# Rule for creating the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
