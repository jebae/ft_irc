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

class MsgToChannelTest: public ::testing::Test
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

TEST_F(MsgToChannelTest, user_joined_channel)
{
	char		*payload = (char *)"toy story5 will be come";

	// set joined
	char		*channel_name = (char *)"room1";
	t_list		*channel;

	me->channel = ft_strdup(channel_name);
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	push_list_node(me, channel);

	// execute
	ASSERT_EQ(handle_msg_to_channel(payload, me, &ctx), 0);

	// test message broadcasted
	t_list_node	*node;
	t_uint8		*relay;
	t_uint8		*content;
	t_chat_hdr	*hdr;

	node = ctx.users.head;
	while (node)
	{
		t_user	*user = (t_user *)node->data;

		if (user->channel && ft_strcmp(user->channel, channel_name) == 0 && user != me)
		{
			ASSERT_EQ(user->output_q.length, (t_uint64)1);
			relay = (t_uint8 *)user->output_q.head->data;
			hdr = (t_chat_hdr *)relay;
			content = relay + sizeof(t_chat_hdr);

			ASSERT_EQ(hdr->type, MSG_TYPE_CHAT_RELAY);
			ASSERT_STREQ((char *)content, me->nick);
			ASSERT_STREQ((char *)content + hdr->content_offset , payload);
		}
		else
			ASSERT_EQ(user->output_q.length, (t_uint64)0);
		node = node->next;
	}
}

TEST_F(MsgToChannelTest, user_not_joined_channel)
{
	char		*payload = (char *)"toy story5 will be come";

	// execute
	ASSERT_EQ(handle_msg_to_channel(payload, me, &ctx), 0);

	// test message to user
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CHATACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You are not in any channel");
}

TEST_F(MsgToChannelTest, user_does_not_have_nick)
{
	char		*payload = (char *)"toy story5 will be come";

	// set joined
	char		*channel_name = (char *)"room1";
	t_list		*channel;

	me->channel = ft_strdup(channel_name);
	channel = (t_list *)get_hashmap(channel_name, &ctx.channels);
	push_list_node(me, channel);

	// set nick NULL
	ft_memdel((void **)&me->nick);

	// execute
	ASSERT_EQ(handle_msg_to_channel(payload, me, &ctx), 0);

	// test message to user
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CHATACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You need to set nickname");
}
