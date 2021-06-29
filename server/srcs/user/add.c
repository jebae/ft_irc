#include "server.h"

static t_user	*create_user(int sockfd)
{
	t_user	*user;

	user = (t_user *)ft_memalloc(sizeof(t_user));
	if (user == NULL)
		return (NULL);
	user->sockfd = sockfd;
	user->nick = NULL;
	user->channel = NULL;
	user->disconnect = 0;
	init_list(&user->output_q);
	return (user);
}

int				add_user(t_context *ctx)
{
	struct sockaddr_in	cli_addr;
	int					clifd;
	socklen_t			addr_size;
	t_user				*user;

	addr_size = sizeof(cli_addr);
	clifd = accept(ctx->listener, (struct sockaddr *)&cli_addr, &addr_size);
	if (clifd == -1)
		return (error((char *)"fail to accept connection"));
	printf("client accept: %s:%d\n",
		inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
	if ((user = create_user(clifd)) == NULL)
	{
		close(clifd);
		return (error((char *)"fail to create user"));
	}
	if (push_list_node(user, &ctx->users) == -1)
	{
		close(clifd);
		ft_memdel((void **)&user);
		return (error((char *)"fail to push user to list"));
	}
	ctx->maxfd = (ctx->maxfd > user->sockfd)
		? ctx->maxfd : user->sockfd;
	return (0);
}