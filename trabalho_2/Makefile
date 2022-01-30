# the compiler
CC = gcc

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC)  -pthread -o $(TARGET).out $(TARGET).c

clean:
	$(RM) $(TARGET)
