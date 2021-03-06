#include "server.h"

int		enqueue_ack(
	t_uint8 type, t_uint8 *payload, t_uint64 size, t_user *user)
{
	t_uint8		*msg;
	t_msg_hdr	*hdr;

	msg = (t_uint8 *)ft_memalloc(size + sizeof(t_msg_hdr));
	if (msg == NULL)
		return (error((char *)"fail to allocate msg"));
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

int		broadcast_ack(
	t_uint8 type, t_uint8 *payload, t_uint64 size, t_list *users)
{
	t_list_node	*node;
	t_user		*user;

	node = users->head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		if (enqueue_ack(type, payload, size, user) == -1)
			return (-1);
		node = node->next;
	}
	return (0);
}

int		broadcast_ack_except_me(
	t_uint8 type, t_uint8 *payload, t_uint64 size,
	t_list *users, t_user *me)
{
	t_list_node	*node;
	t_user		*user;

	node = users->head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		if (user != me)
		{
			if (enqueue_ack(type, payload, size, user) == -1)
				return (-1);
		}
		node = node->next;
	}
	return (0);
}