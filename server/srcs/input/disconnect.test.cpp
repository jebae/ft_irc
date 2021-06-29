#include <gtest/gtest.h>
#include "server.h"

static t_user	*create_user(int sockfd)
{
	t_user	*user;

	user = (t_user *)ft_memalloc(sizeof(t_user));
	if (user == NULL)
		return (NULL);
	user->sockfd = sockfd;
	user->nick = NULL;
	user->channel = NULL;
	init_list(&user->output_q);
	return (user);
}

static int		list_has(void *data, t_list *list)
{
	t_list_node	*node;

	node = list->head;
	while (node != NULL)
	{
		if (node->data == data)
			return (1);
		node = node->next;
	}
	return (0);
}

class DisconnectTest: public ::testing::Test
{
protected:
	t_context	ctx;
	t_user		*me;

	void	SetUp(void)
	{
		if (init_context(&ctx) == -1)
			throw "fail to init_context";
		if ((me = create_user(42)) == NULL)
			throw "fail to create user";
		if (push_list_node(me, &ctx.users) == -1)
			throw "fail to push_list_node";

		if (create_channel((char *)"room1", &ctx.channels) == -1)
			throw "fail to create channel";
		if (create_channel((char *)"room2", &ctx.channels) == -1)
			throw "fail to create channel";

		change_nick((char *)"woody", me, &ctx.user_by_nick);

		char	*nicks[] = {
			(char *)"totoro", (char *)"buzz", (char *)"slinky",
			(char *)"bo", (char *)"jessy", (char *)"potato",
			(char *)"buster", (char *)"rex", (char *)"ham",
		};

		for (unsigned int i=1; i < 10; i++)
		{
			t_user	*user = create_user(i);
			t_list	*channel;
			char	*channel_name;

			if (push_list_node(user, &ctx.users) == -1)
				throw "fail to push_list_node";
			if (change_nick(nicks[i - 1], user, &ctx.user_by_nick) == -1)
				throw "fail to set nick";

			channel_name = (i % 3)
				? (char *)"room1"
				: (i % 2)
					? (char *)"room2"
					: NULL;

			if (channel_name == NULL)
				continue ;

			channel = (t_list *)get_hashmap(channel_name, &ctx.channels);

			user->channel = ft_strdup(channel_name);
			push_list_node(user, channel);
		}
	}

	void	TearDown(void)
	{
		clear_context(&ctx);
	}
};

TEST_F(DisconnectTest, channel_not_joined)
{
	char	nick[32];

	strcpy(nick, me->nick);

	// execute
	ASSERT_EQ(handle_disconnect(me, &ctx), 0);

	// test disconnect
	ASSERT_EQ(me->disconnect, 1);

	// test user_by_nick
	ASSERT_EQ(get_hashmap(nick, &ctx.user_by_nick), (void *)NULL);
}

TEST_F(DisconnectTest, channel_joined)
{
	char	*channel_name = (char *)"room1";
	t_list	*channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	size_t	prev_channel_len;
	char	nick[32];

	strcpy(nick, me->nick);

	// set channel joined
	me->channel = ft_strdup(channel_name);
	push_list_node(me, channel);
	prev_channel_len = channel->length;

	// execute
	ASSERT_EQ(handle_disconnect(me, &ctx), 0);

	// test disconnect
	ASSERT_EQ(me->disconnect, 1);

	// test channel
	ASSERT_EQ(channel->length, prev_channel_len - 1);
	ASSERT_EQ(list_has((void *)me, channel), 0);

	// test user_by_nick
	ASSERT_EQ(get_hashmap(nick, &ctx.user_by_nick), (void *)NULL);

	// test leave message
	t_list_node	*node = ctx.users.head;

	while (node)
	{
		t_user	*user = (t_user *)node->data;

		if (user->channel && ft_strcmp(user->channel, channel_name) == 0)
		{
			ASSERT_EQ(user->output_q.length, (t_uint64)1);
			t_uint8 *ack = (t_uint8 *)user->output_q.head->data;
			t_msg_hdr *hdr = (t_msg_hdr *)ack;

			ASSERT_EQ(hdr->type, MSG_TYPE_DISCONNECTACK);
			ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"[woody] disconnected");
		}
		else
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}
}