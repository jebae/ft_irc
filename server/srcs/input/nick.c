#include "server.h"

static int	rollback(char *prev, char *key, t_user *user, char *err_msg)
{
	if (user->nick != NULL)
		ft_memdel((void **)&user->nick);
	if (key != NULL)
		ft_memdel((void **)&key);
	user->nick = prev;
	return (error(err_msg));
}

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

int			nick(t_user *user, char *payload, t_context *ctx)
{
	char	*key;
	char	*prev;

	if (ft_strlen(payload) > MAX_NICK_LEN)
		return (ack_nick_too_long(user));
	if (get_hashmap(payload, &ctx->user_by_nick) != NULL)
		return (ack_already_exist(user));
	prev = user->nick;
	key = NULL;
	if ((user->nick = ft_strdup(payload)) == NULL)
		return (rollback(prev, NULL, user,
			(char *)"fail to copy nick payload"));
	if ((key = ft_strdup(user->nick)) == NULL)
		return (rollback(prev, NULL, user,
			(char *)"fail to copy nick key"));
	if (set_hashmap(key, user, &ctx->user_by_nick) == -1)
		return (rollback(prev, key, user,
			(char *)"fail to set user_by_nick"));
	if (prev != NULL)
	{
		remove_hashmap(prev, &ctx->user_by_nick);
		ft_memdel((void **)&prev);
	}
	return (ack_changed(user));
}