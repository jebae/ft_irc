#include "server.h"

static int		read_header(int sockfd, t_hdr *hdr)
{
	t_uint64	size_diff;

	if (recv(sockfd, hdr, sizeof(t_msg_hdr), 0) == -1)
		return (error((char *)"fail to recv header"));
	if (hdr->msg.type == MSG_TYPE_DIRECT_CHAT)
	{
		size_diff = sizeof(t_chat_hdr) - sizeof(t_msg_hdr);
		if (recv(sockfd,
			(t_uint8 *)hdr + sizeof(t_msg_hdr), size_diff, 0) == -1)
			return (error((char *)"fail to recv header"));
	}
	return (0);
}

static int		read_input(int sockfd, t_uint64 size, t_uint8 **payload)
{
	static t_uint8	dumb_buf[1024];

	*payload = (t_uint8 *)ft_memalloc(size);
	if (*payload == NULL)
	{
		recv(sockfd, dumb_buf, 1024, 0);
		return (error((char *)"fail to malloc payload"));
	}
	if (recv(sockfd, *payload, size, 0) == -1)
	{
		ft_memdel((void **)payload);
		return (error((char *)"fail to recv payload"));
	}
	return (0);
}

static void		log_request(t_user *user, t_msg_hdr *hdr)
{
	if (hdr->type == MSG_TYPE_NICK)
		printf("/nick");
	else if (hdr->type == MSG_TYPE_CREATE_CHANNEL)
		printf("/create_channel");
	else if (hdr->type == MSG_TYPE_LEAVE)
		printf("/leave");
	else if (hdr->type == MSG_TYPE_REMOVE_CHANNEL)
		printf("/remove_channel");
	else if (hdr->type == MSG_TYPE_JOIN)
		printf("/join");
	else if (hdr->type == MSG_TYPE_CHANNEL_CHAT
		|| hdr->type == MSG_TYPE_DIRECT_CHAT)
		printf("/msg");
	else if (hdr->type == MSG_TYPE_WHERE_AM_I)
		printf("/where_am_i");
	else if (hdr->type == MSG_TYPE_WHO)
		printf("/who");
	else if (hdr->type == MSG_TYPE_CHANNELS)
		printf("/channels");
	printf(" [%s]\n", user->nick);
}

static int		route(
	t_hdr *hdr, t_uint8 *payload, t_user *user, t_context *ctx)
{
	if (hdr->msg.type == MSG_TYPE_NICK)
		return (handle_nick((char *)payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_CREATE_CHANNEL)
		return (handle_create_channel((char *)payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_LEAVE)
		return (handle_leave(user, ctx));
	else if (hdr->msg.type == MSG_TYPE_REMOVE_CHANNEL)
		return (handle_remove_channel((char *)payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_JOIN)
		return (handle_join((char *)payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_CHANNEL_CHAT)
		return (handle_msg_to_channel((char *)payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_DIRECT_CHAT)
		return (handle_msg_to_user(&hdr->chat, payload, user, ctx));
	else if (hdr->msg.type == MSG_TYPE_WHERE_AM_I)
		return (handle_where_am_i(user));
	else if (hdr->msg.type == MSG_TYPE_WHO)
		return (handle_who(user, ctx));
	else if (hdr->msg.type == MSG_TYPE_CHANNELS)
		return (handle_channels(user, ctx));
	return (0);
}

int				route_input(t_user *user, t_context *ctx)
{
	t_hdr		hdr;
	t_uint8		*payload;
	int			res;

	if (read_header(user->sockfd, &hdr) == -1)
		return (-1);
	if (read_input(user->sockfd, hdr.msg.size, &payload) == -1)
		return (-1);
	log_request(user, &hdr.msg);
	res = route(&hdr, payload, user, ctx);
	ft_memdel((void **)&payload);
	return (res);
}