#include "client.h"

int		send_msg(int sockfd)
{
	char	*line;
	int		res;

	if (get_next_line(STDIN_FILENO, &line) == -1)
		return (error("fail to read line"));
	res = 0;
	if (send(sockfd, line, ft_strlen(line) + 1, 0) == -1)
	{
		error("fail to send");
		res = -1;
	}
	ft_memdel((void **)&line);
	return (res);
}