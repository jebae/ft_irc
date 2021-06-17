#include "client.h"

int			read_stdin(t_context *ctx)
{
	if (get_next_line(STDIN_FILENO, &ctx->user_input) == -1)
		return (error((char *)"fail to read line"));
	return (0);
}

static int	recv_simple_payload(int sockfd, t_msg_hdr *hdr)
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

int			read_socket(int sockfd)
{
	t_msg_hdr	hdr;

	if (recv(sockfd, &hdr, sizeof(t_msg_hdr), 0) == -1)
		return (error((char *)"fail to recv header"));
	// route when hdr is special like msg from user
	return (recv_simple_payload(sockfd, &hdr));
}