# ----------------------
NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC =
OBJ = $(SRC:.c=.o)
HEADERS =
# ----------------------

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re



# SRC_BONUS =
# OBJ_BONUS = $(SRC_BONUS:.c=.o)

# bonus: $(BONUS)

# $(BONUS): $(OBJ_BONUS)
# 	$(CC) $(CFLAGS) $(OBJ_BONUS) -o $(BONUS)

# %_bonus.o: %_bonus.c
# 	$(CC) $(CFLAGS) -c $< -o $@
