#include "client.h"

int	error(char *msg)
{
	printf("[ERROR] %s\n", msg);
	return (-1);
}

int	info(char *msg)
{
	printf("[INFO] %s\n", msg);
	return (0);
}