#include "server.h"

int		enqueue_chat(
	t_uint8 *payload, t_uint64 size,
	t_uint64 content_offset, t_user *user)
{
	t_uint8		*msg;
	t_chat_hdr	*hdr;

	msg = (t_uint8 *)ft_memalloc(size + sizeof(t_chat_hdr));
	if (msg == NULL)
		return (error((char *)"fail to allocate msg"));
	hdr = (t_chat_hdr *)msg;
	hdr->type = MSG_TYPE_CHAT_RELAY;
	hdr->size = size;
	hdr->content_offset = content_offset;
	ft_memcpy(msg + sizeof(t_chat_hdr), payload, size);
	if (push_list_node(msg, &user->output_q) == -1)
	{
		ft_memdel((void **)&msg);
		return (error((char *)"fail to push output msg"));
	}
	return (0);
}

int		broadcast_chat(
	t_uint8 *payload, t_uint64 size,
	t_uint64 content_offset, t_list *users)
{
	t_list_node	*node;
	t_user		*user;

	node = users->head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		if (enqueue_chat(payload, size, content_offset, user) == -1)
			return (-1);
		node = node->next;
	}
	return (0);
}

int		broadcast_chat_except_me(
	t_uint8 *payload, t_uint64 size,
	t_uint64 content_offset, t_list *users, t_user *me)
{
	t_list_node	*node;
	t_user		*user;

	node = users->head;
	while (node != NULL)
	{
		user = (t_user *)node->data;
		if (user != me)
		{
			if (enqueue_chat(payload, size, content_offset, user) == -1)
				return (-1);
		}
		node = node->next;
	}
	return (0);
}