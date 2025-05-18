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

# ----------------------------------- Flags ------------------------------------
CPPFLAGS = -I include
DEBUG_FLAGS = -g
WARNING_FLAGS = -pedantic-errors -Wall -Wextra
SO_FLAGS = -shared -fPIC
CPP_20 = -std=c++20
CC = g++ $(CPP_20)

# Linker flags for Handleton
LINK_HANDLETON_SO = -L$(SO_DEBUG_PATH) -lhandleton -Wl,-rpath=$(SO_DEBUG_PATH)

# ------------------------------ General Variables -----------------------------
# 🔹 Source files for Handleton (Framework's `libhandleton.so`)
FRAMEWORK_HANDLETON_SRCS = \
	src/handleton.cpp \
	src/logger.cpp \
	src/thread_pool.cpp \
	src/scheduler.cpp \
	src/cmd.cpp

FRAMEWORK_HANDLETON_OBJS = $(FRAMEWORK_HANDLETON_SRCS:src/%.cpp=$(O_DEBUG_PATH)/%.o)

# 🔹 Object files for Framework
ALL_OBJS = \
	$(O_DEBUG_PATH)/dispatcher.o \
	$(O_DEBUG_PATH)/dir_monitor.o \
	$(O_DEBUG_PATH)/dll_loader.o \
	$(O_DEBUG_PATH)/mediator.o \
	$(O_DEBUG_PATH)/reactor.o \
	$(O_DEBUG_PATH)/framework.o \
	$(O_DEBUG_PATH)/input_proxy.o \
	$(O_DEBUG_PATH)/key_args.o \
	$(O_DEBUG_PATH)/cmd.o \
	$(O_DEBUG_PATH)/reactor_listener_select.o \
	$(O_DEBUG_PATH)/async_injection.o \
	$(O_DEBUG_PATH)/scheduler.o

# =================================== Rules ====================================
# ✅ Default rule
.PHONY: all
all: framework

# ✅ Build Framework (Ensure `libhandleton.so` exists first)
.PHONY: framework
framework: $(SO_DEBUG_PATH)/libhandleton.so $(SO_DEBUG_PATH)/libframework.so
	@echo "$(GREEN)Framework build complete.$(NC)"

# ✅ Build Handleton (`libhandleton.so`)
$(SO_DEBUG_PATH)/libhandleton.so: $(FRAMEWORK_HANDLETON_OBJS) | dirs
	@echo "$(CYAN)Building Framework's libhandleton.so$(NC)"
	@$(CC) $(SO_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@
	@echo "$(GREEN)Framework's libhandleton.so built successfully.$(NC)"

# ✅ Build Framework Shared Library (`libframework.so`)
$(SO_DEBUG_PATH)/libframework.so: $(ALL_OBJS) $(SO_DEBUG_PATH)/libhandleton.so | dirs
	@echo "$(CYAN)Building libframework.so$(NC)"
	@$(CC) $(SO_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $^ -o $@ -L$(SO_DEBUG_PATH) -lhandleton
	@echo "$(GREEN)libframework.so built successfully.$(NC)"

# ✅ Build object files
$(O_DEBUG_PATH)/%.o: src/%.cpp | dirs
	@echo "$(CYAN)Building $(@F)...$(NC)"
	@$(CC) -c $< -o $@ $(WARNING_FLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(SO_FLAGS)

# ✅ Clean rule
.PHONY: clean
clean:
	@echo "$(RED)Cleaning Framework...$(NC)"
	@rm -rf bin/debug bin/release exe
	@echo "$(GREEN)Framework clean complete.$(NC)"

# ✅ Create necessary directories
.PHONY: dirs
dirs:
	@mkdir -p $(SO_DEBUG_PATH) $(O_DEBUG_PATH) $(SO_RELEASE_PATH) $(O_RELEASE_PATH) $(EXE_PATH)



# SPECIFIC RULES FOR ANY REASON:
#_______________________________________________________________________________

# ----------------------------- Build test executables -------------------------
#exe/%.out: test/%.cpp $(ALL_OBJS) $(SO_DEBUG_PATH)/libhandleton.so | dirs
#	@echo "$(CYAN)Building $(@F)...$(NC)"
#	@$(CC) $(ALL_OBJS) $< $(TEST_FLAGS) $(LINK_HANDLETON_SO) -o $@
#	@echo "$(GREEN)$(@F) built successfully.$(NC)"

# Test rules for individual components
#logger: exe/logger.out 
#thread_pool: exe/thread_pool.out
#dispatcher: exe/dispatcher.out
#factory: exe/factory.out
#dir_monitor: exe/dir_monitor.out
#dll_loader: exe/dll_loader.out
#reactor: exe/reactor.out
#scheduler: exe/scheduler.out
#framework: exe/framework.out
#
# Framework-specific rule
#exe/framework.out: test/framework_test.cpp $(ALL_OBJS) $(SO_DEBUG_PATH)/libhandleton.so
#	@echo "$(CYAN)Building framework_test.cpp$(NC)"
#	@$(CC) $^ $(TEST_FLAGS) $(LINK_HANDLETON_SO) -o $@
#	@echo "$(GREEN)-----> 'exe/framework.out' is ready to execute <-----$(NC)"
#
## Scheduler-specific rule
#exe/scheduler.out: test/scheduler_test.cpp $(ALL_OBJS) $(SO_DEBUG_PATH)/libhandleton.so
#	@echo "$(CYAN)Building scheduler_test.cpp$(NC)"
#	@$(CC) $^ $(TEST_FLAGS) $(LINK_HANDLETON_SO) -o $@
#	@echo "$(GREEN)-----> 'exe/scheduler.out' is ready to execute <-----$(NC)"
#_______________________________________________________________________________