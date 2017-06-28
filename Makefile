NAME = knn
SRC = main.c

.PHONY: all fclean re

all: $(NAME)

$(NAME):
	gcc $(SRC) -o $(NAME)

fclean:
	rm $(NAME)

re: fclean all
