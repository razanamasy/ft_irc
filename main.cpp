#include "server.hpp"
#include "utils.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Must be this format :" <<std::endl;
        std::cout << "./ircserv [port number] [password]:" <<std::endl;
        return (1);
    }

    server irc_serv(atoi(argv[1]));

    irc_serv.generate_key(ft_strlen(argv[2]));
    irc_serv.cipher_password(argv[2]);

    ft_bzero(argv[2], ft_strlen(argv[2]));

    try 
    {
        irc_serv.listen_all_socks();
    }
    catch (server::SocketError exp)
    {
        std::cerr << exp.what() << std::endl;

        clear_all_socks();
    }
    return (0);
}
