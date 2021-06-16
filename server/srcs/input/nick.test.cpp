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

class NickTest: public ::testing::Test
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

TEST_F(NickTest, new_nickname)
{
	char	*payload = (char *)"buzz";

	// execute
	ASSERT_EQ(nick(me, payload, &ctx), 0);

	// test nickname
	ASSERT_STREQ(me->nick, payload);

	// test user_by_nick
	t_user *user = (t_user *)get_hashmap(payload, &ctx.user_by_nick);
	ASSERT_EQ(user, me);

	// test output_q
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;
	ASSERT_EQ(hdr->type, MSG_TYPE_NICKACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"Your nickname: buzz");
}

TEST_F(NickTest, change_nickname)
{
	char	*prev = (char *)"woody";
	char	*payload = (char *)"buzz";

	// set previous nickname
	me->nick = ft_strdup(prev);
	set_hashmap(ft_strdup(me->nick), me, &ctx.user_by_nick);

	// execute
	ASSERT_EQ(nick(me, payload, &ctx), 0);

	// test nickname
	ASSERT_STREQ(me->nick, payload);

	// test user_by_nick
	t_user *user = (t_user *)get_hashmap(payload, &ctx.user_by_nick);
	ASSERT_EQ(user, me);

	user = (t_user *)get_hashmap(prev, &ctx.user_by_nick);
	ASSERT_EQ(user, (t_user *)NULL);

	// test output_q
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;
	ASSERT_EQ(hdr->type, MSG_TYPE_NICKACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"Your nickname: buzz");
}

TEST_F(NickTest, nickname_already_exist)
{
	char	*payload = (char *)"buzz";
	t_user	*user;

	// set already exist user
	user = create_user(1);
	ASSERT_NE(user, (t_user *)NULL);
	ASSERT_NE(push_list_node(user, &ctx.users), -1);
	set_hashmap(ft_strdup(payload), user, &ctx.user_by_nick);

	// execute
	ASSERT_EQ(nick(me, payload, &ctx), 0);

	// test nickname
	ASSERT_EQ(me->nick, (char *)NULL);

	// test user_by_nick
	t_user *target = (t_user *)get_hashmap(payload, &ctx.user_by_nick);
	ASSERT_EQ(target, user);

	// test output_q
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;
	ASSERT_EQ(hdr->type, MSG_TYPE_NICKACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"Nickname already exist");
}

TEST_F(NickTest, nickname_too_long)
{
	char	*payload = (char *)"qwerqwerqwerqwerqwerqwerqwerqwer";

	// execute
	ASSERT_EQ(nick(me, payload, &ctx), 0);

	// test nickname
	ASSERT_STREQ(me->nick, (char *)NULL);

	// test user_by_nick
	t_user *user = (t_user *)get_hashmap(payload, &ctx.user_by_nick);
	ASSERT_EQ(user, (t_user *)NULL);

	// test output_q
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr = (t_msg_hdr *)ack;
	ASSERT_EQ(hdr->type, MSG_TYPE_NICKACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"Nickname too long");
}