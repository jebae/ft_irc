#include "client.h"

int			read_stdin(t_context *ctx)
{
	if (get_next_line(STDIN_FILENO, &ctx->user_input) == -1)
		return (error((char *)"fail to read line"));
	return (0);
}

static int	read_header(int sockfd, t_hdr *hdr)
{
	if (recv(sockfd, hdr, sizeof(t_msg_hdr), 0) == -1)
		return (error((char *)"fail to recv header"));
	if (hdr->msg.type == MSG_TYPE_CHAT_RELAY)
	{
		if (recv(sockfd, (t_uint8 *)hdr + sizeof(t_msg_hdr),
			sizeof(t_chat_hdr) - sizeof(t_msg_hdr), 0) == -1)
			return (error((char *)"fail to recv header"));
	}
	return (0);
}

static int	read_simple_payload(int sockfd, t_msg_hdr *hdr)
{
	t_uint8	*payload;

	payload = (t_uint8 *)ft_memalloc(hdr->size);
	if (payload == NULL)
		return (error((char *)"fail to allocate payload"));
	if (recv(sockfd, payload, hdr->size, 0) == -1)
		return (error((char *)"fail to receive payload"));
	printf("%s\n", payload);
	ft_memdel((void **)&payload);
	return (0);
}

static int	read_relay_payload(int sockfd, t_chat_hdr *hdr)
{
	t_uint8	*payload;

	payload = (t_uint8 *)ft_memalloc(hdr->size);
	if (payload == NULL)
		return (error((char *)"fail to allocate payload"));
	if (recv(sockfd, payload, hdr->size, 0) == -1)
		return (error((char *)"fail to receive payload"));
	printf("[%s]: %s\n", (char *)payload,
		(char *)payload + hdr->content_offset);
	ft_memdel((void **)&payload);
	return (0);
}

int			read_socket(int sockfd)
{
	t_hdr	hdr;

	if (read_header(sockfd, &hdr) == -1)
		return (-1);
	if (hdr.msg.type == MSG_TYPE_CHAT_RELAY)
		return (read_relay_payload(sockfd, &hdr.chat));
	return (read_simple_payload(sockfd, &hdr.msg));
}