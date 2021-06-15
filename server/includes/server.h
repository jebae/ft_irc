#ifndef SERVER_H
# define SERVER_H

# include "libft.h"
# include "shared.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>

typedef struct	s_user
{
	int		sockfd;
	char	*nick;
	char	*channel;
	t_uint8	rbuf[512];
	t_uint8	wbuf[512];
}				t_user;


typedef struct	s_context
{
	int			maxfd;
	int			listener;
	fd_set		rset;
	fd_set		wset;
	t_list		users;
	t_hashmap	channels;
	t_hashmap	user_by_nick;
}				t_context;

int				error(char *msg);
int				init_context(t_context *ctx);
void			clear_context(t_context *ctx);
int				add_user(t_context *ctx);
int				route_input(t_user *user, t_context *ctx);
int				handle_output(t_user *user, t_context *ctx);
int				run_server(t_uint16 port);

#endif