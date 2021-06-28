#include "server.h"

static int		ack_no_channel_joined(t_user *user)
{
	static char	*payload = (char *)"You are not in any channel";

	return (enqueue_ack(MSG_TYPE_WHOACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static t_uint64	get_payload_size(t_list *channel)
{
	t_list_node	*node;
	t_user		*user;
	t_uint64	len;

	node = channel->head;
	len = 0;
	while (node)
	{
		user = (t_user *)node->data;
		len += (user->nick) ? ft_strlen(user->nick) + 1 : 0;
		node = node->next;
	}
	return (len);
}

static int		ack_user_list(t_user *user, t_list *channel)
{
	t_list_node	*node;
	t_user		*channel_user;
	char		*payload;
	t_uint64	size;
	int			res;

	size = get_payload_size(channel);
	payload = (char *)ft_memalloc(size);
	if (payload == NULL)
		return (error((char *)"fail to allocate payload"));
	node = channel->head;
	while (node)
	{
		channel_user = (t_user *)node->data;
		if (channel_user->nick)
		{
			ft_strcat(payload, channel_user->nick);
			if (node->next)
				ft_strcat(payload, "\n");
		}
		node = node->next;
	}
	res = enqueue_ack(MSG_TYPE_WHOACK, (t_uint8 *)payload, size, user);
	ft_memdel((void **)&payload);
	return (res);
}

int				handle_who(t_user *user, t_context *ctx)
{
	t_list	*channel;

	if (user->channel == NULL)
		return (ack_no_channel_joined(user));
	channel = (t_list *)get_hashmap(user->channel, &ctx->channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	return (ack_user_list(user, channel));
}