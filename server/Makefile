NAME = server

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
SRCS = context.c\
	output.c\
	run.c\
	user.c\
	utils.c\
	main.c\

SRC_INPUT = router.c

# objs
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
OBJS += $(addprefix $(OBJDIR)/, $(SRC_INPUT:.c=.o))

# compile objs
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/input/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# commands
all: $(NAME)

$(NAME): $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $(OBJS) -o $@

$(OBJDIR):
	mkdir -p $@