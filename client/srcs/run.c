#include "client.h"

static t_context	*g_ctx;

static int	get_connected_socket(t_uint16 port)
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
	if (connect(sockfd,
		(struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(sockfd);
		return (error((char *)"fail to connect server"));
	}
	printf("connected\n");
	return (sockfd);
}

static void	init_fdset(t_context *ctx)
{
	FD_ZERO(&ctx->rset);
	FD_ZERO(&ctx->wset);
	FD_SET(ctx->sockfd, &ctx->rset);
	FD_SET(STDIN_FILENO, &ctx->rset);
	if (ctx->user_input != NULL)
		FD_SET(ctx->sockfd, &ctx->wset);
}

static int	loop_io(t_context *ctx)
{
	struct timeval	timeout;

	while (!ctx->disconnect)
	{
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		init_fdset(ctx);
		if (select(ctx->sockfd + 1, &ctx->rset, &ctx->wset, NULL,
			&timeout) == -1)
			return (error((char *)"fail to select"));
		if (FD_ISSET(ctx->sockfd, &ctx->rset))
			if (read_socket(ctx->sockfd) == -1)
				return (-1);
		if (FD_ISSET(STDIN_FILENO, &ctx->rset))
			if (read_stdin(ctx) == -1)
				return (-1);
		if (FD_ISSET(ctx->sockfd, &ctx->wset))
			send_msg(ctx);
	}
	return (0);
}

static void	handle_signal_quit(int sig)
{
	t_msg_hdr	hdr;

	(void)sig;
	hdr.type = MSG_TYPE_DISCONNECT;
	hdr.size = 0;
	send(g_ctx->sockfd, &hdr, sizeof(t_msg_hdr), 0);
	close(g_ctx->sockfd);
	exit(0);
}

int			run_client(t_uint16 port)
{
	t_context	ctx;
	int			res;

	ctx.disconnect = 0;
	ctx.user_input = NULL;
	if ((ctx.sockfd = get_connected_socket(port)) == -1)
		return (-1);
	g_ctx = &ctx;
	signal(SIGINT, handle_signal_quit);
	res = loop_io(&ctx);
	close(ctx.sockfd);
	return (res);
}