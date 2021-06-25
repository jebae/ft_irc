#include "server.h"

static int	ack_nick_not_set(t_user *user)
{
	static char	*payload = (char *)"You need to set nickname";

	return (enqueue_ack(MSG_TYPE_CHATACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_not_joined_channel(t_user *user)
{
	static char	*payload = (char *)"You are not in any channel";

	return (enqueue_ack(MSG_TYPE_CHATACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	relay_chat(char *content, t_user *user, t_hashmap *channels)
{
	t_list		*channel;
	t_uint64	nick_len;
	t_uint64	size;
	char		*payload;
	int			res;

	channel = (t_list *)get_hashmap(user->channel, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	nick_len = ft_strlen(user->nick);
	size = nick_len + ft_strlen(content) + 2;
	payload = (char *)ft_memalloc(size);
	if (payload == NULL)
		return (error((char *)"fail to allocate payload"));
	ft_strcpy(payload, user->nick);
	ft_strcpy(payload + nick_len + 1, content);
	res = broadcast_chat_except_me(
		(t_uint8 *)payload, size, nick_len + 1, channel, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_msg_to_channel(
	char *payload, t_user *user, t_context *ctx)
{
	if (user->nick == NULL)
		return (ack_nick_not_set(user));
	if (user->channel == NULL)
		return (ack_not_joined_channel(user));
	return (relay_chat(payload, user, &ctx->channels));
}