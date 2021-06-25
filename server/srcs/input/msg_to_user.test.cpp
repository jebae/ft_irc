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

class MsgToUserTest: public ::testing::Test
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

TEST_F(MsgToUserTest, receiver_exist)
{
	char		*receiver = (char *)"buzz";
	char		*content = (char *)"toy story5 will be come";
	t_uint8		*payload = (t_uint8 *)ft_memalloc(
		ft_strlen(receiver) + ft_strlen(content) + 2);
	t_chat_hdr	hdr;

	// set input
	hdr.size = ft_strlen(receiver) + ft_strlen(content) + 2;
	hdr.type = MSG_TYPE_DIRECT_CHAT;
	hdr.content_offset = ft_strlen(receiver) + 1;
	ft_strcpy((char *)payload, receiver);
	ft_strcpy((char *)payload + hdr.content_offset, content);

	// execute
	ASSERT_EQ(handle_msg_to_user(&hdr, (t_uint8 *)payload, me, &ctx), 0);

	// test message sent
	t_uint8		*relay;
	t_chat_hdr	*hdr_relay;
	char		*content_relay;
	t_user		*user = (t_user *)get_hashmap(receiver, &ctx.user_by_nick);

	ASSERT_EQ(user->output_q.length, (t_uint64)1);
	relay = (t_uint8 *)user->output_q.head->data;
	hdr_relay = (t_chat_hdr *)relay;
	content_relay = (char *)(relay + sizeof(t_chat_hdr));

	ASSERT_EQ(hdr_relay->type, MSG_TYPE_CHAT_RELAY);
	ASSERT_EQ(hdr_relay->size, ft_strlen(me->nick) + ft_strlen(content) + 2);
	ASSERT_EQ(hdr_relay->content_offset, ft_strlen(me->nick) + 1);
	ASSERT_STREQ(content_relay, me->nick);
	ASSERT_STREQ(content_relay + hdr_relay->content_offset , content);

	free(payload);
}

TEST_F(MsgToUserTest, receiver_not_exist)
{
	char		*receiver = (char *)"who";
	char		*content = (char *)"toy story5 will be come";
	t_uint8		*payload = (t_uint8 *)ft_memalloc(
		ft_strlen(receiver) + ft_strlen(content) + 2);
	t_chat_hdr	hdr;

	// set input
	hdr.size = ft_strlen(receiver) + ft_strlen(content) + 2;
	hdr.type = MSG_TYPE_DIRECT_CHAT;
	hdr.content_offset = ft_strlen(receiver) + 1;
	ft_strcpy((char *)payload, receiver);
	ft_strcpy((char *)payload + hdr.content_offset, content);

	// execute
	ASSERT_EQ(handle_msg_to_user(&hdr, (t_uint8 *)payload, me, &ctx), 0);

	// test message ack
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr_ack = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr_ack->type, MSG_TYPE_CHATACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"User [who] not exist");

	free(payload);
}

TEST_F(MsgToUserTest, user_does_not_have_nick)
{
	char		*receiver = (char *)"buzz";
	char		*content = (char *)"toy story5 will be come";
	t_uint8		*payload = (t_uint8 *)ft_memalloc(
		ft_strlen(receiver) + ft_strlen(content) + 2);
	t_chat_hdr	hdr;

	// set input
	hdr.size = ft_strlen(receiver) + ft_strlen(content) + 2;
	hdr.type = MSG_TYPE_DIRECT_CHAT;
	hdr.content_offset = ft_strlen(receiver) + 1;
	ft_strcpy((char *)payload, receiver);
	ft_strcpy((char *)payload + hdr.content_offset, content);

	// delete nick
	ft_memdel((void **)&me->nick);

	// execute
	ASSERT_EQ(handle_msg_to_user(&hdr, (t_uint8 *)payload, me, &ctx), 0);

	// test message ack
	ASSERT_EQ(me->output_q.length, (t_uint64)1);

	t_uint8 *ack = (t_uint8 *)me->output_q.head->data;
	t_msg_hdr *hdr_ack = (t_msg_hdr *)ack;

	ASSERT_EQ(hdr_ack->type, MSG_TYPE_CHATACK);
	ASSERT_STREQ((char *)ack + sizeof(t_msg_hdr), (char *)"You need to set nickname");

	free(payload);
}