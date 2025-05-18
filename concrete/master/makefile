# ----------------------------------- Colors -----------------------------------
LIGHT_GREEN = \033[1;32m
GREEN = \033[0;32m
RED = \033[0;31m
CYAN = \033[0;36m
PURPLE = \033[0;35m
NC = \033[0m

# ----------------------------------- Paths ------------------------------------
SO_DEBUG_PATH = bin/debug/so
O_DEBUG_PATH = bin/debug/o
SO_RELEASE_PATH = bin/release/so
O_RELEASE_PATH = bin/release/o
EXE_PATH = exe
FRAMEWORK_SO_PATH = ../../framework/bin/debug/so
COMMUNICATION_SO_PATH = ../communication/bin/debug/so

# ----------------------------------- Flags ------------------------------------
CPPFLAGS = -I include -I ../communication/include -I ../../framework/include
DEBUG_FLAGS = -g
WARNING_FLAGS = -pedantic-errors -Wall -Wextra
SO_FLAGS = -shared -fPIC  # Ensure all objects are compiled with -fPIC
CPP_20 = -std=c++20
CC = g++ $(CPP_20)

# ✅ Ensure `libhandleton.so` is linked first
LD_FLAGS = -L$(COMMUNICATION_SO_PATH) -lcommunication \
           -L$(FRAMEWORK_SO_PATH) -lframework \
           -L$(SO_DEBUG_PATH) -lhandleton \
           -Wl,-rpath=$(COMMUNICATION_SO_PATH):$(FRAMEWORK_SO_PATH):$(SO_DEBUG_PATH)

# ------------------------------ General Variables -----------------------------
# ✅ Handleton Source Files
# ✅ Handleton (Minions) Source Files
HANDLETON_SRCS = src/handleton.cpp

# ✅ Handleton (Minions) Object Files (Compiled with -fPIC)
HANDLETON_OBJS = $(HANDLETON_SRCS:src/%.cpp=$(O_DEBUG_PATH)/%.o)


# ==============================================================================
# ✅ Master Source Files
MASTER_SRCS = src/master_concrete.cpp \
			  src/minion_udp.cpp \
			  src/nbd_key_args.cpp \
              src/nbd_proxy.cpp \
			  src/raid_manager.cpp \
			  src/response_udp.cpp \
			  src/read_cmd.cpp \
              src/response_manager.cpp \
			  src/write_cmd.cpp

MASTER_OBJS = $(MASTER_SRCS:src/%.cpp=$(O_DEBUG_PATH)/%.o)
# ==============================================================================


# ✅ Main Test File (providing `main()`)
MAIN_SRC = test/master_concrete_test.cpp
MAIN_OBJ = $(MAIN_SRC:test/%.cpp=$(O_DEBUG_PATH)/%.o)

# ✅ All object files for Master (Compiled with -fPIC)
ALL_OBJS = $(MASTER_OBJS) $(MAIN_OBJ)

# =================================== Rules ====================================
.PHONY: all
all: communication framework handleton master

# ✅ Ensure `communication` is built first
.PHONY: communication
communication:
	@echo "$(CYAN)Building communication...$(NC)"
	@echo "$(PURPLE)Entering directory '../communication'$(NC)"
	@$(MAKE) -C ../communication --no-print-directory
	@echo "$(PURPLE)Leaving directory '../communication'$(NC)"
	@echo "$(GREEN)Communication built successfully.$(NC)"

# ✅ Ensure `framework` is built after `communication`
.PHONY: framework
framework: communication
	@echo "$(CYAN)Building framework...$(NC)"
	@echo "$(PURPLE)Entering directory '../../framework'$(NC)"
	@$(MAKE) -C ../../framework --no-print-directory
	@echo "$(PURPLE)Leaving directory '../../framework'$(NC)"
	@echo "$(GREEN)Framework built successfully.$(NC)"

# ✅ Ensure `handleton` is built before Master
.PHONY: handleton
handleton: $(SO_DEBUG_PATH)/libhandleton.so
	@echo "$(GREEN)Handleton built successfully.$(NC)"

# ✅ Master Build
.PHONY: master
master: framework handleton $(EXE_PATH)/master.out
	@echo "$(GREEN)Master build complete.$(NC)"

# ----------------- Clean rules -----------------
.PHONY: clean
clean:
	@echo "$(RED)Cleaning Master...$(NC)"
	@rm -rf $(O_DEBUG_PATH) $(SO_DEBUG_PATH) $(EXE_PATH)
	@echo "$(GREEN)Master clean complete.$(NC)"

.PHONY: clean_framework
clean_framework:
	@echo "$(RED)Cleaning Framework...$(NC)"
	@echo "$(PURPLE)Entering directory '../../framework'$(NC)"
	@$(MAKE) -C ../../framework clean --no-print-directory
	@echo "$(PURPLE)Leaving directory '../../framework'$(NC)"
	@echo "$(GREEN)Framework clean complete.$(NC)"

.PHONY: clean_communication
clean_communication:
	@echo "$(RED)Cleaning Communication...$(NC)"
	@echo "$(PURPLE)Entering directory '../communication'$(NC)"
	@$(MAKE) -C ../communication clean --no-print-directory
	@echo "$(PURPLE)Leaving directory '../communication'$(NC)"
	@echo "$(GREEN)Communication clean complete.$(NC)"

# -------------------- Build `libhandleton.so` ------------------
$(SO_DEBUG_PATH)/libhandleton.so: $(HANDLETON_OBJS) | dirs
	@echo "$(CYAN)Building libhandleton.so$(NC)"
	@$(CC) $(SO_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@
	@echo "$(GREEN)libhandleton.so built successfully.$(NC)"

# -------------------- Build Master Executable ------------------
$(EXE_PATH)/master.out: $(ALL_OBJS) $(SO_DEBUG_PATH)/libhandleton.so | dirs
	@echo "$(CYAN)Building Master.out$(NC)"
	@$(CC) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@ $(LD_FLAGS)
	@echo "$(GREEN)Master executable built successfully.$(NC)"

# -------------------- Build object files (With -fPIC for Shared Libs) ----------------------
$(O_DEBUG_PATH)/%.o: src/%.cpp | dirs
	@echo "$(CYAN)Building C++ Object File: $(@F)...$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(SO_FLAGS)

# -------------------- Build test main object file (With -fPIC) ----------------------
$(O_DEBUG_PATH)/master_concrete_test.o: test/master_concrete_test.cpp | dirs
	@echo "$(CYAN)Building master_concrete_test.o$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(SO_FLAGS)

# Create necessary directories
.PHONY: dirs
dirs:
	@mkdir -p $(SO_DEBUG_PATH) $(O_DEBUG_PATH) $(SO_RELEASE_PATH) $(O_RELEASE_PATH) $(EXE_PATH)
