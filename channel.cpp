# include "channel.hpp"
# include <string> 

// CTOR DTOR

channel::channel(const std::string& channel_name, user* op)
{
	_name = channel_name;
	_operator= op;
	_users.push_front(op);
	nbr_of_users = 1;
	_invite_only = false;
}

channel::~channel(){};

// ACCESS

std::string channel::topic()
{
	return (this->_topic);
}

std::string&			channel::name()	{ 
		return _name; 
}

const std::string&		channel::name() const { 
		return _name; 
}

const user&				channel::op() const	{ 
		return *_operator; 
}

size_t	channel::number_of_users() const { 
		return nbr_of_users; 
}

std::list<user*>&	channel::users()
{ return _users; }

void	channel::invite_only(bool b)
{ _invite_only = b; }

bool	channel::is_invite_only()
{ return _invite_only; }

std::list<user*>&	channel::invite_list()
{ return _invite_list; }

void	channel::add_to_invite_list(user* usr)
{
	if (!this->is_invited(usr))
		_invite_list.push_back(usr);
}

bool	channel::is_invited(user* usr)
{
	channel::user_iterator	b = _invite_list.begin();
	channel::user_iterator	e = _invite_list.end();
	while (b != e)
	{
		if ((*b) == usr)
			return true;
		b++;
	}
	return false;
}

void	channel::remove_from_invite_list(user* usr)
{
	channel::user_iterator	b = _invite_list.begin();
	channel::user_iterator	e = _invite_list.end();
	while (b != e)
	{
		if ((*b) == usr)
			break ;
		b++;
	}
	if (b != e)
		_invite_list.erase(b);
}

bool	channel::is_op(const user& usr) const { 
		if (!_operator)
			return (false);
		return (*_operator == usr); 
}

bool	channel::is_empty() const
{
	return !nbr_of_users;
}

bool	channel::is_full() const
{ return (nbr_of_users >= MAX_USER_PER_CHANNEL); }


channel::user_iterator	channel::search(const user& usr)
{
	channel::user_iterator	b = _users.begin();
	channel::user_iterator	e = _users.end();
	while (b != e)
	{
		if (**b == usr)
			break ;
		++b;
	}
	return b;
}

bool	channel::is_user_in_channel(user& usr) { 
		return !(search(usr) == _users.end()); 
}

// MESSAGE

void	channel::send_a_message(message m, const user& usr)
{
	channel::user_iterator	b = _users.begin();
	channel::user_iterator	e = _users.end();
	while (b != e)
	{
		if (**b != usr)
			(**b).send_a_message(m);
		++b;
	}
}

// MODIFY USER

bool	channel::add_user(user& usr) 		// false: reply to JOIN, 471 ERR_CHANNELISFULL
{
	if (nbr_of_users == MAX_USER_PER_CHANNEL)
		return false;
	if (is_user_in_channel(usr))
		return true;
	this->remove_from_invite_list(&usr);
	usr.add_a_chan(this);
	_users.push_front(&usr);
	nbr_of_users += 1;
	return true;
}


bool	channel::remove_user(const user& usr) // false: reply to PART, 422 ERR_NOTONCHANNEL
{
	channel::user_iterator	el = search(usr);
	if (el == _users.end())
		return false;
	if (is_op(usr))
		_operator = NULL;
	_users.erase(el);
	nbr_of_users -= 1;
	return true;
}
