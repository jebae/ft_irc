#include "server.h"

static int	get_socket(t_uint16 port)
{
	static char			*ip = (char *)"127.0.0.1";
	int					sockfd;
	struct sockaddr_in	addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return (error((char *)"fail to create socket"));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		return (error((char *)"fail to bind"));
	return (sockfd);
}

static void	init_fdset(t_context *ctx)
{
	t_list_node	*node;
	t_user		*user;

	FD_ZERO(&ctx->rset);
	FD_ZERO(&ctx->wset);
	node = ctx->users.head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		FD_SET(user->sockfd, &ctx->rset);
		if (user->output_q.length > 0)
			FD_SET(user->sockfd, &ctx->wset);
		node = node->next;
	}
	FD_SET(ctx->listener, &ctx->rset);
}

static int	handle_io(t_context *ctx)
{
	t_list_node	*node;
	t_user		*user;

	if (FD_ISSET(ctx->listener, &ctx->rset))
		if (add_user(ctx) == -1)
			return (-1);
	node = ctx->users.head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		if (FD_ISSET(user->sockfd, &ctx->rset))
			route_input(user, ctx);
		if (FD_ISSET(user->sockfd, &ctx->wset))
			if (send_msg(user) == -1)
				return (-1);
		node = node->next;
	}
	return (0);
}

static int	loop_io_multiplex(t_context *ctx)
{
	int				res;
	struct timeval	timeout;

	while (1)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		init_fdset(ctx);
		res = select(ctx->maxfd + 1, &ctx->rset, &ctx->wset, NULL, &timeout);
		if (res > 0 && handle_io(ctx) == -1)
			return (-1);
		else if (res == -1)
			return (error((char *)"fail to select"));
	}
	return (0);
}

int			run_server(t_uint16 port)
{
	t_context	ctx;
	int			res;

	if (init_context(&ctx) == -1)
		return (error((char *)"fail to initialize context"));
	if ((ctx.listener = get_socket(port)) == -1)
		return (-1);
	ctx.maxfd = ctx.listener;
	if (listen(ctx.listener, 1) == -1)
		return (error((char *)"fail to listen"));
	printf("server is running on %hu\n", port);
	res = loop_io_multiplex(&ctx);
	clear_context(&ctx);
	close(ctx.listener);
	return (res);
}