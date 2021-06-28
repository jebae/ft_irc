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

class ChannelsTest: public ::testing::Test
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
	}

	void	TearDown(void)
	{
		clear_context(&ctx);
	}
};

TEST_F(ChannelsTest, no_channels)
{
	// execute
	ASSERT_EQ(handle_channels(me, &ctx), 0);

	// test message sent
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CHANNELSACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"No channels");
}

TEST_F(ChannelsTest, one_channel_exist)
{
	char	*channel_name = (char *)"room1";

	// set channel
	create_channel(channel_name, &ctx.channels);

	// execute
	ASSERT_EQ(handle_channels(me, &ctx), 0);

	// test message sent
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CHANNELSACK);
	ASSERT_NE(strstr((char *)ack + sizeof(t_msg_hdr), channel_name), (char *)NULL);
}

TEST_F(ChannelsTest, multi_channel_exist)
{
	const char	*channels[] = {
		"room1", "room2", "room3", "room4",
	};

	// set channel
	for (unsigned int i=0; i < sizeof(channels) / sizeof(char *); i++)
	{
		create_channel((char *)channels[i], &ctx.channels);
	}

	// execute
	ASSERT_EQ(handle_channels(me, &ctx), 0);

	// test message sent
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_CHANNELSACK);

	for (unsigned int i=0; i < sizeof(channels) / sizeof(char *); i++)
	{
		ASSERT_NE(
			strstr((char *)ack + sizeof(t_msg_hdr), (char *)channels[i]), (char *)NULL);
	}
}