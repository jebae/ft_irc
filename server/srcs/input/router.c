#include "server.h"

/*
** TODO
** recv() header first, recv payload after
*/
int		route_input(t_user *user, t_context *ctx)
{
	(void)ctx;
	if (recv(user->sockfd, user->rbuf, 512, 0) == -1)
		return (-1);
	printf("%s\n", user->rbuf);
	return (0);
}