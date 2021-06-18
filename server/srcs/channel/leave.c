#include "server.h"

static void	remove_user_from_channel(t_user *target, t_list *channel)
{
	t_list_node	*node;
	t_list_node	*prev;

	prev = NULL;
	node = channel->head;
	while (node)
	{
		if ((t_user *)node->data == target)
			break ;
		prev = node;
		node = node->next;
	}
	if (node == NULL)
		return ;
	if (prev != NULL)
		prev->next = node->next;
	else
		channel->head = node->next;
	if (node == channel->tail)
		channel->tail = prev;
	channel->length--;
	ft_memdel((void **)&node);
}

int			leave_channel(t_user *user, t_hashmap *channels)
{
	t_list	*channel;

	if (user->channel == NULL)
		return (0);
	channel = (t_list *)get_hashmap(user->channel, channels);
	if (channel == NULL)
		return (error((char *)"fail to find channel"));
	remove_user_from_channel(user, channel);
	ft_memdel((void **)&user->channel);
	return (0);
}