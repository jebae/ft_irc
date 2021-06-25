#include "server.h"

static int	ack_nick_not_set(t_user *user)
{
	static char	*payload = (char *)"You need to set nickname";

	return (enqueue_ack(MSG_TYPE_CHATACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user));
}

static int	ack_user_not_exist(t_user *user, char *nick)
{
	char	*payload;
	int		res;

	payload = strcat_all(3,
		(char *)"User [", nick, (char *)"] not exist");
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	res = enqueue_ack(MSG_TYPE_CHATACK,
		(t_uint8 *)payload, ft_strlen(payload) + 1, user);
	ft_memdel((void **)&payload);
	return (res);
}

static int	relay_chat(
	t_chat_hdr *hdr, t_uint8 *payload_to_relay,
	t_user *user, t_hashmap *user_by_nick)
{
	char		*payload;
	char		*content;
	int			res;
	t_user		*receiver;
	t_uint64	size;

	receiver = (t_user *)get_hashmap((char *)payload_to_relay, user_by_nick);
	content = (char *)payload_to_relay + hdr->content_offset;
	size = ft_strlen(user->nick) + ft_strlen(content) + 2;
	payload = (char *)ft_memalloc(size);
	if (payload == NULL)
		return (error((char *)"fail to concat payload"));
	ft_strcpy(payload, user->nick);
	ft_strcpy(payload + ft_strlen(user->nick) + 1, content);
	res = enqueue_chat((t_uint8 *)payload, size,
		ft_strlen(user->nick) + 1, receiver);
	ft_memdel((void **)&payload);
	return (res);
}

int			handle_msg_to_user(
	t_chat_hdr *hdr, t_uint8 *payload,
	t_user *user, t_context *ctx)
{
	if (user->nick == NULL)
		return (ack_nick_not_set(user));
	if (get_hashmap((char *)payload, &ctx->user_by_nick) == NULL)
		return (ack_user_not_exist(user, (char *)payload));
	return (relay_chat(hdr, payload, user, &ctx->user_by_nick));
}