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

class WhereAmITest: public ::testing::Test
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

TEST_F(WhereAmITest, channel_joined)
{
	char	*channel = (char *)"ch1";

	// set joined channel
	me->channel = ft_strdup(channel);

	// execute
	ASSERT_EQ(handle_where_am_i(me), 0);

	// test ack
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_WHERE_AM_IACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"[ch1]");
}

TEST_F(WhereAmITest, channel_not_joined)
{
	// execute
	ASSERT_EQ(handle_where_am_i(me), 0);

	// test ack
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr->type, MSG_TYPE_WHERE_AM_IACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You are not in any channel");
}