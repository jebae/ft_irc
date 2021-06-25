#include "client.h"

static char		*parse_content(char *input)
{
	int		i;

	i = 0;
	while (input[i] != '\0' && !ft_iswhitespace(input[i]))
		i++;
	if (ft_iswhitespace(input[i]))
		input[i] = '\0';
	else
		return (NULL);
	i++;
	while (input[i] != '\0' && ft_iswhitespace(input[i]))
		i++;
	if (input[i] == '\0')
		return (NULL);
	return (input + i);
}

/*
** Input after command is directly payload
*/
t_uint64		parse_simple_input(
	char *content, t_uint8 type, t_uint8 **msg)
{
	t_uint8		*payload;
	t_msg_hdr	*hdr;
	size_t		payload_size;

	if (content == NULL)
	{
		info((char *)"Input required");
		return (0);
	}
	payload_size = ft_strlen(content) + 1;
	*msg = (t_uint8 *)ft_memalloc(sizeof(t_msg_hdr) + payload_size);
	if (*msg == NULL)
	{
		error((char *)"fail to allocate msg");
		return (0);
	}
	payload = *msg + sizeof(t_msg_hdr);
	ft_strcpy((char *)payload, content);
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
	char		*content;

	if (input && input[0] != '/')
	{
		info((char *)"Command required");
		return (0);
	}
	content = parse_content(input);
	msg_size = 0;
	if (ft_strcmp(input, "/nick") == 0)
		msg_size = parse_simple_input(content, MSG_TYPE_NICK, msg);
	else if (ft_strcmp(input, "/create_channel") == 0)
		msg_size = parse_simple_input(content, MSG_TYPE_CREATE_CHANNEL, msg);
	else if (ft_strcmp(input, "/leave") == 0)
		msg_size = parse_no_payload(MSG_TYPE_LEAVE, msg);
	else if (ft_strcmp(input, "/remove_channel") == 0)
		msg_size = parse_simple_input(content, MSG_TYPE_REMOVE_CHANNEL, msg);
	else if (ft_strcmp(input, "/join") == 0)
		msg_size = parse_simple_input(content, MSG_TYPE_JOIN, msg);
	else if (ft_strcmp(input, "/msg") == 0)
		msg_size = parse_chat(content, msg);
	return (msg_size);
}