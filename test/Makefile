CC := gcc
SRCDIR := .
BUILDDIR := ../build
TARGET := ../bin/test

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(shell find $(BUILDDIR) -type f -name "*.o" ! -name "main.o")
CFLAGS := -g -Wall 
LIB := `pkg-config --libs openssl` 

INC := -I../include -I../src 

$(TARGET): $(SOURCES)
	@echo "  Building Tests..."
	$(CC) $(CFLAGS) $^ $(OBJECTS) $(INC) $(LIB) -o $(TARGET)
	@echo "  Completed"
	@echo "  ------------------------------------------------------ "

clean:
	@echo "  Cleaning Tests..."; 
	$(RM) -r $(TARGET) $(TARGET).dSYM

.PHONY: clean run
