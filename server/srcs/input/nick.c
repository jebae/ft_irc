#include "server.h"

static int	ack_nick_too_long(t_user *user)
{
	static char	*payload = (char *)"Nickname too long";

	return (enqueue_output(MSG_TYPE_NICKACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_already_exist(t_user *user)
{
	static char	*payload = (char *)"Nickname already exist";

	return (enqueue_output(MSG_TYPE_NICKACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_changed(t_user *user)
{
	static char	*payload_prefix = (char *)"Your nickname: ";
	char		*payload;
	int			res;

	payload = strcat_all(2, payload_prefix, user->nick);
	if (payload == NULL)
		return (-1);
	res = enqueue_output(MSG_TYPE_NICKACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

int			nick(char *payload, t_user *user, t_context *ctx)
{
	if (ft_strlen(payload) > MAX_NICK_LEN)
		return (ack_nick_too_long(user));
	if (get_hashmap(payload, &ctx->user_by_nick) != NULL)
		return (ack_already_exist(user));
	if (change_nick(payload, user, ctx) == -1)
		return (-1);
	return (ack_changed(user));
}