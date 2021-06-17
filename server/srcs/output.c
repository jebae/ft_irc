#include "server.h"

int		send_msg(t_user *user)
{
	t_list		*q;
	t_msg_hdr	*hdr;

	q = &user->output_q;
	while (q->length > 0)
	{
		hdr = (t_msg_hdr *)q->head->data;
		if (send(user->sockfd,
			q->head->data, sizeof(t_msg_hdr) + hdr->size, 0) == -1)
			return (error((char *)"fail to send message"));
		pop_list_node(0, q);
	}
	return (0);
}

int		enqueue_output(
	t_uint8 type, t_uint8 *payload, t_uint64 size, t_user *user)
{
	t_uint8		*msg;
	t_msg_hdr	*hdr;

	msg = (t_uint8 *)ft_memalloc(size + sizeof(t_msg_hdr));
	if (msg == NULL)
		return (-1);
	hdr = (t_msg_hdr *)msg;
	hdr->type = type;
	hdr->size = size;
	ft_memcpy(msg + sizeof(t_msg_hdr), payload, size);
	if (push_list_node(msg, &user->output_q) == -1)
	{
		ft_memdel((void **)&msg);
		return (error((char *)"fail to push output msg"));
	}
	return (0);
}