C_COMPILER		= gcc -m64 -g -Wall -std=c99
CPP_COMPILER	= g++ -m64 -g -Wall -std=c++14
TARGET			= a.out
MAIN			= test.c

ERR = To prove c/c++ languange & compiler compatability, please <make> using either <make c> or <make cpp>

.PHONY: c cpp clean

$(TARGET): $(MAIN) $(SOURCES)
	$(error $(ERR))

cpp: $(MAIN) $(SOURCES)
	$(CPP_COMPILER) -o $(TARGET) $(MAIN)

c: $(MAIN) $(SOURCES)
	$(C_COMPILER) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)
