#include "client.h"

static int	get_connected_socket(t_uint16 port)
{
	static char			*ip = "127.0.0.1";
	int					sockfd;
	struct sockaddr_in	addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return (error("fail to create socket"));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (connect(sockfd,
		(struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(sockfd);
		return (error("fail to connect server"));
	}
	printf("connected\n");
	return (sockfd);
}

static int	loop_io(int sockfd)
{
	fd_set				rset;
	struct timeval		timeout;
	t_uint8				buf[512];

	while (1)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		FD_ZERO(&rset);
		FD_SET(STDIN_FILENO, &rset);
		FD_SET(sockfd, &rset);
		if (select(sockfd + 1, &rset, NULL, NULL, &timeout) == -1)
			return (error("fail to select"));
		if (FD_ISSET(0, &rset))
			send_msg(sockfd);
		if (FD_ISSET(sockfd, &rset))
		{
			recv(sockfd, buf, 512, 0);
			printf("server said: %s\n", buf);
		}
	}
	return (0);
}

int			run_client(t_uint16 port)
{
	int	sockfd;
	int	res;

	if ((sockfd = get_connected_socket(port)) == -1)
		return (-1);
	res = loop_io(sockfd);
	return (res);
}