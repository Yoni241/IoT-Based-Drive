# ---------------- Colors ----------------
LIGHT_GREEN = \033[1;32m
GREEN = \033[0;32m
RED = \033[0;31m
CYAN = \033[0;36m
PURPLE = \033[0;35m
NC = \033[0m

# ---------------- Paths ----------------
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin/debug
SO_DIR = $(BIN_DIR)/so
O_DIR = $(BIN_DIR)/o
LIB_NAME = libcommunication.so

# ---------------- Flags ----------------
CPPFLAGS = -I$(INCLUDE_DIR)
DEBUG_FLAGS = -g
WARNING_FLAGS = -pedantic-errors -Wall -Wextra
SO_FLAGS = -shared -fPIC
CPP_20 = -std=c++20
CC = g++ $(CPP_20)  # Compiler for C++ files
CC_C = gcc  # Compiler for C files

# ---------------- Source & Object Files ----------------
CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)  # C++ Source files
C_SRCS = $(wildcard $(SRC_DIR)/*.c)      # C Source files

CPP_OBJS = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(O_DIR)/%.o)  # C++ Object files
C_OBJS = $(C_SRCS:$(SRC_DIR)/%.c=$(O_DIR)/%.o)        # C Object files

ALL_OBJS = $(CPP_OBJS) $(C_OBJS)

# =================================== Rules ====================================
.PHONY: all
all: $(SO_DIR)/$(LIB_NAME)
	@echo "$(GREEN)Communication shared library built successfully!$(NC)"

# ✅ Build shared library (libcommunication.so)
$(SO_DIR)/$(LIB_NAME): $(ALL_OBJS) | dirs
	@echo "$(CYAN)Building libcommunication.so$(NC)"
	@$(CC) $(SO_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(ALL_OBJS) -o $@
	@echo "$(GREEN)libcommunication.so built successfully!$(NC)"

# ✅ Build C++ object files
$(O_DIR)/%.o: $(SRC_DIR)/%.cpp | dirs
	@echo "$(CYAN)Compiling C++ file: $(@F)...$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) -fPIC

# ✅ Build C object files
$(O_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	@echo "$(CYAN)Compiling C file: $(@F)...$(NC)"
	@$(CC_C) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) -fPIC

# ✅ Create necessary directories
.PHONY: dirs
dirs:
	@mkdir -p $(O_DIR) $(SO_DIR)

# ✅ Clean rule
.PHONY: clean
clean:
	@echo "$(RED)Cleaning Communication...$(NC)"
	@rm -rf $(O_DIR)
	@echo "$(GREEN)Communication clean complete!$(NC)"
