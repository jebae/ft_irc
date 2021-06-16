#ifndef CLIENT_H
# define CLIENT_H

# include "libft.h"
# include "get_next_line.h"
# include "shared.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>

typedef struct	s_context
{
	fd_set	rset;
	fd_set	wset;
	int		sockfd;
	char	*user_input;
}				t_context;


int			error(char *msg);
int			send_msg(t_context *ctx);
int			run_client(t_uint16 port);
int			read_stdin(t_context *ctx);

#endif