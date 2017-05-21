CC := gcc
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
LIBPREFIX := lib
TESTDIR := test
OUTDIR := bin
TARGET := $(LIBPREFIX)wssha256engine.so
TESTTARGET := wssha256enginetest
 
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

TESTSOURCES := $(shell find $(TESTDIR) -type f -name "*.$(SRCEXT)")
TESTCFLAGS := -g -Wall

CFLAGS := -Wall -fPIC
LIB := `pkg-config --libs openssl` -L$(LIBDIR) # -lwssha
INC := -I include 

all: $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TESTTARGET) 

# Link object files into a shared library
$(OUTDIR)/$(TARGET): $(OBJECTS) 
	@echo "Linking..."
	@mkdir -p $(OUTDIR)
	$(CC) -shared -o $(OUTDIR)/$(TARGET) $(LIB) $^
	@echo "Completed"
	@echo "------------------------------------------------------ "

# Compile source into object files 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) 
	@echo "------------------------------------------------------ "
	@echo "Building source files..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
$(OUTDIR)/$(TESTTARGET): $(OUTDIR)/$(TARGET) $(TESTSOURCES)
	@echo "Building Tests..."
	$(CC) $(TESTCFLAGS) $(TESTSOURCES) $(INC) $(LIB) -o $(OUTDIR)/$(TESTTARGET)
	@echo "Test Build Completed"
	@echo "------------------------------------------------------ "

## Clean
clean:
	@echo "Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(OUTDIR)

.PHONY: clean 
