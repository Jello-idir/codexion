# ----------------------
NAME = codexion

CC = cc
CFLAGS = 

SRC = src/main.c src/init.c src/debug.c
OBJ = $(SRC:.c=.o)
HEADERS = header/codexion.h
# ----------------------

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all


.PHONY: all clean fclean re

number_of_coders				= 2
time_to_burnout					= 800
time_to_compile					= 200
time_to_debug					= 100
time_to_refactor				= 300
number_of_compiles_required 	= 20
dongle_cooldown					= 100
scheduler						= "fifo"

run: $(NAME)
	@./$(NAME) $(number_of_coders) $(time_to_burnout) $(time_to_compile) $(time_to_debug) $(time_to_refactor) $(number_of_compiles_required) $(dongle_cooldown) $(scheduler)

# run: $(NAME)
# 	./$(NAME)\
# 	 $(number_of_coders)\
# 	 $(time_to_burnout)\
# 	 $(time_to_compile)\
# 	 $(time_to_debug)\
# 	 $(time_to_refactor)\
# 	 $(number_of_compiles_required) \
# 	 $(dongle_cooldown)\
# 	 $(scheduler)

# SRC_BONUS =
# OBJ_BONUS = $(SRC_BONUS:.c=.o)

# bonus: $(BONUS)

# $(BONUS): $(OBJ_BONUS)
# 	$(CC) $(CFLAGS) $(OBJ_BONUS) -o $(BONUS)

# %_bonus.o: %_bonus.c
# 	$(CC) $(CFLAGS) -c $< -o $@
