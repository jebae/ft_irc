#include "client.h"

t_uint64	parse_nick(char **tokens, t_uint8 **msg)
{
	t_uint8		*payload;
	t_msg_hdr	*hdr;
	size_t		payload_size;

	if (tokens[1] == NULL)
		return (0);
	payload_size = ft_strlen(tokens[1]) + 1;
	*msg = (t_uint8 *)ft_memalloc(sizeof(t_msg_hdr) + payload_size);
	if (*msg == NULL)
	{
		error((char *)"fail to allocate nick msg");
		return (0);
	}
	payload = *msg + sizeof(t_msg_hdr);
	ft_strcpy((char *)payload, tokens[1]);
	hdr = (t_msg_hdr *)*msg;
	hdr->type = MSG_TYPE_NICK;
	hdr->size = payload_size;
	return (payload_size + sizeof(t_msg_hdr));
}