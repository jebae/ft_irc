#include "client.h"

static void	clear_tokens(char **tokens)
{
	int		i;

	i = 0;
	while (tokens[i])
		ft_memdel((void **)&tokens[i++]);
	ft_memdel((void **)&tokens);
}

t_uint64	parse_user_input(char *input, t_uint8 **msg)
{
	char		**tokens;
	t_uint64	msg_size;

	tokens = ft_strsplit(input, ' ');
	if (tokens == NULL)
	{
		error((char *)"fail to tokenize input");
		return (0);
	}
	msg_size = 0;
	if (ft_strcmp(tokens[0], "/nick") == 0)
		msg_size = parse_nick(tokens, msg);
	clear_tokens(tokens);
	return (msg_size);
}