#include "server.h"

static int	ack_channel_not_exist(t_user *user, char *channel_name)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"Channel [", channel_name, (char *)"] not exist");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_output(MSG_TYPE_JOINACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_channel_already_joined(t_user *user, char *channel_name)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"You already joined [", channel_name, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_output(MSG_TYPE_JOINACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_left(t_user *user, char *prev_channel, t_hashmap *channels)
{
	t_list	*channel;
	char	*payload;
	int		res;

	channel = (t_list *)get_hashmap(prev_channel, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	payload = strcat_all(5, (char *)"[", user->nick,
		(char *)"] left [", prev_channel, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_to_users(MSG_TYPE_LEAVEACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, channel);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_to_channel(t_user *user, t_hashmap *channels)
{
	t_list	*channel;
	char	*payload;
	int		res;

	channel = (t_list *)get_hashmap(user->channel, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	payload = strcat_all(5, (char *)"[", user->nick,
		(char *)"] joined [", user->channel, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_to_users_except_me(MSG_TYPE_JOINACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, channel, user);
	ft_memdel((void **)&payload);
	return (res);
}

static int	ack_to_joined_user(t_user *user)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"You joined [", user->channel, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_output(MSG_TYPE_JOINACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_join(char *payload, t_user *user, t_context *ctx)
{
	char	*prev;

	prev = NULL;
	if (get_hashmap(payload, &ctx->channels) == NULL)
		return (ack_channel_not_exist(user, payload));
	if (user->channel && ft_strcmp(user->channel, payload) == 0)
		return (ack_channel_already_joined(user, payload));
	if (user->channel && (prev = ft_strdup(user->channel)) == NULL)
		return (error((char *)"fail to allocate prev channel"));
	if (join_channel(user, payload, &ctx->channels) == -1)
	{
		if (prev)
			ft_memdel((void **)&prev);
		return (-1);
	}
	if (prev)
	{
		if (ack_left(user, prev, &ctx->channels) == -1)
			return (-1);
		ft_memdel((void **)&prev);
	}
	if (ack_to_channel(user, &ctx->channels) == -1)
		return (-1);
	return (ack_to_joined_user(user));
}