#include "server.h"

int		main(int argc, char **argv)
{
	t_uint16	port;

	(void)argc;
	port = ft_atoi(argv[1]);
	return (run_server(port));
}