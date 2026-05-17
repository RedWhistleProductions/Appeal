# Compiler
CXX ?= g++

# Compiler flags
CXXFLAGS = -g -Wall -pthread
LDFLAGS =
LDLIBS =

# Directories
SRC_DIR = ./Source
OBJ_DIR = ./Obj
BIN_DIR = .
MODULES_DIR = ./Modules
PLUGINS_DIR = ./Plugins

MODULE_MAKEFILES = $(wildcard $(MODULES_DIR)/*/Makefile)
PLUGIN_MAKEFILES = $(wildcard $(PLUGINS_DIR)/*/*/Makefile)
MODULE_DIRS = $(sort $(dir $(MODULE_MAKEFILES)))
PLUGIN_DIRS = $(sort $(dir $(PLUGIN_MAKEFILES)))
MODULE_BUILD_TARGETS = $(foreach dir,$(MODULE_DIRS),build-$(subst /,_,$(dir)))
PLUGIN_BUILD_TARGETS = $(foreach dir,$(PLUGIN_DIRS),build-$(subst /,_,$(dir)))
MODULE_INSTALL_TARGETS = $(foreach dir,$(MODULE_DIRS),install-$(subst /,_,$(dir)))
PLUGIN_INSTALL_TARGETS = $(foreach dir,$(PLUGIN_DIRS),install-$(subst /,_,$(dir)))
MODULE_CLEAN_TARGETS = $(foreach dir,$(MODULE_DIRS),clean-$(subst /,_,$(dir)))
PLUGIN_CLEAN_TARGETS = $(foreach dir,$(PLUGIN_DIRS),clean-$(subst /,_,$(dir)))

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Executable
ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
else
    EXE_EXT =
endif
TARGET = $(BIN_DIR)/Appeal$(EXE_EXT)

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q
    MKDIR = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
else
    RM = rm -rf
    MKDIR = mkdir -p $(OBJ_DIR)
endif

# Default target
all: $(TARGET) modules plugins

modules: $(MODULE_BUILD_TARGETS)

plugins: $(PLUGIN_BUILD_TARGETS)

install-modules: $(MODULE_INSTALL_TARGETS)

install-plugins: $(PLUGIN_INSTALL_TARGETS)

install: $(TARGET) install-modules install-plugins

# Link object files to create the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up
clean: $(MODULE_CLEAN_TARGETS) $(PLUGIN_CLEAN_TARGETS)
	rm -rf $(OBJ_DIR) $(TARGET)

define BUILD_TARGET
.PHONY: build-$(subst /,_,$(1))
build-$(subst /,_,$(1)):
	$$(MAKE) -C $(1)
endef

define INSTALL_TARGET
.PHONY: install-$(subst /,_,$(1))
install-$(subst /,_,$(1)):
	$$(MAKE) -C $(1) install
endef

define CLEAN_TARGET
.PHONY: clean-$(subst /,_,$(1))
clean-$(subst /,_,$(1)):
	$$(MAKE) -C $(1) clean
endef

$(foreach dir,$(MODULE_DIRS) $(PLUGIN_DIRS),$(eval $(call BUILD_TARGET,$(dir))))
$(foreach dir,$(MODULE_DIRS) $(PLUGIN_DIRS),$(eval $(call INSTALL_TARGET,$(dir))))
$(foreach dir,$(MODULE_DIRS) $(PLUGIN_DIRS),$(eval $(call CLEAN_TARGET,$(dir))))

.PHONY: all modules plugins install-modules install-plugins install clean \
	$(MODULE_BUILD_TARGETS) $(PLUGIN_BUILD_TARGETS) \
	$(MODULE_INSTALL_TARGETS) $(PLUGIN_INSTALL_TARGETS) \
	$(MODULE_CLEAN_TARGETS) $(PLUGIN_CLEAN_TARGETS)
