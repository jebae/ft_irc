#ifndef SERVER_H
# define SERVER_H

#ifdef __cplusplus
extern "C" {
#endif
# include "libft.h"
#ifdef __cplusplus
}
#endif
# include "shared.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>

# define MAX_NICK_LEN	15

typedef struct	s_user
{
	int		sockfd;
	char	*nick;
	char	*channel;
	t_list	output_q;
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
int				run_server(t_uint16 port);

int				add_user(t_context *ctx);
int				change_nick(
	char *new_nick, t_user *user, t_context *ctx);

int				route_input(t_user *user, t_context *ctx);

int				send_msg(t_user *user);
int				enqueue_output(
	t_uint8 type, t_uint8 *payload, t_uint64 size, t_user *user);

int				nick(char *payload, t_user *user, t_context *ctx);

#endif