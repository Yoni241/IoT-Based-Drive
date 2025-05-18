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

# ✅ Ensure `libhandleton.so` (Minions) is linked first
LD_FLAGS = -L$(COMMUNICATION_SO_PATH) -lcommunication \
           -L$(FRAMEWORK_SO_PATH) -lframework \
           -L$(SO_DEBUG_PATH) -lhandleton \
           -Wl,-rpath=$(COMMUNICATION_SO_PATH):$(FRAMEWORK_SO_PATH):$(SO_DEBUG_PATH)

# ------------------------------ General Variables -----------------------------
# ✅ Handleton (Minions) Source Files
HANDLETON_SRCS = src/handleton.cpp

# ✅ Handleton (Minions) Object Files (Compiled with -fPIC)
HANDLETON_OBJS = $(HANDLETON_SRCS:src/%.cpp=$(O_DEBUG_PATH)/%.o)

# ✅ Main test files (providing `main()`)
MINION_TEST_SRCS = test/minion_concrete_1_test.cpp \
                   test/minion_concrete_2_test.cpp \
                   test/minion_concrete_3_test.cpp

MINION_TEST_OBJS = $(MINION_TEST_SRCS:test/%.cpp=$(O_DEBUG_PATH)/%.o)

# ✅ Minion Source Files (Without main)
MINION_SRCS = src/master_udp.cpp \
              src/master_key_args.cpp \
              src/read_cmd_minion.cpp \
              src/write_cmd_minion.cpp \
              src/minion_concrete.cpp

# ✅ Minion Object Files (Compiled with -fPIC)
MINION_OBJS = $(MINION_SRCS:src/%.cpp=$(O_DEBUG_PATH)/%.o)

# =================================== Rules ====================================
.PHONY: all
all: communication framework handleton minion1 minion2 minion3

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

# ✅ Ensure `handleton` (Minions) is built before Minion
.PHONY: handleton
handleton: $(SO_DEBUG_PATH)/libhandleton.so
	@echo "$(GREEN)Minion Handleton built successfully.$(NC)"

# ✅ Minion Tests
.PHONY: minion1 minion2 minion3

minion1: framework handleton $(EXE_PATH)/minion_concrete_1_test.out
	@echo "$(GREEN)Minion Test 1 build complete.$(NC)"

minion2: framework handleton $(EXE_PATH)/minion_concrete_2_test.out
	@echo "$(GREEN)Minion Test 2 build complete.$(NC)"

minion3: framework handleton $(EXE_PATH)/minion_concrete_3_test.out
	@echo "$(GREEN)Minion Test 3 build complete.$(NC)"

# ----------------- Clean rules -----------------
.PHONY: clean
clean:
	@echo "$(RED)Cleaning Minion...$(NC)"
	@rm -rf $(O_DEBUG_PATH) $(SO_DEBUG_PATH) $(EXE_PATH)
	@echo "$(GREEN)Minion clean complete.$(NC)"

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

# -------------------- Build `libhandleton.so` for Minions ------------------
$(SO_DEBUG_PATH)/libhandleton.so: $(HANDLETON_OBJS) | dirs
	@echo "$(CYAN)Building Minion's libhandleton.so$(NC)"
	@$(CC) $(SO_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@
	@echo "$(GREEN)Minion's libhandleton.so built successfully.$(NC)"

# -------------------- Build Minion Test Executables ------------------
$(EXE_PATH)/minion_concrete_1_test.out: $(MINION_OBJS) $(O_DEBUG_PATH)/minion_concrete_1_test.o $(SO_DEBUG_PATH)/libhandleton.so | dirs
	@echo "$(CYAN)Building Minion Test 1 Executable$(NC)"
	@$(CC) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@ $(LD_FLAGS)
	@echo "$(GREEN)Minion Test 1 executable built successfully.$(NC)"

$(EXE_PATH)/minion_concrete_2_test.out: $(MINION_OBJS) $(O_DEBUG_PATH)/minion_concrete_2_test.o $(SO_DEBUG_PATH)/libhandleton.so | dirs
	@echo "$(CYAN)Building Minion Test 2 Executable$(NC)"
	@$(CC) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@ $(LD_FLAGS)
	@echo "$(GREEN)Minion Test 2 executable built successfully.$(NC)"

$(EXE_PATH)/minion_concrete_3_test.out: $(MINION_OBJS) $(O_DEBUG_PATH)/minion_concrete_3_test.o $(SO_DEBUG_PATH)/libhandleton.so | dirs
	@echo "$(CYAN)Building Minion Test 3 Executable$(NC)"
	@$(CC) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@ $(LD_FLAGS)
	@echo "$(GREEN)Minion Test 3 executable built successfully.$(NC)"

# -------------------- Build object files (With -fPIC for Shared Libs) ----------------------
$(O_DEBUG_PATH)/%.o: src/%.cpp | dirs
	@echo "$(CYAN)Building C++ Object File: $(@F)...$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(SO_FLAGS)

# -------------------- Build test main object files (With -fPIC) ----------------------
$(O_DEBUG_PATH)/%.o: test/%.cpp | dirs
	@echo "$(CYAN)Building Test Object File: $(@F)...$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(SO_FLAGS)

# Create necessary directories
.PHONY: dirs
dirs:
	@mkdir -p $(SO_DEBUG_PATH) $(O_DEBUG_PATH) $(SO_RELEASE_PATH) $(O_RELEASE_PATH) $(EXE_PATH)
