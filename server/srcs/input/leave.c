#include "server.h"

static int	ack_no_channel_joined(t_user *user)
{
	static char	*payload = (char *)"You are not in any channel";

	return (enqueue_ack(MSG_TYPE_LEAVEACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_to_channel(
	t_user *user, char *channel_name, t_hashmap *channels)
{
	t_list	*channel;
	char	*payload;
	int		res;
	char	*nick;

	channel = (t_list *)get_hashmap(channel_name, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	nick = (user->nick) ? user->nick : (char *)"unknown";
	payload = strcat_all(5, (char *)"[", nick,
		(char *)"] left [", channel_name, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_ack(MSG_TYPE_LEAVEACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, channel);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_to_left_user(t_user *user, char *channel_name)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"You left [", channel_name, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_ack(MSG_TYPE_LEAVEACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_leave(t_user *user, t_context *ctx)
{
	char	*channel_name;
	int		res;

	if (user->channel == NULL)
		return (ack_no_channel_joined(user));
	channel_name = ft_strdup(user->channel);
	if (channel_name == NULL)
		return (error((char *)"fail to copy channel name"));
	if (leave_channel(user, &ctx->channels) == -1)
	{
		ft_memdel((void **)&channel_name);
		return (-1);
	}
	if (ack_to_channel(user, channel_name, &ctx->channels) == -1)
	{
		ft_memdel((void **)&channel_name);
		return (-1);
	}
	res = ack_to_left_user(user, channel_name);
	ft_memdel((void **)&channel_name);
	return (res);
}