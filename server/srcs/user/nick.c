#include "server.h"

static int	rollback(char *prev, char *key, t_user *user, char *err_msg)
{
	if (user->nick != NULL)
		ft_memdel((void **)&user->nick);
	if (key != NULL)
		ft_memdel((void **)&key);
	user->nick = prev;
	return (error(err_msg));
}

int			change_nick(
	char *new_nick, t_user *user, t_hashmap *user_by_nick)
{
	char	*key;
	char	*prev;

	prev = user->nick;
	key = NULL;
	if ((user->nick = ft_strdup(new_nick)) == NULL)
		return (rollback(prev, NULL, user,
			(char *)"fail to copy nick payload"));
	if ((key = ft_strdup(user->nick)) == NULL)
		return (rollback(prev, NULL, user,
			(char *)"fail to copy nick key"));
	if (set_hashmap(key, user, user_by_nick) == -1)
		return (rollback(prev, key, user,
			(char *)"fail to set user_by_nick"));
	if (prev != NULL)
	{
		remove_hashmap(prev, user_by_nick);
		ft_memdel((void **)&prev);
	}
	return (0);
}