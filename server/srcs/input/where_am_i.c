#include "server.h"

static int	ack_no_channel_joined(t_user *user)
{
	static char	*payload = (char *)"You are not in any channel";

	return (enqueue_ack(MSG_TYPE_WHERE_AM_IACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_channel_joined(t_user *user)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"[", user->channel, (char *)"]");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_ack(MSG_TYPE_WHERE_AM_IACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_where_am_i(t_user *user)
{
	if (user->channel == NULL)
		return (ack_no_channel_joined(user));
	return (ack_channel_joined(user));
}