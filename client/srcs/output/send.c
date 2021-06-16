#include "client.h"

/*
** TODO
** parse user_input and send msg
*/
int		send_msg(t_context *ctx)
{
	t_uint16	size;
	int			err;

	printf("send\n");
	size = ft_strlen(ctx->user_input) + 1;
	err = 0;
	if (send(ctx->sockfd, ctx->user_input, size, 0) == -1)
		err = 1;
	ft_memdel((void **)&ctx->user_input);
	if (err)
		return (error("fail to send"));
	return (0);
}