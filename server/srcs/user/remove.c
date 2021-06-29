#include "server.h"

void		remove_user(t_user *user, t_context *ctx)
{
	if (user->nick)
	{
		remove_hashmap(user->nick, &ctx->user_by_nick);
		ft_memdel((void **)&user->nick);
	}
	if (user->channel)
		leave_channel(user, &ctx->channels);
	clear_list(&user->output_q);
	close(user->sockfd);
	user->disconnect = 1;
}

void		remove_disconnected_users(t_list *users)
{
	t_list_node	*cur;
	t_list_node	*prev;
	t_list_node	*next;

	prev = NULL;
	cur = users->head;
	while (cur)
	{
		next = cur->next;
		if (((t_user *)cur->data)->disconnect)
		{
			if (prev)
				prev->next = next;
			else
				users->head = next;
			ft_memdel((void **)&cur->data);
			ft_memdel((void **)&cur);
			users->length--;
		}
		else
			prev = cur;
		cur = next;
	}
	users->tail = prev;
}