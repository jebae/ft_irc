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

class LeaveTest: public ::testing::Test
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

TEST_F(LeaveTest, leave_only_me_existed_channel)
{
	char	*channel_name = (char *)"only_me";
	t_list	*channel;

	// set channel
	create_channel(channel_name, &ctx.channels);
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);

	// set join
	me->channel = ft_strdup(channel_name);
	push_list_node(me, channel);

	ASSERT_EQ(channel->length, (t_uint64)1);
	ASSERT_EQ(channel->head->data, (void *)me);

	// execute
	ASSERT_EQ(handle_leave(me, &ctx), 0);

	// test user->channel
	ASSERT_EQ(me->channel, (char *)NULL);

	// test channel
	ASSERT_EQ(channel->length, (t_uint64)0);
	ASSERT_EQ(channel->head, (t_list_node *)NULL);
	ASSERT_EQ(channel->tail, (t_list_node *)NULL);

	// test msg
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_LEAVE_ACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You left [only_me]");

	// test other user's msg
	t_list_node	*node = ctx.users.head;

	while (node)
	{
		t_user	*user = (t_user *)node->data;

		if (user != me)
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}
}

TEST_F(LeaveTest, leave_multiple_user_existed_channel)
{
	char		*channel_name = (char *)"room1";
	t_list		*channel;
	t_uint64	prev_len;

	// set channel
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);

	// set join
	me->channel = ft_strdup(channel_name);
	push_list_node(me, channel);

	prev_len = channel->length;

	// execute
	ASSERT_EQ(handle_leave(me, &ctx), 0);

	// test user->channel
	ASSERT_EQ(me->channel, (char *)NULL);

	// test channel
	ASSERT_EQ(channel->length, prev_len - 1);

	t_list_node	*node;

	node = channel->head;
	while (node)
	{
		t_user *user = (t_user *)node->data;

		ASSERT_NE(user, me);
		node = node->next;
	}

	// test msg
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_LEAVE_ACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You left [room1]");

	// test other user's msg
	node = ctx.users.head;

	while (node)
	{
		t_user	*user = (t_user *)node->data;

		if (user != me)
		{
			if (user->channel == NULL)
				ASSERT_EQ(user->output_q.length, (t_uint64)0);
			else if (ft_strcmp(user->channel, channel_name) == 0)
			{
				ASSERT_EQ(user->output_q.length, (t_uint64)1);

				ack = (t_uint8 *)user->output_q.head->data;
				hdr = (t_msg_hdr *)ack;

				ASSERT_EQ(hdr->type, MSG_TYPE_LEAVE_ACK);
				ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"[woody] left [room1]");
			}
			else
				ASSERT_EQ(user->output_q.length, (t_uint64)0);
		}
		node = node->next;
	}
}

TEST_F(LeaveTest, no_channel_joined)
{
	// execute
	ASSERT_EQ(handle_leave(me, &ctx), 0);

	// test user->channel
	ASSERT_EQ(me->channel, (char *)NULL);

	// test msg
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_LEAVE_ACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You are not in any channel");
}