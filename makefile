NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/main.c \
	src/simulation.c \
	src/time.c \
	src/logging.c \
	src/monitor.c \
	src/coder.c \
	src/dongle.c \
	src/dongle_wait.c \
	src/init.c \
	src/init_coders_dongles.c \
	src/parse.c \
	src/heap.c \
	src/cleanup.c
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


.PHONY: all clean fclean re run

number_of_coders				= 2
time_to_burnout					= 1003
time_to_compile					= 500
time_to_debug					= 1
time_to_refactor				= 1
number_of_compiles_required 	= 10
dongle_cooldown					= 0
scheduler						= "fifo"

run: $(NAME)
	@./$(NAME)\
	 $(number_of_coders)\
	 $(time_to_burnout)\
	 $(time_to_compile)\
	 $(time_to_debug)\
	 $(time_to_refactor)\
	 $(number_of_compiles_required) \
	 $(dongle_cooldown)\
	 $(scheduler)
