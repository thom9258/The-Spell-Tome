# (((FORCE_NO_BACKUP)))
#COMPILER		= g++ -m64 -g -Wall -std=c++11
COMPILER		= gcc -m64 -g -Wall -std=c99
TARGET			= a.out
MAIN			= test.c

$(TARGET): $(MAIN) $(SOURCES)
	 $(COMPILER) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)
