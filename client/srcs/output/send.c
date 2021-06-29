#include "client.h"

static void	print_my_chat(t_chat_hdr *hdr, t_uint8 *msg)
{
	char	*payload;

	if (hdr->type == MSG_TYPE_CHANNEL_CHAT)
		printf("[me]: @channel %s\n", msg + sizeof(t_msg_hdr));
	else
	{
		payload = (char *)msg + sizeof(t_chat_hdr);
		printf("[me]: @%s %s\n", payload, payload + hdr->content_offset);
	}
}

int			send_msg(t_context *ctx)
{
	t_uint16	msg_size;
	t_uint8		*msg;
	t_chat_hdr	*hdr;
	int			err;

	msg = NULL;
	msg_size = parse_user_input(ctx->user_input, &msg);
	ft_memdel((void **)&ctx->user_input);
	if (msg_size == 0)
		return (0);
	err = 0;
	if (send(ctx->sockfd, msg, msg_size, 0) == -1)
		err = 1;
	hdr = (t_chat_hdr *)msg;
	if (hdr->type == MSG_TYPE_CHANNEL_CHAT
		|| hdr->type == MSG_TYPE_DIRECT_CHAT)
		print_my_chat(hdr, msg);
	if (hdr->type == MSG_TYPE_DISCONNECT)
		ctx->disconnect = 1;
	ft_memdel((void **)&msg);
	if (err)
		return (error((char *)"fail to send"));
	return (0);
}