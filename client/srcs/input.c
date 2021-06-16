#include "client.h"

int		read_stdin(t_context *ctx)
{
	printf("read stdin\n");
	if (get_next_line(STDIN_FILENO, &ctx->user_input) == -1)
		return (error("fail to read line"));
	printf("input: %s\n", ctx->user_input);
	return (0);
}