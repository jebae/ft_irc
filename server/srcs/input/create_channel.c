#include "server.h"

static int	ack_channel_name_too_long(t_user *user)
{
	static char	*payload = (char *)"Channel name too long";

	return (enqueue_output(MSG_TYPE_CREATE_CHANNELACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_channel_already_exist(t_user *user)
{
	static char	*payload = (char *)"Channel already exist";

	return (enqueue_output(MSG_TYPE_CREATE_CHANNELACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_channel_created(char *name, t_list *users)
{
	char		*payload;
	int			res;

	payload = strcat_all(3, (char *)"Channel [", name, (char *)"] created");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_to_all(MSG_TYPE_CREATE_CHANNELACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, users);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_create_channel(char *payload, t_user *user, t_context *ctx)
{
	if (ft_strlen(payload) > MAX_CHANNEL_NAME_LEN)
		return (ack_channel_name_too_long(user));
	if (get_hashmap(payload, &ctx->channels) != NULL)
		return (ack_channel_already_exist(user));
	if (create_channel(payload, &ctx->channels) == -1)
		return (-1);
	return (ack_channel_created(payload, &ctx->users));
}