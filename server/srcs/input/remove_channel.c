#include "server.h"

static int	copy_channel_users(t_list *dest, t_list *src)
{
	t_list_node	*node;

	init_list(dest);
	node = src->head;
	while (node != NULL)
	{
		if (push_list_node(node->data, dest) == -1)
			return (error((char *)"fail to push_list_node"));
		node = node->next;
	}
	return (0);
}

static void	clear_channel_users(t_list *users)
{
	t_list_node	*node;
	t_list_node	*tmp;

	node = users->head;
	while (node)
	{
		tmp = node->next;
		ft_memdel((void **)&node);
		node = tmp;
	}
}

static int	ack_channel_not_exist(char *channel_name, t_user *user)
{
	char	*payload;
	int		res;

	payload = strcat_all(3, (char *)"Channel [", channel_name, (char *)"] not exist");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_output(MSG_TYPE_REMOVE_CHANNELACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_channel_removed(char *channel_name, t_list *users)
{
	char	*payload;
	int		res;

	payload = strcat_all(3, (char *)"Channel [", channel_name,
		(char *)"] removed");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_to_channel(MSG_TYPE_REMOVE_CHANNELACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, users);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_remove_channel(
	char *payload, t_user *user, t_context *ctx)
{
	t_list	channel_users;
	t_list	*channel;
	int		res;

	channel = (t_list *)get_hashmap(payload, &ctx->channels);
	if (channel == NULL)
		return (ack_channel_not_exist(payload, user));
	if (copy_channel_users(&channel_users, channel) == -1)
	{
		clear_channel_users(&channel_users);
		return (-1);
	}
	if (remove_channel(payload, &ctx->channels) == -1)
	{
		clear_channel_users(&channel_users);
		return (-1);
	}
	res = ack_channel_removed(payload, &channel_users);
	clear_channel_users(&channel_users);
	return (res);
}