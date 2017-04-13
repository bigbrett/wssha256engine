CC := gcc
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
TESTDIR := test
OUTDIR := bin
TARGET := wssha256engine.so
 
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS := -Wall -fPIC
LIB := `pkg-config --libs openssl` -L$(LIBDIR)
INC := -I include 

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
	@echo "Building..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Clean
clean:
	@echo "Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(OUTDIR)"; $(RM) -r $(BUILDDIR) $(OUTDIR)
#	@cd $(TESTDIR); make clean

# Tests
#test:
#	@cd $(TESTDIR); make

.PHONY: clean test 
