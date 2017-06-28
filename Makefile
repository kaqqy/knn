NAME = knn
SRC = main.c json.c
HDR = .

.PHONY: all fclean re

all: $(NAME)

$(NAME):
	gcc $(SRC) -I$(HDR) -o $(NAME)

fclean:
	rm $(NAME)

re: fclean all
