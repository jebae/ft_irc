#include "server.h"

int		send_msg(t_user *user)
{
	t_list		*q;
	t_msg_hdr	*hdr;
	t_uint64	hdr_size;

	q = &user->output_q;
	while (q->length > 0)
	{
		hdr = (t_msg_hdr *)q->head->data;
		hdr_size = (hdr->type == MSG_TYPE_CHAT_RELAY)
			? sizeof(t_chat_hdr)
			: sizeof(t_msg_hdr);
		if (send(user->sockfd,
			q->head->data, hdr_size + hdr->size, 0) == -1)
			return (error((char *)"fail to send message"));
		pop_list_node(0, q);
	}
	return (0);
}