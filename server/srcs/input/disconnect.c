#include "server.h"

static int	ack_user_disconnected(
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
	payload = strcat_all(3,
		(char *)"[", nick, (char *)"] disconnected");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = broadcast_ack_except_me(MSG_TYPE_DISCONNECTACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, channel, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_disconnect(t_user *user, t_context *ctx)
{
	if (user->channel)
		ack_user_disconnected(user, user->channel, &ctx->channels);
	remove_user(user, ctx);
	return (0);
}