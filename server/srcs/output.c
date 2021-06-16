#include "server.h"

/*
** TODO
** send with user->wbuf
*/
int		handle_output(t_user *user, t_context *ctx)
{
	(void)user;
	(void)ctx;
	return (0);
}

int		enqueue_output(
	t_uint8 type, t_uint8 *payload, t_uint64 size, t_user *user)
{
	t_uint8		*msg;
	t_msg_hdr	*hdr;

	msg = (t_uint8 *)ft_memalloc(size + sizeof(t_msg_hdr));
	if (msg == NULL)
		return (-1);
	hdr = (t_msg_hdr *)msg;
	hdr->type = type;
	hdr->size = size;
	ft_memcpy(msg + sizeof(t_msg_hdr), payload, size);
	if (push_list_node(msg, &user->output_q) == -1)
	{
		ft_memdel((void **)&msg);
		return (error((char *)"fail to push output msg"));
	}
	return (0);
}