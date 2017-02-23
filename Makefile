CC	=	g++

RM	=	rm -f

NAME	=	eegreader

SRC	=	src/eegreader.c

OBJ	=	$(SRC:.c=.o)

CFLAGS 	+=	-W -Wall -Wextra -lpython2.7 -lpthread -I include -std=c++11

all:	$(NAME)

$(NAME):	$(OBJ)
		$(CC) -o $(NAME) $(OBJ) $(CFLAGS)

clean:
		$(RM) $(OBJ)

fclean:		clean
		$(RM) $(NAME)

re:		fclean all
