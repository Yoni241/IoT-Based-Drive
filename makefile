# ----------------------------------- Colors -----------------------------------
LIGHT_GREEN = \033[1;32m
GREEN = \033[0;32m
RED = \033[0;31m
CYAN = \033[0;36m
PURPLE = \033[0;35m
NC = \033[0m

# ----------------------------------- Paths ------------------------------------
FRAMEWORK_DIR = framework
MASTER_DIR = concrete/master
MINIONS_DIR = concrete/minions
COMMUNICATION_DIR = concrete/communication

# Default target
.PHONY: all
all: framework communication minions 

# Run framework's Makefile
.PHONY: framework
framework:
	@echo "$(CYAN)Building framework...$(NC)"
	@$(MAKE) -C $(FRAMEWORK_DIR)
	@echo "$(GREEN)Framework build complete.$(NC)"

# Run concrete's Makefile (even if it's incomplete)
.PHONY: minions
concrete:
	@echo "$(CYAN)Building minions...$(NC)"
	@if [ -f "$(CONCRETE_MINIONS_DIR)/Makefile" ]; then \
		$(MAKE) -C $(CONCRETE_MINIONS_DIR); \
	else \
		echo "$(RED)Skipping concrete build: Makefile not found.$(NC)"; \
	fi

# Clean both projects
.PHONY: clean
clean:
	@echo "$(RED)Cleaning all projects...$(NC)"
	@$(MAKE) -C $(FRAMEWORK_DIR) clean
	@if [ -f "$(CONCRETE_DIR)/Makefile" ]; then \
		$(MAKE) -C $(CONCRETE_DIR) clean; \
	else \
		echo "$(RED)Skipping concrete clean: Makefile not found.$(NC)"; \
	fi
	@echo "$(GREEN)Clean complete.$(NC)"
