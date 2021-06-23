#include "server.h"

int			join_channel(t_user *user, char *name, t_hashmap *channels)
{
	char	*prev;
	char	*new_channel;
	t_list	*channel;

	channel = (t_list *)get_hashmap(name, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	prev = user->channel;
	new_channel = ft_strdup(name);
	if (new_channel == NULL)
		return (error((char *)"fail to allocate channel"));
	if (push_list_node(user, channel) == -1)
	{
		ft_memdel((void **)&new_channel);
		return (error((char *)"fail to add user to channel"));
	}
	leave_channel(user, channels);
	user->channel = new_channel;
	return (0);
}