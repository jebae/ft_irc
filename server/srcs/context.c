#include "server.h"

static void	clear_channel(t_tree_node *node)
{
	t_list_node	*cur;
	t_list_node	*next;

	ft_memdel((void **)&node->key);
	cur = ((t_list *)node->val)->head;
	while (cur != NULL)
	{
		next = cur->next;
		ft_memdel((void **)&cur);
		cur = next;
	}
}

static void	clear_user_by_nick(t_tree_node *node)
{
	ft_memdel((void **)&node->key);
}

int			init_context(t_context *ctx)
{
	init_list(&ctx->users);
	if (init_hashmap(32, &ctx->channels, (t_key_cmp)ft_strcmp, clear_channel) == -1)
		return (-1);
	if (init_hashmap(128, &ctx->user_by_nick,
		(t_key_cmp)ft_strcmp, clear_user_by_nick) == -1)
	{
		clear_hashmap(&ctx->channels);
		return (-1);
	}
	ctx->maxfd = -1;
	return (0);
}

void		clear_context(t_context *ctx)
{
	clear_list(&ctx->users);
	clear_hashmap(&ctx->channels);
	clear_hashmap(&ctx->user_by_nick);
}