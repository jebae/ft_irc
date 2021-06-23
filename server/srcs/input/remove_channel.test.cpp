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

static void		copy_list(t_list *dest, t_list *src)
{
	t_list_node	*node;

	init_list(dest);
	node = src->head;
	while (node != NULL)
	{
		push_list_node(node->data, dest);
		node = node->next;
	}
}

static void		clear_channel_users(t_list *users)
{
	t_list_node	*node;
	t_list_node	*tmp;

	node = users->head;
	while (node)
	{
		tmp = node->next;
		ft_memdel((void **)&node);
		node = tmp;
	}
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

class RemoveChannelTest: public ::testing::Test
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

TEST_F(RemoveChannelTest, remove_only_me_existed_channel)
{
	// set channel
	char	*channel_name = (char *)"only_me";
	t_list	*channel;

	ASSERT_EQ(create_channel(channel_name, &ctx.channels), 0);
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);

	me->channel = ft_strdup(channel_name);
	push_list_node(me, channel);

	// execute
	ASSERT_EQ(handle_remove_channel(channel_name, me, &ctx), 0);

	// test channel removed
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	ASSERT_EQ(channel, (t_list *)NULL);

	// test user->channel
	ASSERT_EQ(me->channel, (char *)NULL);

	// test message
	t_list_node *node = ctx.users.head;
	char		*msg = (char *)"Channel [only_me] removed";

	while (node != NULL)
	{
		t_user *user = (t_user *)node->data;

		if (user == me)
		{
			ASSERT_EQ(user->output_q.length, (t_uint64)1);

			t_uint8 *ack = (t_uint8 *)user->output_q.head->data;
			t_msg_hdr *hdr = (t_msg_hdr *)ack;

			ASSERT_EQ(hdr->type, MSG_TYPE_REMOVE_CHANNELACK);
			ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), msg);
		}
		else
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}
}

TEST_F(RemoveChannelTest, remove_multi_user_existed_channel)
{
	// set channel
	char	*channel_name = (char *)"room1";
	t_list	*channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	t_list	channel_users;

	me->channel = ft_strdup(channel_name);
	push_list_node(me, channel);
	copy_list(&channel_users, channel);

	// execute
	ASSERT_EQ(handle_remove_channel(channel_name, me, &ctx), 0);

	// test channel removed
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	ASSERT_EQ(channel, (t_list *)NULL);

	// test user and message
	t_list_node *node = ctx.users.head;
	char		*msg = (char *)"Channel [room1] removed";

	while (node != NULL)
	{
		t_user *user = (t_user *)node->data;

		if (list_has(node->data, &channel_users))
		{
			ASSERT_EQ(user->channel, (char *)NULL);

			ASSERT_EQ(user->output_q.length, (t_uint64)1);

			t_uint8 *ack = (t_uint8 *)user->output_q.head->data;
			t_msg_hdr *hdr = (t_msg_hdr *)ack;

			ASSERT_EQ(hdr->type, MSG_TYPE_REMOVE_CHANNELACK);
			ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), msg);
		}
		else
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}

	// free
	clear_channel_users(&channel_users);
}

TEST_F(RemoveChannelTest, channel_not_exist)
{
	// execute
	ASSERT_EQ(handle_remove_channel((char *)"anywhere", me, &ctx), 0);

	// test message
	t_list_node *node = ctx.users.head;
	char		*msg = (char *)"Channel [anywhere] not exist";

	while (node != NULL)
	{
		t_user *user = (t_user *)node->data;

		if (user == me)
		{
			ASSERT_EQ(user->output_q.length, (t_uint64)1);

			t_uint8 *ack = (t_uint8 *)user->output_q.head->data;
			t_msg_hdr *hdr = (t_msg_hdr *)ack;

			ASSERT_EQ(hdr->type, MSG_TYPE_REMOVE_CHANNELACK);
			ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), msg);
		}
		else
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}
}