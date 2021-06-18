#include "server.h"

int		create_channel(char *name, t_hashmap *channels)
{
	char	*key;
	t_list	*users;

	if ((key = ft_strdup(name)) == NULL)
		return (error((char *)"fail to copy channel key"));
	if ((users = (t_list *)ft_memalloc(sizeof(t_list))) == NULL)
	{
		ft_memdel((void **)&key);
		return (error((char *)"fail to allocate channel user list"));
	}
	init_list(users);
	if (set_hashmap(key, users, channels) == -1)
	{
		ft_memdel((void **)&key);
		ft_memdel((void **)&users);
		return (error((char *)"fail to set channels hashmap"));
	}
	return (0);
}