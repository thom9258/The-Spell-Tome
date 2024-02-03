COMPILER		= gcc -m64 -g -Wall -std=c99
EXTRAFLAGS      = -Wextra -Werror -ggdb 
TARGET			= a.out
MAIN			= test.c

.PHONY: clean

$(TARGET): $(MAIN)
	 $(COMPILER) $(EXTRAFLAGS) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)
