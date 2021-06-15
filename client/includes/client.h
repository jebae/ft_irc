#ifndef CLIENT_H
# define CLIENT_H

# include "libft.h"
# include "get_next_line.h"
# include "shared.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>

int			error(char *msg);
int			send_msg(int sockfd);
int			run_client(t_uint16 port);

#endif