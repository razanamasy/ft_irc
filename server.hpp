#ifndef SERVER_HPP
# define SERVER_HPP
# include "channel.hpp"
# include "replies.hpp"
# include <arpa/inet.h>
# include <unistd.h>
# include <netinet/in.h>
# include <netdb.h> 
# include <map>
# include <list>
# include <vector>
# include <utility>
# include <algorithm>
# include <stdio.h>
# include <signal.h>
# define MAX_USERS 10
# define MAX_CHANNEL 10

void	handle_interruption(int sig);
void	ft_bzero(char * ptr, int len);

class	server
{
	private:
		std::list<user>				_users;
		std::list<channel>			_channels;
		size_t						nbr_of_users;
		size_t						nbr_of_channels;
		int							port;
		std::string					key;
		std::string					hash;
		struct sockaddr_in			addr_client;
		std::map<int, std::string>	fd_buffer;
		

	public:
		typedef std::list<channel>::iterator	chan_iterator;
		typedef std::list<user>::iterator	user_iterator;

		int		ini_server(int port);
		int     problem_on_client(int fdcur);
		int		parcing_manage_request(int fdcur);
		int		new_connection();
		int 	request_from_client(int fdcur);
		int		listen_all_socks();
		bool	uncorrect_param(std::string str);
		int		manage(int fd, message msg);
		void 	load_local_serv_ip(sockaddr_in & addr, int port);

		server(int port);
		std::string getkey() const ;
		std::string gethash() const ;

		~server();

		// ACCESSORS

		size_t	number_of_users()		const ;
		size_t	number_of_channels()	const ;

		void setkey(std::string key);
		void sethash(std::string hash);

		std::list<user>&	Users();
		std::list<channel>&	channels();



		//COMMANDS

		int	pass_cmd(user& usr, std::list<std::string> list_param);
		int	nick_cmd(user& usr, std::list<std::string> list_param);
		int	user_cmd(user& usr, std::list<std::string> list_param);
		int	mode_cmd(user& usr, std::list<std::string>	_params);
		int	kick_cmd(user& asskicker, std::list<std::string> list_param);
		int	ping_cmd(user& usr);
		int	join_cmd(user& usr, std::list<std::string> list_param);
		int	who_cmd(user& usr, std::list<std::string> list_param);
		int	privmsg_cmd(user& sender, std::list<std::string> list_param, std::string cmd_name);
		int	part_cmd(user& usr, std::list<std::string> list_param);
		int	quit_cmd(user& usr, std::list<std::string> list_param);
		int	notice_cmd(user& usr, std::list<std::string> list_param);
		int topic_cmd(user &usr, std::list<std::string> list_param);
		int	invite_cmd(user& usr, std::list<std::string> list_param);

		//A SORTIR DANS UTILS COMMANDS
		//UTILS FOR JOIN COMMANDS
		int	join_each_channel(user& usr, std::string chan_name);
		void	create_channel(user& usr, std::string chan_name, std::list<std::string> list_param, chan_iterator& chan);
		int	fill_channel(user& usr, std::string chan_name, chan_iterator& chan);
		void	set_up_messages(message& m, std::string chan_name, chan_iterator& chan, user& usr);

		//UTILS FOR OTHER COMMANDS
		void	kick_each_user(user& asskicker, std::vector<std::string>::iterator b, std::list<std::string> list_param);
		void	part_each_channel(user& usr, std::vector<std::string>::iterator	b, std::list<std::string> list_param);
		void	msg_each_receivers(user& sender, std::vector<std::string>::iterator b,
			       std::list<std::string> list_param, std::string cmd_name);
		template <class T>
		T	at(std::list<T> lst, int index)
		{
			typename std::list<T>::iterator	_b = lst.begin();
			typename std::list<T>::iterator	_e = lst.end();

			if (index < 0)
				return *(lst.end());

			int size = 0;
			for (; _b != _e; _b++)
			{
				size++;
			}

			if (index > size)
				return *(lst.end());

			_b = lst.begin();
			_e = lst.end();

			for (int i = 0; i < index; i++)
				_b++;
			return (*_b);
		}

		// CHANS
		void	add_channel(channel& chan);
		void	remove_channel(server::chan_iterator chan);
		void	pop_user_from_chan(user& usr, chan_iterator it_chan);
		chan_iterator	search_channel(std::string name);
		bool	channel_exists(std::string name);

		// USERS

		void	add_user(const user& usr);
		void	remove_user(user_iterator ui);
		user_iterator	search_user(const std::string& nick);
		user_iterator get_user_by_fd(int fd);
		bool	is_ip_already_here(std::string ip);
		bool	user_exists(const std::string& nick);
		bool	user_exists(const int sockfd);

		// COMMANDS
		
		bool	join(const user& usr, std::string chan);
		bool	nick(user& usr, const std::string new_nick);
		bool	kick(const user& asskicker, user& asskicked, channel& chan);
		bool	User(	const std::string username,
						const std::string hostname,
						const std::string servername, 
						const std::string realname		);
		void	quit(user& usr, std::string msg);
		void	pong(user& usr);

		//CRYPTO
		void    generate_key(size_t len);
		void    cipher_password(std::string password);
		std::string cipher_test_password(std::string password, std::string key);
		bool    password_match(std::string candidate, std::string hash, std::string key);
};

std::ostream&	operator<<(std::ostream& o, channel& chan);
std::ostream&	operator<<(std::ostream& o, user& usr);
std::ostream&	operator<<(std::ostream& o, server& serv);

#endif
