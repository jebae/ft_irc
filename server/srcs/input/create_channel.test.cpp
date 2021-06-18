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

class CreateChannelTest: public ::testing::Test
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

		for (unsigned int i=1; i < 10; i++)
		{
			t_user	*user = create_user(i);

			if (push_list_node(user, &ctx.users) == -1)
				throw "fail to push_list_node";
		}
	}

	void	TearDown(void)
	{
		clear_context(&ctx);
	}
};

TEST_F(CreateChannelTest, new_channel)
{
	char	*payload = (char *)"toy_story";

	// execute
	ASSERT_EQ(handle_create_channel(payload, me, &ctx), 0);

	// test channel created
	t_list	*channel_users = (t_list *)get_hashmap(payload, &ctx.channels);

	ASSERT_NE(channel_users, (t_list *)NULL);
	ASSERT_EQ(channel_users->length, (t_uint64)0);

	// test message broadcasted
	t_list_node *node = ctx.users.head;
	char		*msg = (char *)"Channel [toy_story] created";

	while (node != NULL)
	{
		t_user *user = (t_user *)node->data;

		ASSERT_EQ(user->output_q.length, (t_uint64)1);

		t_uint8 *ack = (t_uint8 *)user->output_q.head->data;
		t_msg_hdr *hdr = (t_msg_hdr *)ack;

		ASSERT_EQ(hdr->type, MSG_TYPE_CREATE_CHANNELACK);
		ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), msg);
		node = node->next;
	}
}

TEST_F(CreateChannelTest, channel_already_exist)
{
	char	*payload = (char *)"toy_story";

	// set already exist
	t_list	*channel_users = (t_list *)ft_memalloc(sizeof(t_list));

	init_list(channel_users);
	set_hashmap(ft_strdup(payload), channel_users, &ctx.channels);

	// execute
	ASSERT_EQ(handle_create_channel(payload, me, &ctx), 0);

	// test channel same with before execute
	ASSERT_EQ(get_hashmap(payload, &ctx.channels), channel_users);

	// test message
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CREATE_CHANNELACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), "Channel already exist");
}

TEST_F(CreateChannelTest, channel_name_too_long)
{
	char	*payload = (char *)"asdfasdfsdfasdfasdfasdfasdfasdfasdf";

	// execute
	ASSERT_EQ(handle_create_channel(payload, me, &ctx), 0);

	// test channel not created
	ASSERT_EQ(get_hashmap(payload, &ctx.channels), (void *)NULL);

	// test message
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CREATE_CHANNELACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), "Channel name too long");
}