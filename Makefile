# Makefile for CCIMX93 Hello World Application
# Compatible with Yocto build system

# Compiler and flags
CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c99
LDFLAGS ?= 

# Directories
SRCDIR = .
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = hello-world.c system-test.c
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)

# Target executables
TARGETS = hello-world system-test

# Default target
all: $(TARGETS)

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link hello-world executable
hello-world: $(OBJDIR)/hello-world.o | $(BINDIR)
	$(CC) $(LDFLAGS) $< -o $@

# Link system-test executable
system-test: $(OBJDIR)/system-test.o | $(BINDIR)
	$(CC) $(LDFLAGS) $< -o $@

# Install target (used by Yocto)
install: $(TARGETS)
	@echo "Installing executables..."
	@if [ -n "$(DESTDIR)" ]; then \
		install -d $(DESTDIR)/usr/bin; \
		install -m 0755 hello-world $(DESTDIR)/usr/bin/; \
		install -m 0755 system-test $(DESTDIR)/usr/bin/; \
		echo "Installed to $(DESTDIR)/usr/bin/"; \
	else \
		echo "DESTDIR not set, installing to current directory"; \
		install -d ./install/usr/bin; \
		install -m 0755 hello-world ./install/usr/bin/; \
		install -m 0755 system-test ./install/usr/bin/; \
	fi

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR) $(TARGETS)
	rm -rf ./install

# Clean everything including executables
distclean: clean
	rm -f hello-world system-test

# Show help
help:
	@echo "Available targets:"
	@echo "  all         - Build all executables (default)"
	@echo "  hello-world - Build main hello world application"
	@echo "  system-test - Build system testing utility"
	@echo "  install     - Install executables (used by Yocto)"
	@echo "  clean       - Remove build artefacts"
	@echo "  distclean   - Remove all generated files"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC          - Compiler (default: gcc)"
	@echo "  CFLAGS      - Compiler flags"
	@echo "  LDFLAGS     - Linker flags"
	@echo "  DESTDIR     - Installation destination"

# Test target
test: $(TARGETS)
	@echo "Testing executables..."
	@./hello-world --version
	@./system-test

# Development target
dev: CFLAGS += -g -DDEBUG
dev: $(TARGETS)

.PHONY: all install clean distclean help test dev

# Dependency tracking
-include $(OBJECTS:.o=.d)

$(OBJDIR)/%.d: $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@
