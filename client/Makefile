NAME = client

# path
SRCDIR = ./srcs
INCDIR = ./includes
LIBDIR = ../libs
OBJDIR = ./objs
LIBFT = $(LIBDIR)/libft

# compiler option
CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I ./$(INCDIR) -I ../includes -I $(LIBFT)/includes
LIBS = -L $(LIBFT) -lft

# srcs
SRCS = run.c\
	utils.c\
	main.c\
	input.c\

SRC_OUTPUT = send.c\
	parse.c\
	parse_chat.c\

# objs
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
OBJS += $(addprefix $(OBJDIR)/, $(SRC_OUTPUT:.c=.o))

# compile objs
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/output/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# commands
all: $(NAME)

$(NAME): $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $(OBJS) -o $@

$(OBJDIR):
	mkdir -p $@

def = ''

test:
	$(MAKE) -C $(LIBFT)
	g++\
		-Wall -Wextra -std=c++11\
		$(def)\
		-lgtest\
		$(LIBS)\
		$(INCLUDES)\
		$(filter-out ./srcs/main.c, $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/output/*.c)) $(SRCDIR)/*.test.cpp $(SRCDIR)/**/*.test.cpp\
		-o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re:	fclean all

.PHONY: all test clean fclean re