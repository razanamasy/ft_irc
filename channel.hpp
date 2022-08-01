#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "user.hpp"
# include "message.hpp"
# include <iostream>
# include <list>
# define MAX_USER_PER_CHANNEL 10

class user;

class channel
{
	private:
		std::string			_name;
		user*				_operator;
		std::list<user*>	_users;
		size_t				nbr_of_users;
		std::string 		_topic;
		bool				_invite_only;
		std::list<user*>	_invite_list;

	public:

		// ITERATOR

		typedef std::list<user*>::iterator	user_iterator;
		
		// CTOR DTOR

		channel(const std::string& channel_name, user* op);

		~channel();
		
		// ACCESS

		std::string&		name();
		const std::string&	name() const;
		const user&			op() const;
		size_t				number_of_users() const;
		std::list<user*>&	users();
		std::string			topic();
		void				invite_only(bool);
		std::list<user*>&	invite_list();

		bool			is_op(const user& usr)	const;
		bool			is_empty() const;
		bool			is_full() const;
		user_iterator	search(const user& usr);
		bool			is_user_in_channel(user& usr);
		bool			is_invite_only();

		void	add_to_invite_list(user* usr);
		bool	is_invited(user* usr);
		void	remove_from_invite_list(user* usr);

		// MESSAGE
		
		void		send_a_message(message m, const user& usr);

		// USER

		bool		add_user(user& usr);
		bool		remove_user(const user& usr);
};

#endif /* CHANNEL_HPP */
