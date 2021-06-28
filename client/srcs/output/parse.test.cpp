#include <gtest/gtest.h>
#include "client.h"

using namespace std;

class ParseUserInput: public ::testing::Test
{
protected:
	t_uint8		*msg;
	char		*input;

	void	SetUp(void)
	{
		input = NULL;
		msg = NULL;
	}

	void	TearDown(void)
	{
		if (msg != NULL)
			free(msg);
		if (input != NULL)
			free(input);
	}
};

TEST_F(ParseUserInput, nick)
{
	input = strdup((char *)"/nick buzz");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr) + 5);

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_NICK);
	ASSERT_EQ(hdr->size, (t_uint64)5); // buzz

	// test payload
	t_uint8	*payload = msg + sizeof(t_msg_hdr);

	ASSERT_STREQ((char *)payload, (char *)"buzz");
}

TEST_F(ParseUserInput, create_channel)
{
	input = strdup((char *)"/create_channel toy_story");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr) + ft_strlen("toy_story") + 1);

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_CREATE_CHANNEL);
	ASSERT_EQ(hdr->size, ft_strlen("toy_story") + 1);

	// test payload
	t_uint8	*payload = msg + sizeof(t_msg_hdr);

	ASSERT_STREQ((char *)payload, (char *)"toy_story");
}

TEST_F(ParseUserInput, leave)
{
	input = strdup((char *)"/leave");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr));

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_LEAVE);
	ASSERT_EQ(hdr->size, (t_uint64)0);
}

TEST_F(ParseUserInput, remove_channel)
{
	input = strdup((char *)"/remove_channel toy_story");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr) + ft_strlen("toy_story") + 1);

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_REMOVE_CHANNEL);
	ASSERT_EQ(hdr->size, ft_strlen("toy_story") + 1);

	// test payload
	t_uint8	*payload = msg + sizeof(t_msg_hdr);

	ASSERT_STREQ((char *)payload, (char *)"toy_story");
}

TEST_F(ParseUserInput, join)
{
	input = strdup((char *)"/join toy_story");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr) + ft_strlen("toy_story") + 1);

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_JOIN);
	ASSERT_EQ(hdr->size, ft_strlen("toy_story") + 1);

	// test payload
	t_uint8	*payload = msg + sizeof(t_msg_hdr);

	ASSERT_STREQ((char *)payload, (char *)"toy_story");
}

TEST_F(ParseUserInput, msg_to_channel)
{
	input = strdup((char *)"/msg hi guys");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr) + ft_strlen("hi guys") + 1);

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_CHANNEL_CHAT);
	ASSERT_EQ(hdr->size, ft_strlen("hi guys") + 1);

	// test payload
	t_uint8	*payload = msg + sizeof(t_msg_hdr);

	ASSERT_STREQ((char *)payload, (char *)"hi guys");
}

TEST_F(ParseUserInput, msg_to_user)
{
	input = strdup((char *)"/msg #woody welcome aboard");
	t_chat_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_chat_hdr)
		+ ft_strlen("woody")
		+ ft_strlen("welcome aboard") + 2);

	// test header
	hdr = (t_chat_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_DIRECT_CHAT);
	ASSERT_EQ(hdr->size, ft_strlen("woody")
		+ ft_strlen("welcome aboard") + 2);

	// test payload
	t_uint8	*payload = msg + sizeof(t_chat_hdr);

	ASSERT_STREQ((char *)payload, (char *)"woody");
	ASSERT_STREQ((char *)payload + hdr->content_offset,
		(char *)"welcome aboard");
}

TEST_F(ParseUserInput, msg_to_user_without_nick)
{
	input = strdup((char *)"/msg # welcome aboard");
	t_uint64	size;

	// execute
	::testing::internal::CaptureStdout();
	size = parse_user_input(input, &msg);
	string output = ::testing::internal::GetCapturedStdout();

	// test message size
	ASSERT_EQ(size, (t_uint64)0);

	// test info
	ASSERT_NE(strstr(output.c_str(), (char *)"Wrong format\nUsage: /chat #[nick] [content]"), (char *)NULL);
}

TEST_F(ParseUserInput, msg_to_user_without_chat)
{
	input = strdup((char *)"/msg #woody   ");
	t_uint64	size;

	// execute
	::testing::internal::CaptureStdout();
	size = parse_user_input(input, &msg);
	string output = ::testing::internal::GetCapturedStdout();

	// test message size
	ASSERT_EQ(size, (t_uint64)0);

	// test info
	ASSERT_NE(strstr(output.c_str(), (char *)"Wrong format\nUsage: /chat #[nick] [content]"), (char *)NULL);
}

TEST_F(ParseUserInput, input_required)
{
	input = strdup((char *)"/nick");
	t_uint64	size;

	// execute
	::testing::internal::CaptureStdout();
	size = parse_user_input(input, &msg);
	string output = ::testing::internal::GetCapturedStdout();

	// test message size
	ASSERT_EQ(size, (t_uint64)0);

	// test info
	ASSERT_NE(strstr(output.c_str(), (char *)"Input required"), (char *)NULL);
}

TEST_F(ParseUserInput, command_required)
{
	input = strdup((char *)"nick");
	t_uint64	size;

	// execute
	::testing::internal::CaptureStdout();
	size = parse_user_input(input, &msg);
	string output = ::testing::internal::GetCapturedStdout();

	// test message size
	ASSERT_EQ(size, (t_uint64)0);

	// test info
	ASSERT_NE(strstr(output.c_str(), (char *)"Command required"), (char *)NULL);
}

TEST_F(ParseUserInput, where_am_i)
{
	input = strdup((char *)"/where_am_i");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr));

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_WHERE_AM_I);
	ASSERT_EQ(hdr->size, (t_uint64)0);
}

TEST_F(ParseUserInput, who)
{
	input = strdup((char *)"/who");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr));

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_WHO);
	ASSERT_EQ(hdr->size, (t_uint64)0);
}

TEST_F(ParseUserInput, channels)
{
	input = strdup((char *)"/channels");
	t_msg_hdr	*hdr;
	t_uint64	size;

	// execute
	size = parse_user_input(input, &msg);

	// test message size
	ASSERT_EQ(size, sizeof(t_msg_hdr));

	// test header
	hdr = (t_msg_hdr *)msg;
	ASSERT_EQ(hdr->type, MSG_TYPE_CHANNELS);
	ASSERT_EQ(hdr->size, (t_uint64)0);
}