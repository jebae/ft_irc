#include "client.h"

int		send_msg(t_context *ctx)
{
	t_uint16	msg_size;
	t_uint8		*msg;
	int			err;

	msg = NULL;
	msg_size = parse_user_input(ctx->user_input, &msg);
	ft_memdel((void **)&ctx->user_input);
	if (msg_size == 0)
		return (0);
	err = 0;
	if (send(ctx->sockfd, msg, msg_size, 0) == -1)
		err = 1;
	ft_memdel((void **)&msg);
	if (err)
		return (error((char *)"fail to send"));
	return (0);
}