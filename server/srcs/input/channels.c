#include "server.h"

static int		ack_no_channel_joined(t_user *user)
{
	static char	*payload = (char *)"No channels";

	return (enqueue_ack(MSG_TYPE_CHANNELSACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static t_uint64	get_payload_size(t_list *channels)
{
	t_list_node	*node;
	char		*name;
	t_uint64	len;

	node = channels->head;
	len = 0;
	while (node)
	{
		name = (char *)node->data;
		len += ft_strlen(name) + 1;
		node = node->next;
	}
	return (len);
}

static int		ack_channels(t_user *user, t_list *channels)
{
	t_list_node	*node;
	char		*name;
	char		*payload;
	t_uint64	size;
	int			res;

	size = get_payload_size(channels);
	payload = (char *)ft_memalloc(size);
	if (payload == NULL)
		return (error((char *)"fail to allocate payload"));
	node = channels->head;
	while (node)
	{
		name = (char *)node->data;
		ft_strcat(payload, name);
		if (node->next)
			ft_strcat(payload, "\n");
		node = node->next;
	}
	res = enqueue_ack(MSG_TYPE_CHANNELSACK, (t_uint8 *)payload, size, user);
	ft_memdel((void **)&payload);
	return (res);
}

int				handle_channels(t_user *user, t_context *ctx)
{
	t_list	channels;
	int		res;

	if (get_hashmap_keys(&ctx->channels, &channels) == -1)
		return (error((char *)"fail to get hashmap keys"));
	if (channels.length == 0)
		return (ack_no_channel_joined(user));
	res = ack_channels(user, &channels);
	clear_list(&channels);
	return (res);
}