#ifndef USER_HPP
# define USER_HPP
# include "channel.hpp"
# include "message.hpp"
# include <list>
# include <unistd.h>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# define MAX_CHANNEL_PER_USER 10

class channel;

std::string	to_lower_string(std::string str);

class user
{
	private:
		std::string			_nickname;
		std::string			_username;
		std::string			_realname;
		std::string			_servername;
		std::list<channel*>	_channel;
		std::string			buffer;
		const int			fd_user;
		const std::string	_hostname;
		size_t				in_channels;
		bool				passed;

	public:
		//GETTER
		std::string				nickname() const;
		std::string				username() const;
		std::string				hostname() const;
		std::string				realname() const;
		std::string				servername() const;
		int						getfd() const;
		int						get_in_channels() const;
		channel*				get_last_channel() const;
		std::list<channel*>&	get_channels();

		//setter
		void	nickname(const std::string& nick);
		void	username(const std::string& usr);
		void	realname(const std::string& real);
		void	servername(const std::string& srv);

		std::string	to_prefix() const;

		bool	can_join_a_channel() const;
		bool	add_a_chan(channel *newChan);

		user(const int fd, std::string hn);

		bool	is_passed();
		void	pass();

		~user();
		bool	operator==(const user& usr) const;
		bool	operator!=(const user& usr) const;
		void	add_to_buffer(message msg);
		void	send_buffer();
		void	send_a_message(message& m) const;
		void	send_a_string(const std::string& m) const;
		bool remove_a_chan(channel chan);
		void	send_to_all_known_users(message m);
};

#endif
