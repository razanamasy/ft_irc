#include "user.hpp"

//OUT OF MEMEBER FUNCTION UTILS FOR USER
std::string	to_lower_string(std::string str)
{
	std::string	res(str.length(), ' ');

	for (size_t i = 0 ; i < str.length(); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			res[i] = str[i] + 32;
		else
			res[i] = str[i];
	}
	return res;
}

//getters
std::string	user::nickname() const	{ return _nickname; };
std::string	user::username() const	{ return _username; };
std::string	user::hostname() const	{ return _hostname; };
std::string	user::realname() const	{ return _realname; };
std::string	user::servername() const	{ return _servername; };
int		user::getfd() const	{ return fd_user; };
int		user::get_in_channels() const { return in_channels; };

channel*		user::get_last_channel() const 
{
	if (this->_channel.empty())
		return (NULL); 
	return _channel.back();
};

std::list<channel*>&	user::get_channels()
{ return _channel; }

//setter
void	user::nickname(const std::string& nick)		{ _nickname = nick; };
void	user::username(const std::string& usr)		{ _username = usr; };
void	user::realname(const std::string& real)		{ _realname = real; };
void	user::servername(const std::string& srv)	{ _servername = srv; };

std::string	user::to_prefix() const
{
	std::string	res = _nickname + '!' + _username + '@' + _hostname;
	return res;
}

bool	user::can_join_a_channel() const	{ return in_channels < MAX_CHANNEL_PER_USER; }

bool	user::add_a_chan(channel *newChan)
{
	if (this->in_channels >= MAX_CHANNEL_PER_USER)
		return false;
	in_channels += 1;
	this->_channel.push_back(newChan);
	//send_a_string(raw_join_messages);
	return true;
}

bool user::remove_a_chan(channel chan)
{
	if ((this->_channel).empty())
		return true;
	std::list<channel *>::iterator b = this->_channel.begin();
	std::list<channel *>::iterator e = this->_channel.end();
	while (b != e)
	{
		if ((*b)->name() == chan.name())
			break;
		b++;
	}
	if (b != e)
	{
		(this->_channel).erase(b);
		in_channels -= 1;
	}
	return (true);
}

user::user(const int fd, std::string hn): fd_user(fd), _hostname(hn)
{
	in_channels = 0;
	passed = false;
}

bool	user::is_passed() { return passed; }

void	user::pass() { passed = true; }

user::~user()
{

}

void	user::add_to_buffer(message msg)
{ buffer += msg.raw(); }

void	user::send_buffer()
{
	if (!buffer.length())
		return ;
	this->send_a_string(buffer);
	buffer.clear();
}

bool	user::operator==(const user& usr) const
{ return (to_lower_string(_nickname) == to_lower_string(usr.nickname())); }

bool	user::operator!=(const user& usr) const
{ return (_nickname != usr.nickname()); }

void	user::send_a_message(message& m) const
{ this->send_a_string(m.raw()); }

void	user::send_a_string(const std::string& m) const
{
	std::cout << "sending string to '" << _nickname << "':" << m;
	send(fd_user, m.c_str(), m.length(), 0);
}

bool	is_user_in_a_list(user* usr, std::list<user*>& lst)
{
	std::list<user*>::iterator	b = lst.begin();
	std::list<user*>::iterator	e = lst.end();

	while (b != e)
	{
		if ((*b) == usr)
			return true;
		b++;
	}
	return false;
}

void	user::send_to_all_known_users(message m)
{
	std::list<user*>	common_users;

	std::list<channel*>::iterator	b = _channel.begin();
	std::list<channel*>::iterator	e = _channel.end();

	while (b != e)
	{
		std::list<user*>::iterator	ub = (*b)->users().begin();
		std::list<user*>::iterator	ue = (*b)->users().end();
		while (ub != ue)
		{
			if ((*ub != this) && !is_user_in_a_list(*ub, common_users))
			{
				(*ub)->send_a_message(m);
				common_users.push_front(*ub);
			}
			ub++;
		}
		b++;
	}
}
