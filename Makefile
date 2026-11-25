CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = mypreprocessor
SOURCES = main.c comments.c macros.c file_inclusion.c

all: $(TARGET)

$(TARGET): $(SOURCES) preprocessor.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) *.i
