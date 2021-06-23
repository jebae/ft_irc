#include "client.h"

/*
** Input after command is directly payload
*/
static t_uint64	parse_simple_input(char *input, t_uint8 type, t_uint8 **msg)
{
	t_uint8		*payload;
	t_msg_hdr	*hdr;
	size_t		payload_size;

	payload_size = ft_strlen(input) + 1;
	*msg = (t_uint8 *)ft_memalloc(sizeof(t_msg_hdr) + payload_size);
	if (*msg == NULL)
	{
		error((char *)"fail to allocate msg");
		return (0);
	}
	payload = *msg + sizeof(t_msg_hdr);
	ft_strcpy((char *)payload, input);
	hdr = (t_msg_hdr *)*msg;
	hdr->type = type;
	hdr->size = payload_size;
	return (payload_size + sizeof(t_msg_hdr));
}

static t_uint64	parse_no_payload(t_uint8 type, t_uint8 **msg)
{
	t_msg_hdr	*hdr;

	*msg = (t_uint8 *)ft_memalloc(sizeof(t_msg_hdr));
	if (*msg == NULL)
	{
		error((char *)"fail to allocate msg");
		return (0);
	}
	hdr = (t_msg_hdr *)*msg;
	hdr->type = type;
	hdr->size = 0;
	return (sizeof(t_msg_hdr));
}

t_uint64		parse_user_input(char *input, t_uint8 **msg)
{
	t_uint64	msg_size;

	msg_size = 0;
	if (ft_strncmp(input, "/nick ", 6) == 0)
		msg_size = parse_simple_input(input + 6, MSG_TYPE_NICK, msg);
	else if (ft_strncmp(input, "/create_channel ", 16) == 0)
		msg_size = parse_simple_input(
			input + 16, MSG_TYPE_CREATE_CHANNEL, msg);
	else if (ft_strcmp(input, "/leave") == 0)
		msg_size = parse_no_payload(MSG_TYPE_LEAVE, msg);
	else if (ft_strncmp(input, "/remove_channel ", 16) == 0)
		msg_size = parse_simple_input(
			input + 16, MSG_TYPE_REMOVE_CHANNEL, msg);
	return (msg_size);
}