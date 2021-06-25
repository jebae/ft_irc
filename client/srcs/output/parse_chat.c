#include "client.h"

static char		*parse_msg(char *input)
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

static t_uint64	parse_direct_chat(char *content, t_uint8 **msg)
{
	t_uint8		*payload;
	t_chat_hdr	hdr;
	t_uint64	nick_len;
	char		*chat;

	chat = parse_msg(content + 1);
	nick_len = ft_strlen(content + 1);
	if (nick_len == 0 || chat == NULL)
	{
		info((char *)"Wrong format\nUsage: /chat #[nick] [content]");
		return (0);
	}
	hdr.size = nick_len + ft_strlen(chat) + 2;
	if ((*msg = (t_uint8 *)ft_memalloc(sizeof(t_chat_hdr) + hdr.size)) == NULL)
	{
		error((char *)"fail to allocate msg");
		return (0);
	}
	hdr.type = MSG_TYPE_DIRECT_CHAT;
	hdr.content_offset = nick_len + 1;
	ft_memcpy(*msg, &hdr, sizeof(t_chat_hdr));
	payload = *msg + sizeof(t_chat_hdr);
	ft_strcpy((char *)payload, content + 1);
	ft_strcpy((char *)payload + nick_len + 1, chat);
	return (hdr.size + sizeof(t_chat_hdr));
}

t_uint64		parse_chat(char *content, t_uint8 **msg)
{
	if (content == NULL)
	{
		info((char *)"Input required");
		return (0);
	}
	if (content[0] == '#')
		return (parse_direct_chat(content, msg));
	return (parse_simple_input(content, MSG_TYPE_CHANNEL_CHAT, msg));
}