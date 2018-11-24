LIBFLAG = $(shell pkg-config  allegro --libs --cflags)

NAME=jogo-vida

default: $(NAME)

$(NAME) : $(NAME).c
		cc $(NAME).c -o $(NAME) $(LIBFLAG)

clean:
	 rm -f $(NAME)
