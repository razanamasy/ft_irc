#include "server.hpp"

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

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Error nb arguments" <<std::endl;
        return (1);
    }

    server irc_serv(atoi(argv[1]));

    irc_serv.generate_key(ft_strlen(argv[2]));
    irc_serv.cipher_password(argv[2]);

    ft_bzero(argv[2], ft_strlen(argv[2]));

    irc_serv.listen_all_socks();
    return (0);
}
