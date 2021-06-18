#include "server.h"

static int		read_input(int sockfd, t_msg_hdr *hdr, t_uint8 **payload)
{
	static t_uint8	dumb_buf[1024];

	if (recv(sockfd, hdr, sizeof(t_msg_hdr), 0) == -1)
		return (error((char *)"fail to recv header"));
	*payload = (t_uint8 *)ft_memalloc(hdr->size);
	if (*payload == NULL)
	{
		recv(sockfd, dumb_buf, 1024, 0);
		return (error((char *)"fail to malloc payload"));
	}
	if (recv(sockfd, *payload, hdr->size, 0) == -1)
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
	if (hdr->type == MSG_TYPE_CREATE_CHANNEL)
		printf("/create_channel");
	printf(" %s\n", user->nick);
}

int				route_input(t_user *user, t_context *ctx)
{
	t_msg_hdr	hdr;
	t_uint8		*payload;
	int			res;

	if (read_input(user->sockfd, &hdr, &payload) == -1)
		return (-1);
	log_request(user, &hdr);
	res = 0;
	if (hdr.type == MSG_TYPE_NICK)
		res = handle_nick((char *)payload, user, ctx);
	else if (hdr.type == MSG_TYPE_CREATE_CHANNEL)
		res = handle_create_channel((char *)payload, user, ctx);
	ft_memdel((void **)&payload);
	return (res);
}