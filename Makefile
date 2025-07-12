# Silent mode: don’t echo commands
MAKEFLAGS += -s

# -----------------------------------------------------------------------------
# Paths & tools
# -----------------------------------------------------------------------------
LIBFTDIR     := Libft/Libft
LIBFTNAME    := libft.a
NAME         := libft.a

SRCDIR       := src
DEBUGDIR     := testDebug
ANIM_SCRIPT  := $(DEBUGDIR)/proc_comm.sh

CC           := cc
CFLAGS       := -Wall -Wextra -Werror -fsanitize=undefined

# -----------------------------------------------------------------------------
# Source lists
# -----------------------------------------------------------------------------
# “Mandatory” client/server
CLIENT_SRCS     := $(SRCDIR)/client.c     $(SRCDIR)/sign_utils.c
SERVER_SRCS     := $(SRCDIR)/server.c     $(SRCDIR)/sign_utils.c

# “Bonus” client/server
CLIENT_BONUS_SRCS := $(SRCDIR)/client_bonus.c  $(SRCDIR)/sign_utils_bonus.c
SERVER_BONUS_SRCS := $(SRCDIR)/server_bonus.c  $(SRCDIR)/sign_utils_bonus.c

# ARQ client/server
CLIENT_ARQ_SRCS  := $(SRCDIR)/client_arq.c  \
                    $(SRCDIR)/sign_utils.c  \
                    $(SRCDIR)/queue.c
SERVER_ARQ_SRCS  := $(SRCDIR)/server_arq.c  \
                    $(SRCDIR)/sign_utils.c  \
					$(SRCDIR)/queue.c

# Convert .c → .o
CLIENT_OBJS        := $(CLIENT_SRCS:.c=.o)
SERVER_OBJS        := $(SERVER_SRCS:.c=.o)
BONUS_CLIENT_OBJS  := $(CLIENT_BONUS_SRCS:.c=.o)
BONUS_SERVER_OBJS  := $(SERVER_BONUS_SRCS:.c=.o)
CLIENT_ARQ_OBJS    := $(CLIENT_ARQ_SRCS:.c=.o)
SERVER_ARQ_OBJS    := $(SERVER_ARQ_SRCS:.c=.o)

# -----------------------------------------------------------------------------
# Colors
# -----------------------------------------------------------------------------
GREEN  := \033[0;32m
YELLOW := \033[1;33m
RED    := \033[0;31m
NC     := \033[0m

# -----------------------------------------------------------------------------
# Phony targets
# -----------------------------------------------------------------------------
.PHONY: all mandatory bonus animation arq clean fclean re

# Default
all: mandatory

# -----------------------------------------------------------------------------
# Build or copy libftprintf
# -----------------------------------------------------------------------------
$(NAME): $(LIBFTDIR)/$(LIBFTNAME)
	@cp $< $@

$(LIBFTDIR)/$(LIBFTNAME):
	@$(MAKE) -C $(LIBFTDIR)

# -----------------------------------------------------------------------------
# Mandatory 42client / 42server
# -----------------------------------------------------------------------------
mandatory: $(NAME) $(CLIENT_OBJS) $(SERVER_OBJS)
	@echo "# $(GREEN)Building mandatory...$(NC)"
	@$(CC) $(CFLAGS) -o 42client $(CLIENT_OBJS) $(NAME)
	@$(CC) $(CFLAGS) -o 42server $(SERVER_OBJS) $(NAME)
	@echo "$(GREEN)Done!$(NC)"

# -----------------------------------------------------------------------------
# Bonus 42client / 42server
# -----------------------------------------------------------------------------
bonus: $(NAME) $(BONUS_CLIENT_OBJS) $(BONUS_SERVER_OBJS)
	@echo "# $(GREEN)Building bonus...$(NC)"
	@$(CC) $(CFLAGS) -o 42client $(BONUS_CLIENT_OBJS) $(NAME)
	@$(CC) $(CFLAGS) -o 42server $(BONUS_SERVER_OBJS) $(NAME)
	@echo "$(GREEN)Done!$(NC)"

# -----------------------------------------------------------------------------
# ASCII animation (unchanged)
# -----------------------------------------------------------------------------
animation: $(ANIM_SCRIPT)
	@echo "# $(GREEN)Starting ASCII animation... (Ctrl+C to stop)$(NC)"
	@chmod +x $(ANIM_SCRIPT)
	@./$(ANIM_SCRIPT)

# -----------------------------------------------------------------------------
# ARQ build
# -----------------------------------------------------------------------------
arq: $(NAME) $(CLIENT_ARQ_OBJS) $(SERVER_ARQ_OBJS)
	@echo "# $(GREEN)Building ARQ version...$(NC)"
	@$(CC) $(CFLAGS) -o 42client_arq $(CLIENT_ARQ_OBJS) $(NAME)
	@$(CC) $(CFLAGS) -o 42server_arq $(SERVER_ARQ_OBJS) $(NAME)
	@echo "$(GREEN)ARQ build done!$(NC)"

# -----------------------------------------------------------------------------
# Generic compile rule
# -----------------------------------------------------------------------------
%.o: %.c
	@echo "Compiling $< → $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------
# Clean / full clean
# -----------------------------------------------------------------------------
clean:
	@rm -f $(CLIENT_OBJS) $(SERVER_OBJS)        \
	       $(BONUS_CLIENT_OBJS) $(BONUS_SERVER_OBJS) \
	       $(CLIENT_ARQ_OBJS) $(SERVER_ARQ_OBJS)
	@$(MAKE) -C $(LIBFTDIR) clean
	@echo "# $(YELLOW)Cleaning objects...$(NC)"

fclean: clean
	@rm -f $(NAME) 42client 42server         \
	       42client_arq 42server_arq
	@$(MAKE) -C $(LIBFTDIR) fclean
	@echo "# $(RED)Fully cleaned!$(NC)"

# -----------------------------------------------------------------------------
# Rebuild everything
# -----------------------------------------------------------------------------
re: fclean all

