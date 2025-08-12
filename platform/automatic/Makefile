# Location of this Makefile: platform/automatic/Makefile

# Get project root directory and project name from root directory
ROOT_DIR := ../..
PROJECT_NAME := $(notdir $(shell cd $(ROOT_DIR) && pwd))

# Build configuration
BUILD_CONFIG ?= Release

# Directories
SRC_DIR := $(ROOT_DIR)/source
INCLUDE_DIR := $(ROOT_DIR)/include
BUILD_DIR := $(BUILD_CONFIG)
DEPLOY_BASE_DIR := $(ROOT_DIR)/deploy

# Find all subdirectories in source (up to depth 5)
SRC_SUBDIRS := $(shell find $(SRC_DIR) -type d -maxdepth 5)
BUILD_SUBDIRS := $(SRC_SUBDIRS:$(SRC_DIR)%=$(BUILD_DIR)%)

# Find all .c files in the base source directory
APPLICATION_SRCS := $(wildcard $(SRC_DIR)/*.c)

# Find Application targets using entry point detection script
ifeq ($(OS),Windows_NT)
    APPLICATION_TARGETS := $(shell find-entry-points.bat $(SRC_DIR) 2>nul)
else
    APPLICATION_TARGETS := $(shell find-entry-points.sh $(SRC_DIR) 2>/dev/null)
endif

# Convert targets to full paths and object files
APPLICATION_TARGET_PATHS := $(addprefix $(SRC_DIR)/,$(APPLICATION_TARGETS))
APPLICATION_TARGET_OBJS := $(APPLICATION_TARGET_PATHS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Supporting application files (non-entry-point .c files in base directory)
APPLICATION_SUPPORT_SRCS := $(filter-out $(APPLICATION_TARGET_PATHS),$(APPLICATION_SRCS))
APPLICATION_SUPPORT_OBJS := $(APPLICATION_SUPPORT_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# All application objects (for directory creation and cleanup)
APPLICATION_OBJS := $(APPLICATION_TARGET_OBJS) $(APPLICATION_SUPPORT_OBJS)

# Find all library source files (excluding root directory and application targets)
LIB_SRCS := $(shell find $(SRC_DIR) -mindepth 2 -maxdepth 5 -name '*.c')
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)%.c=$(BUILD_DIR)%.o)

# All object files
ALL_OBJS := $(APPLICATION_OBJS) $(LIB_OBJS)

# Get all unique object directories
OBJ_DIRS := $(sort $(dir $(ALL_OBJS)))

# Version information
# (Temporary - this needs to be extracted from the library header or when running version.sh/.bat)
VERSION_MAJOR := 1
VERSION_MINOR := 0
VERSION_PATCH := 0

# Doxygen configuration
DOXYFILE := $(ROOT_DIR)/documentation/doxygen/Doxyfile
DOXYGEN := doxygen

# Build OS detection and settings
ifeq ($(OS),Windows_NT)
    HOST_OS := windows
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        HOST_ARCH := x64
    else
        HOST_ARCH := x86
    endif
    # Commands
    RM := del /Q /S
    MKDIR := mkdir
    RMDIR := rmdir /S /Q
    CP := copy
else
    HOST_ARCH := $(shell uname -m)
    UNAME_S := $(shell uname -s)
    HOST_OS := $(UNAME_S)
    # Commands
    RM := rm -f
    MKDIR := mkdir -p
    RMDIR := rm -rf
    CP := cp -f
endif

ICON_DIR := $(ROOT_DIR)/resource/icon

# Set target OS if not set
TARGET_OS ?= $(HOST_OS)
TARGET_ARCH ?= $(HOST_ARCH)
# Set target platform if not set
PLATFORM ?= $(TARGET_OS)/$(TARGET_ARCH)

ifeq ($(TARGET_OS),windows)
    # Paths
    APP_ICON := $(ICON_DIR)/windows/icon.ico
    # Deliverebles
    SHARED_LIB := $(PROJECT_NAME).dll
    SHARED_FLAGS := -shared
    STATIC_LIB := lib$(PROJECT_NAME).lib
    EXE_EXT := .exe
    APPLICATION_EXE := $(PROJECT_NAME).$(EXE_EXT)
else ifeq ($(TARGET_OS),linux)
    # Paths
    APP_ICON := $(ICON_DIR)/linux/icon.png
    # Deliverebles
    SHARED_LIB := lib$(PROJECT_NAME).so.$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
    SHARED_SONAME := lib$(PROJECT_NAME).so.$(VERSION_MAJOR)
    SHARED_LINK := lib$(PROJECT_NAME).so
    SHARED_FLAGS := -shared -Wl,-soname,$(SHARED_SONAME)
    STATIC_LIB := lib$(PROJECT_NAME).a
    EXE_EXT := 
    APPLICATION_EXE := $(PROJECT_NAME)
else ifeq ($(TARGET_OS),Darwin)
    # Paths
    APP_ICON := $(ICON_DIR)/macos/icon.icns
    # Deliverebles  
    SHARED_LIB := lib$(PROJECT_NAME).$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH).dylib
    SHARED_SONAME := lib$(PROJECT_NAME).$(VERSION_MAJOR).dylib
    SHARED_LINK := lib$(PROJECT_NAME).dylib
    SHARED_FLAGS := -shared -install_name @rpath/$(SHARED_SONAME)
    STATIC_LIB := lib$(PROJECT_NAME).a
    EXE_EXT := 
    APPLICATION_EXE := $(PROJECT_NAME)
else
    # Default/fallback configuration (Unix-like)
    APP_ICON := $(ICON_DIR)/default/icon.png
    SHARED_LIB := lib$(PROJECT_NAME).so.$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
    SHARED_SONAME := lib$(PROJECT_NAME).so.$(VERSION_MAJOR)
    SHARED_LINK := lib$(PROJECT_NAME).so
    SHARED_FLAGS := -shared
    STATIC_LIB := lib$(PROJECT_NAME).a
    EXE_EXT := 
    APPLICATION_EXE := $(PROJECT_NAME)
endif

# Windows resource handling
ifeq ($(TARGET_OS),windows)
    # Resource compiler (Windows only)
    RC := windres
    
    # Resource file
    RESOURCE_FILE := $(SRC_DIR)/$(PROJECT_NAME).rc
    RESOURCE_OBJ := $(BUILD_DIR)/$(PROJECT_NAME)_res.o
    
    # Check if resource file exists
    ifneq ($(wildcard $(RESOURCE_FILE)),)
        # Add resource object to each application if it exists
        RESOURCE_OBJS_FOR_APPS := $(RESOURCE_OBJ)
    endif
endif

# Compiler settings
CC ?= gcc
CFLAGS ?= -Wall -Werror
LDFLAGS ?=

CFLAGS += -I$(INCLUDE_DIR)

# Build type specific settings
ifeq ($(BUILD_CONFIG),Debug)
    CFLAGS += -g -O0 -DDEBUG
else ifeq ($(BUILD_CONFIG),Release)
    CFLAGS += -O3 -DNDEBUG
endif

# Auto-discover libraries from directory structure
LIBRARY_BASE_DIR := $(ROOT_DIR)/library/$(PLATFORM)
LIBRARIES := $(notdir $(wildcard $(LIBRARY_BASE_DIR)/*))

# Include platform-specific library dependencies
ifneq ($(LIBRARIES),)
    $(foreach lib,$(LIBRARIES),$(eval -include $(LIBRARY_BASE_DIR)/$(lib)/$(lib).mk))
endif

# Debug: Print discovered libraries (remove this line in production)
$(info Discovered libraries for $(PLATFORM): $(LIBRARIES))

# Auto-discover dependency libraries from directory structure
DEPENDENCY_BASE_DIR := $(ROOT_DIR)/dependency/library/$(PLATFORM)
DEPENDENCY_LIBRARIES := $(notdir $(wildcard $(DEPENDENCY_BASE_DIR)/*))

# Include platform-specific dependency library configurations
ifneq ($(DEPENDENCY_LIBRARIES),)
    $(foreach lib,$(DEPENDENCY_LIBRARIES),$(eval -include $(DEPENDENCY_BASE_DIR)/$(lib)/$(lib).mk))
endif

# Debug: Print discovered dependency libraries (remove this line in production)
$(info Discovered dependency libraries for $(PLATFORM): $(DEPENDENCY_LIBRARIES))

# Default target builds everything
all: static shared application

# Generate documentation using Doxygen
doxygen:
	@echo "Generating documentation..."
	@$(DOXYGEN) $(DOXYFILE)

# Create all required directories
$(BUILD_SUBDIRS):
	@echo "Creating directory: $@"
	@$(MKDIR) "$@"

# Make sure the object directories exist before compiling
$(ALL_OBJS): | $(BUILD_SUBDIRS)

# Pattern rule for C object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile Windows resource files
ifeq ($(HOST_OS),windows)
$(RESOURCE_OBJ): $(RESOURCE_FILE) | $(BUILD_DIR)
	@echo "Compiling Windows resources..."
	@$(RC) -i $< -o $@
endif

# Build static library
static: $(LIB_OBJS)
ifneq ($(strip $(LIB_OBJS)),)
	@echo "Creating static library..."
	@ar rcs $(BUILD_DIR)/$(STATIC_LIB) $(LIB_OBJS)
else
	@echo "No library sources found, skipping static library creation..."
endif

# Build shared library
shared: $(LIB_OBJS)
ifneq ($(strip $(LIB_OBJS)),)
	@echo "Creating shared library..."
	@$(CC) $(SHARED_FLAGS) -o $(BUILD_DIR)/$(SHARED_LIB) $(LIB_OBJS) $(LDFLAGS)
else
	@echo "No library sources found, skipping shared library creation..."
endif

# Generate executable names for each application target
define make_exe_name
$(if $(filter main.c,$(notdir $(1))),\
	$(if $(APPLICATION_EXE),$(APPLICATION_EXE),$(PROJECT_NAME)$(EXE_EXT)),\
	$(basename $(notdir $(1)))$(EXE_EXT))
endef
APPLICATION_EXES := $(foreach target,$(APPLICATION_TARGET_PATHS),$(call make_exe_name,$(target)))

# Generate rules for all application targets
define make_app_rule
$(call make_exe_name,$(1)): $(1:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) $(LIB_OBJS) $(if $(filter main.c,$(notdir $(1))),$(APPLICATION_SUPPORT_OBJS)) $(RESOURCE_OBJS_FOR_APPS)
	@echo "Creating application: $(strip $(call make_exe_name,$(1)))"
	@$(CC) $(1:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) $(LIB_OBJS) $(if $(filter main.c,$(notdir $(1))),$(APPLICATION_SUPPORT_OBJS)) $(RESOURCE_OBJS_FOR_APPS) -o $(BUILD_DIR)/$(strip $(call make_exe_name,$(1))) $(LDFLAGS)
endef
$(foreach target,$(APPLICATION_TARGET_PATHS),$(eval $(call make_app_rule,$(target))))

# Build all applications
application: $(APPLICATION_EXES)

# Deploy section

# Function to check if directory exists and has content
define check_dir_exists
$(if $(filter windows,$(HOST_OS)),\
	if exist "$(1)" ( dir /b "$(1)" >nul 2>nul ),\
	[ -d "$(1)" ] && [ -n "$(ls -A $(1) 2>/dev/null)" ])
endef

# Function to check if file exists
define check_file_exists
$(if $(filter windows,$(HOST_OS)),\
	if exist "$(1)",\
	[ -f "$(1)" ])
endef

# Function to create directory
define create_dir
$(if $(filter windows,$(HOST_OS)),\
	mkdir "$(1)" 2>nul || exit 0,\
	mkdir -p "$(1)")
endef

# Function to copy single file
define copy_file
$(if $(filter windows,$(HOST_OS)),\
	copy /Y "$(1)" "$(2)",\
	cp "$(1)" "$(2)")
endef

# Function to copy directory recursively
define copy_recursive
$(if $(filter windows,$(HOST_OS)),\
	xcopy /E /I /H /Y "$(1)" "$(2)",\
	cp -r $(1)/* $(2)/)
endef

# Detect operating system and define deployment paths
ifeq ($(HOST_OS),windows)
    DEPENDENCY_RESOURCE_DIR := $(ROOT_DIR)\dependency\resource
    DEPLOY_LIB_DIR ?= $(DEPLOY_BASE_DIR)\library\$(PLATFORM)
    DEPLOY_APPS_DIR ?= $(DEPLOY_BASE_DIR)\application\$(PLATFORM)
    DEPLOY_RESOURCES_DIR ?= $(DEPLOY_APPS_DIR)\resource
else
    DEPENDENCY_RESOURCE_DIR := $(ROOT_DIR)/dependency/resource
    DEPLOY_LIB_DIR ?= $(DEPLOY_BASE_DIR)/library/$(PLATFORM)
    DEPLOY_APPS_DIR ?= $(DEPLOY_BASE_DIR)/application/$(PLATFORM)
    DEPLOY_RESOURCES_DIR ?= $(DEPLOY_APPS_DIR)/resource
endif
DEPLOY_STATIC_LIB_DIR ?= $(DEPLOY_LIB_DIR)
DEPLOY_SHARED_LIB_DIR ?= $(DEPLOY_LIB_DIR)

# Deploy static library
deploy_static_lib:
	@if $(call check_file_exists,$(BUILD_DIR)/$(STATIC_LIB)); then \
		echo "Creating static library deploy directory..."; \
		$(call create_dir,$(DEPLOY_STATIC_LIB_DIR)); \
		echo "Deploying static library..."; \
		$(call copy_file,$(BUILD_DIR)/$(STATIC_LIB),$(DEPLOY_STATIC_LIB_DIR)/$(STATIC_LIB)); \
	else \
		echo "Static library $(STATIC_LIB) not found in $(BUILD_DIR), skipping static library deployment..."; \
	fi
	
# Deploy shared library
deploy_shared_lib:
	@if $(call check_file_exists,$(BUILD_DIR)/$(SHARED_LIB)); then \
		echo "Creating shared library deploy directory..."; \
		$(call create_dir,$(DEPLOY_SHARED_LIB_DIR)); \
		echo "Deploying shared library..."; \
		$(call copy_file,$(BUILD_DIR)/$(SHARED_LIB),$(DEPLOY_SHARED_LIB_DIR)/$(SHARED_LIB)); \
		if [ "$(HOST_OS)" != "windows" ] && [ -n "$(SHARED_SONAME)" ] && [ -n "$(SHARED_LINK)" ]; then \
			cd $(DEPLOY_SHARED_LIB_DIR) && \
			ln -sf $(SHARED_LIB) $(SHARED_SONAME) && \
			ln -sf $(SHARED_SONAME) $(SHARED_LINK); \
			echo "Created symlinks:"; \
			echo "  $(SHARED_LIB)"; \
			echo "  ├── $(SHARED_SONAME)"; \
			echo "  └── $(SHARED_LINK)"; \
		fi; \
	else \
		echo "Shared library $(SHARED_LIB) not found in $(BUILD_DIR), skipping shared library deployment..."; \
	fi
	
# Deploy application executables
deploy_applications:
	@if [ -n "$(APPLICATION_EXES)" ]; then \
		echo "Creating applications deploy directory..."; \
		$(call create_dir,$(DEPLOY_APPS_DIR)); \
		echo "Deploying application executables..."; \
		for exe in $(APPLICATION_EXES); do \
			if $(call check_file_exists,$(BUILD_DIR)/$$exe); then \
				echo "  Deploying $$exe..."; \
				$(call copy_file,$(BUILD_DIR)/$$exe,$(DEPLOY_APPS_DIR)/$$exe); \
			else \
				echo "  Warning: Application executable $$exe not found in $(BUILD_DIR)"; \
			fi; \
		done; \
	else \
		echo "No application executables defined, skipping application deployment..."; \
	fi

deploy_resources:
	@if $(call check_dir_exists,$(DEPENDENCY_RESOURCE_DIR)); then \
		echo "Creating deploy resources directory..."; \
		$(call create_dir,$(DEPLOY_RESOURCES_DIR)); \
		echo "Deploying resources..."; \
		$(call copy_recursive,$(DEPENDENCY_RESOURCE_DIR),$(DEPLOY_RESOURCES_DIR)); \
	else \
		echo "No dependency resources found, skipping resource deployment..."; \
	fi

# Deploy all libraries
deploy_libraries: deploy_static_lib deploy_shared_lib

# Deploy everything
deploy: deploy_libraries deploy_applications deploy_resources

# Function to remove directory quietly
define remove_dir_quiet
$(if $(filter windows,$(HOST_OS)),\
	if exist "$(1)" rmdir /S /Q "$(1)" 2>nul || exit 0,\
	rm -rf "$(1)" 2>/dev/null || true)
endef

# Function to remove file quietly
define remove_file_quiet
$(if $(filter windows,$(HOST_OS)),\
	if exist "$(1)" del /Q "$(1)" 2>nul || exit 0,\
	rm -f "$(1)" 2>/dev/null || true)
endef

# Clean build artefacts and documentation
clean:
	@echo "Cleaning build artefacts..."
	@$(call remove_dir_quiet,$(BUILD_DIR))
	@echo "Cleaning documentation..."
	@$(call remove_dir_quiet,$(ROOT_DIR)/documentation/doxygen/html)
	@$(call remove_dir_quiet,$(ROOT_DIR)/documentation/doxygen/latex)
	@$(call remove_dir_quiet,$(ROOT_DIR)/documentation/doxygen/markdown)

# Development targets
debug:
	$(MAKE) BUILD_CONFIG=Debug all

release:
	$(MAKE) BUILD_CONFIG=Release all

# Function to print list items
define print_list
$(foreach item,$(1),@echo "  $(item)"$(newline))
endef

# Function to print file list from directory (simplified for cross-platform)
define print_resource_files
$(if $(filter windows,$(HOST_OS)),\
	if exist "$(ROOT_DIR)\dependency\resource" ( \
		echo   $(ROOT_DIR)\dependency\resource ^(exists^) && \
		dir /b "$(ROOT_DIR)\dependency\resource" 2>nul | findstr /r ".*" >nul && echo     ^(files found^) || echo     ^(no files found^) \
	) else ( \
		echo   $(ROOT_DIR)\dependency\resource ^(not found^) \
	),\
	if [ -d "$(ROOT_DIR)/dependency/resource" ]; then \
		echo "  $(ROOT_DIR)/dependency/resource (exists)"; \
		find "$(ROOT_DIR)/dependency/resource" -type f 2>/dev/null | head -5 | while read file; do \
			echo "    $$file"; \
		done; \
	else \
		echo "  $(ROOT_DIR)/dependency/resource (not found)"; \
	fi)
endef

# Print configuration
print:
	@echo "Project Configuration:"
	@echo "  Name: $(PROJECT_NAME)"
	@echo "  Host OS: $(HOST_OS)"
	@echo "  Host Architecture: $(HOST_ARCH)"
	@echo "  Target OS: $(TARGET_OS)"
	@echo "  Target Architecture: $(TARGET_ARCH)"
	@echo "  Target Platform: $(PLATFORM)"
	@echo "  Build Config: $(BUILD_CONFIG)"
	@echo ""
	@echo "Paths:"
	@echo "  Build: $(BUILD_DIR)"
	@echo "  Source: $(SRC_DIR)"
	@echo "  Include: $(INCLUDE_DIR)"
	@echo "  Deploy Static Library: $(DEPLOY_STATIC_LIB_DIR)"
	@echo "  Deploy Shared Library: $(DEPLOY_SHARED_LIB_DIR)"
	@echo "  Deploy Applications: $(DEPLOY_APPS_DIR)"
	@echo "  Documentation: $(ROOT_DIR)/documentation/doxygen"
	@echo ""
	@echo "Source Directories:"
	$(call print_list,$(SRC_SUBDIRS))
	@echo ""
	@echo "Build Directories:"
	$(call print_list,$(BUILD_SUBDIRS))
	@echo ""
	@echo "Application Targets:"
	$(call print_list,$(APPLICATION_TARGETS))
	@echo ""
	@echo "Application Support Files:"
	$(call print_list,$(APPLICATION_SUPPORT_SRCS))
	@echo ""
	@echo "Application Executables:"
	$(call print_list,$(notdir $(APPLICATION_EXES)))
	@echo ""
	@echo "Library Sources:"
	$(call print_list,$(LIB_SRCS))
	@echo ""
	@echo "Resource Directory:"
	@$(call print_resource_files)
	@echo ""
	@echo "Windows Resource Files:"
ifeq ($(HOST_OS),windows)
	@if $(call check_file_exists,$(RESOURCE_FILE)); then \
		echo "  $(RESOURCE_FILE)"; \
	else \
		echo "  None found"; \
	fi
else
	@echo "  N/A (not Windows)"
endif

.PHONY: all application static shared clean debug release doxygen deploy_libraries deploy_static_lib deploy_shared_lib deploy_applications deploy_resources print