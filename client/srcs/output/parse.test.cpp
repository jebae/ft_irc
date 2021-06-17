#include <gtest/gtest.h>
#include "client.h"

class ParseUserInput: public ::testing::Test
{
protected:
	t_uint8		*msg;

	void	SetUp(void)
	{
		msg = NULL;
	}

	void	TearDown(void)
	{
		if (msg != NULL)
			free(msg);
	}
};

TEST_F(ParseUserInput, nick)
{
	char		*input = (char *)"/nick buzz";
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