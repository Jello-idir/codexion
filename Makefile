NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/main.c \
	src/simulation.c \
	src/startup.c \
	src/time.c \
	src/logging.c \
	src/monitor.c \
	src/coder.c \
	src/dongle.c \
	src/dongle_acquire.c \
	src/dongle_wait.c \
	src/init.c \
	src/init_coders_dongles.c \
	src/parse.c \
	src/heap.c \
	src/cleanup.c
OBJ = $(SRC:.c=.o)
HEADERS = header/codexion.h

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
