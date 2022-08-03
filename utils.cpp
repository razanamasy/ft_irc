#include <stdlib.h>
#include <unistd.h>
#include "utils.hpp" 

int 	serv_socket;
fd_set 	set;

int ft_strlen(char *str)
{
    size_t len;

    if (str == NULL)
        return (0);
    len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return (len);
}

void ft_bzero(char * ptr, int len)
{
    for (int i = 0 ; i < len ; i++)
    {
        ptr[i] = 0;
    }
}

void clear_all_socks()
{
    close(serv_socket);
	for (int i = 0; i < 1024; i++)
	{
		if (FD_ISSET(i, &set))
		{
			FD_CLR(i, &set);
			close(i);
		}
	}
	FD_ZERO(&set);
}


