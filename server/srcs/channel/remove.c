#include "server.h"

static void	leave_users(t_list *channel, t_hashmap *channels)
{
	t_list_node	*node;

	while (channel->length > 0)
	{
		node = channel->head;
		leave_channel((t_user *)node->data, channels);
	}
}

int			remove_channel(char *name, t_hashmap *channels)
{
	t_list	*channel;

	channel = (t_list *)get_hashmap(name, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	leave_users(channel, channels);
	remove_hashmap(name, channels);
	return (0);
}