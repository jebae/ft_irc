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
	run.c\
	utils.c\
	main.c\

SRC_INPUT = router.c\
	nick.c\
	create_channel.c\
	leave.c\
	remove_channel.c\
	join.c\
	msg_to_channel.c\
	msg_to_user.c\
	where_am_i.c\
	who.c\

SRC_OUTPUT = send.c\
	ack.c\
	chat.c\

SRC_USER = add.c\
	nick.c\

SRC_CHANNEL = create.c\
	leave.c\
	remove.c\
	join.c\

# objs
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
OBJS += $(addprefix $(OBJDIR)/input_, $(SRC_INPUT:.c=.o))
OBJS += $(addprefix $(OBJDIR)/output_, $(SRC_OUTPUT:.c=.o))
OBJS += $(addprefix $(OBJDIR)/user_, $(SRC_USER:.c=.o))
OBJS += $(addprefix $(OBJDIR)/channel_, $(SRC_CHANNEL:.c=.o))

# compile objs
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/input_%.o: $(SRCDIR)/input/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/output_%.o: $(SRCDIR)/output/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/user_%.o: $(SRCDIR)/user/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/channel_%.o: $(SRCDIR)/channel/%.c
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
		$(filter-out ./srcs/main.c, $(wildcard $(SRCDIR)/**/*.c) $(wildcard $(SRCDIR)/*.c)) $(SRCDIR)/*.test.cpp $(SRCDIR)/**/*.test.cpp\
		-o $@

.PHONY: all test