#include <cstdarg>
#include <vector>
#include "server.hpp"

///////////////////////
// COMMANDS TO PARSE //
///////////////////////

/* PASS
 * 1: <password>
 *
 * The PASS command is used to set a 'connection password'.  The
 * password can and must be set before any attempt to register the
 * connection is made.  Currently this requires that clients send a PASS
 * command before sending the NICK/USER combination. The password supplied
 * must match the one supplied at the server's launch. It is possible to send
 * multiple PASS commands before registering but only the last one sent
 * is used for verification and it may not be changed once registered.
 *
 * Numeric Replies:
 *
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED */
/*
int ERROR(user &usr, std::string error_message)
{
    message msg("irc.local", "ERROR");
    msg.add_params("Closing link");
    msg.add_params(error_message);
    usr.send_a_message(msg);
    return (0);
}*/

int	server::pass_cmd(user& usr, std::list<std::string> list_param)
{
	extern fd_set set;
	std::list<user>::const_iterator	_b = _users.begin();
	std::list<user>::const_iterator	_e = _users.end();

	while (_b != _e)
	{
		if (*_b == usr && !( (usr.username()).empty() ))
		{
			return (ALREADYREGISTERED(usr));
		}
		_b++;
	}
	if (this->password_match(list_param.front(), this->hash, this->key))
	{
		usr.pass();
	}
	return (0);
}

/* NICK
 * 1: <nickname> 
 *
 * NICK message is used to give user a nickname or change the previous
 * one.
 * When a user attempts to set it's nick for the first time with a nick that
 * already exists, an underscore '_' is added at it's end,
 * and RPL_WHOISUSER is sent about who already has the nick.
 * When a user attempts to change it's nick to one that already exists,
 * and ERR_NICKNAMEINUSE is sent.
 *
 * Numeric Replies:
 * ERR_NONICKNAMEGIVEN
 * ERR_NICKNAMEINUSE
 * RPL_WHOISUSER */

int	server::nick_cmd(user& usr, std::list<std::string> list_param)
{
	std::string new_nick = at(list_param, 0);

	if (uncorrect_param(new_nick))
		return (ERRONEUSNICKNAME(usr, new_nick));

	if (usr.nickname().empty())
	{
		while (this->user_exists(new_nick))
			new_nick += "_";
		usr.nickname(new_nick);
		return 0;
	}
	if (new_nick == usr.nickname())
		return 0;
	if (this->user_exists(new_nick))
		return NICKNAMEINUSE(usr, new_nick);
	message msg(usr.to_prefix(), "NICK");
	msg.add_params(new_nick);
	usr.send_a_message(msg);
	usr.nickname(new_nick);
	usr.send_to_all_known_users(msg);
	return 0;
}

int server::topic_cmd(user &usr, std::list<std::string> list_param)
{
	server::chan_iterator chan = this->search_channel(at(list_param, 0));
	message m;
	if (chan == (this->_channels).end())
	{
		NOSUCHCHANNEL(usr, at(list_param, 0));
		return (0);
	}

	if (list_param.size() == 1)
	{
		if ((*chan).topic().empty())
		{
			m = RPL_NOTOPIC(usr, at(list_param, 0));
			usr.send_a_message(m);
			return (0);
		}
		else
		{
			m = RPL_TOPIC(usr, at(list_param, 0), at(list_param, 1));
			usr.send_a_message(m);
			return (0);
		}
	}
	
	channel::user_iterator user = (*chan).search(usr);

	if (user == (*chan).users().end())
	{
		NOTONCHANNEL(usr, at(list_param, 0));
		return (0);
	}

	if ((*chan).is_op(usr))
	{
		(*chan).settopic(at(list_param, 1));
		message msg(usr.to_prefix(), "TOPIC");
		msg.add_params(at(list_param, 0));
		msg.add_params(at(list_param, 1));
		usr.send_a_message(msg);
		(*chan).send_a_message(msg, usr);
	}
	else
	{
		CHANOPRIVSNEEDED(usr, at(list_param, 0));
	}

	return (0);
}

/* USER
 * 4: <username> <hostname> <servername> <realname>
 *
 * The USER message is used at the beginning of connection to specify
 * the username, hostname, servername and realname of a new user.
 * Note that hostname and servername are normally ignored by the IRC
 * server when the USER command comes from a directly connected client
 * (for security reasons).
 * 
 * It must be noted that realname parameter must be the last parameter,
 * because it may contain space characters and must be prefixed with a
 * colon (':') to make sure this is recognised as such.
 * 
 * Numeric Replies:
 * 
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED */

int	server::user_cmd(user& usr, std::list<std::string> list_param) 
{
	extern fd_set set;
	if (usr.nickname().empty())
		return 0;
	if (!this->user_exists(usr.nickname()))				// user was not correctly set? useless protection?
		return NOSUCHNICK(usr, usr.nickname());
	else if (!usr.username().empty()) 
	{// hostname already filled
		return ALREADYREGISTERED(usr);
	}

	if (!usr.is_passed())
	{
		message	m("", "ERROR");
		std::string	str = 		"Closing link: (" 
							+ 	list_param.front() 
							+ 	"@" + usr.hostname() 
							+ 	") [Access denied by configuration]";
		m.add_params(str);
		usr.send_a_message(m);
		close(usr.getfd());
		FD_CLR(usr.getfd(), &set);
		remove_user(get_user_by_fd(usr.getfd()));
		return 1;
	}

	usr.username(at(list_param, 0));					// fill user params
	if (list_param.size() == 4)
		usr.servername(at(list_param, 2)); 
	if (list_param.size() == 4)
		usr.realname(at(list_param, 3));

	message	m("irc.local", "001");
	m.add_params(usr.nickname());
	m.add_params("establishing connection");
	usr.add_to_buffer(m);
	m.command("002");
	usr.add_to_buffer(m);
	m.command("003");
	usr.add_to_buffer(m);
	m.command("004");
	usr.add_to_buffer(m);
	usr.send_buffer();
	return 0;
}

/* MODE
 * 2: <nick> +i
 * 1: <channel> [b]
 *
 * The MODE command can be used to change the mode of a channel/user
 * channel:
 * 		without any other parameter, it returns RPL_CHANNELMODEIS
 * 		else it returns RPL_ENDOFBANLIST
 *
 * user:
 * 		turns the users invisible (+i option, cf RFC1450 4.2.3.2)
 *
 * Numeric replies:
 * ERR_NEEDMOREPARAMS
 * ERR_CHANOPRIVSNEEDED
 * ERR_NOSUCHNICK
 * ERR_NOTONCHANNEL
 * ERR_UNKNOWNMODE
 * ERR_NOSUCHCHANNEL
 * ERR_UMODEUNKNOWNFLAG
 * RPL_CHANNELMODEIS
 * RPL_UMODEIS
 * RPL_ENDOFBANLIST */

int	server::mode_cmd(user& usr, std::list<std::string> params)
{
	if (params.size() < 1)
		return NEEDMOREPARAMS(usr, "MODE");
	if ((params.front())[0] == '#')
	{
		chan_iterator	chan_it = this->search_channel(params.front());
		if (chan_it == _channels.end())
			return NOSUCHCHANNEL(usr, params.front());
		if (params.size() == 1) // if only 1 parameter, that means the mode is not modified
			return CHANNELMODEIS(usr, usr.nickname(), params.front(), ((*chan_it).is_invite_only() ? ":+int" : ":+nt"));
		if (!(*chan_it).is_op(usr)) //if a mode is specified, user must be operator
			return CHANOPRIVSNEEDED(usr, params.front());
		std::string	flag = at(params, 1);
		if (flag == "+i" && !(*chan_it).is_invite_only())
			(*chan_it).invite_only(true);
		else if (flag == "-i" && (*chan_it).is_invite_only())
			(*chan_it).invite_only(false);
		else // ignoring other cases
			return 0;
		message	m(usr.to_prefix(), "MODE");
		m.add_params((*chan_it).name());
		m.add_params(":" + flag);
		usr.send_a_message(m);
		(*chan_it).send_a_message(m, usr);
		return 0;
	}
	user_iterator	target = search_user(params.front());
	if (target == _users.end())
		return NOSUCHNICK(usr, params.front());
	if ((*target).nickname() != usr.nickname())
		return USERSDONTMATCH(usr);
	message msg(usr.to_prefix(), "MODE");
	msg.add_params(usr.nickname());
	msg.add_params("+i");
	usr.send_a_message(msg);
	return 0;
}

/* KICK
 * 2: <channel> <user> [<comment>]
 * The KICK command can be  used  to  forcibly  remove  a  user  from  a
 * channel. It  'kicks  them  out'  of the channel (forced PART).
 * Only a channel operator may kick another user out of a  channel.
 * Each  server that  receives  a KICK message checks that it is valid
 * (ie the sender is actually a  channel  operator)  before  removing
 * the  victim  from  the channel.
 * 
 * Numeric Replies:
 * 
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHCHANNEL
 * ERR_CHANOPRIVSNEEDED
 * ERR_NOTONCHANNEL */

int	server::kick_cmd(user& asskicker, std::list<std::string> list_param) 
{

	///CREATE LIST OF USER
	std::vector<std::string> list_user;
	std::string	buff = at(list_param, 1);
	size_t c = 0;
	while ((c = buff.find(",")) != std::string::npos)
	{
		if (buff[0] != ',')
			list_user.push_back(buff.substr(0, c));
		buff = buff.substr(c + 1);
		buff.erase(0, buff.find_first_not_of(","));

	}
	if (!buff.empty())
		list_user.push_back(buff);

	std::vector<std::string>::iterator	b = list_user.begin();
	std::vector<std::string>::iterator	e = list_user.end();




	for (; b != e; b++)
	{
		std::cout << "i'm kicking this bitch : " << *b << std::endl;
		server::user_iterator asskicked = this->search_user(*b);
		server::chan_iterator chan		= this->search_channel(at(list_param, 0));

		if (asskicked == this->_users.end())
			(NOSUCHNICK(asskicker, *b));
		else
		{	
			if((*chan).is_op(asskicker))
			{
				if (!(*chan).is_user_in_channel(*asskicked) )
					NOTONCHANNEL(*asskicked, (*chan).name());
				else
				{
					message msg((asskicker).to_prefix(), "KICK");
					msg.add_params((*chan).name());
					msg.add_params((*asskicked).nickname());
					if (list_param.size() == 3)
						msg.add_params(at(list_param, 2));
					(*asskicked).send_a_message(msg);
					(*chan).send_a_message(msg, *asskicked);
					this->pop_user_from_chan(*asskicked, chan);
				}
			}
			else
				CHANOPRIVSNEEDED(asskicker, (*chan).name());
		}
	}
	return (0);
}

/* PING
 * 1: <server>
 * The PING message is used to test the presence of an active client at
 * the other end of the connection.  A PING message is sent at regular
 * intervals if no other activity detected coming from a connection.  If
 * a connection fails to respond to a PING command within a set amount
 * of time, that connection is closed.
 *
 * Any client which receives a PING message must respond to <server>
 * (server which sent the PING message out) as quickly as possible with
 * an appropriate PONG message to indicate it is still there and alive.
 * Servers should not respond to PING commands but rely on PINGs from
 * the other end of the connection to indicate the connection is alive.
 *
 * Numeric Replies:
 *
 * ERR_NOORIGIN
	msg.add_params("irc.local");
 * ERR_NOSUCHSERVER */

int	server::ping_cmd(user& usr) 
{
	message	msg("irc.local", "PONG");
	msg.command(std::string("PONG"));
	//msg.add_params("sitio-toaster");
	msg.add_params("irc.local");
	usr.send_a_message(msg);
	return 0;
}

/* JOIN
 * 1: <channel>
 * The JOIN command is used by client to start listening a specific channel.
 * Once a user has joined a channel, they receive notice about all commands
 * their server receives which affect the channel.
 * This includes MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
 * 
 * Numeric Replies:
 * ERR_NEEDMOREPARAMS
 * ERR_CHANNELISFULL
 * ERR_NOSUCHCHANNEL
 * ERR_TOOMANYCHANNELS
 * RPL_NOTOPIC */

// SPLIT >> IN UTILS FILE
// JOIN CHANNEL >> NO NEED IN SERVER HEADER?

size_t	count_needles(char needle, std::string haystack)
{
	size_t count = 0;

	for (size_t i = 0; (i = haystack.find(needle, i)) != std::string::npos; i++)
		count++;
	return count;	
}	

std::list<std::string>	split(char sep, std::string params_str)
{
	size_t index	= 0;
	size_t len		= 0;
	std::list<std::string> params_list;

	while (params_str[index] == sep) 
		index++;
	while ((len = params_str.find_first_of(sep, index)) != std::string::npos)
	{
		if (len != index)
			params_list.push_back(params_str.substr(index, len - index));
		
		index = len;
		while (params_str[index] == sep)
			index++;
	}
	params_list.push_back(params_str.substr(index));
	return params_list;
}

int	server::join_cmd(user& usr, std::list<std::string> list_param)
{
	std::string param = list_param.front();
	std::list<std::string> chans_names;

	if (param.find(',') != std::string::npos)
	{
			chans_names = split(',', param);
			size_t size = chans_names.size();
			for (size_t i = 0; i < size; i++)
			{
				this->join_channel(usr, chans_names.back());
				chans_names.pop_back();	
			}
	}
	else
		this->join_channel(usr, param);
	return 0;
}

int	server::join_channel(user& usr, std::string chan_name)
{
	std::list<std::string> list_param(1, chan_name);
	chan_name = to_lower_string(chan_name);
	if (chan_name.empty())
		return 1;
	if (chan_name[0] != '#')
		return BADCHANMASK(usr, chan_name);
		//chan_name = "#" + chan_name;
	if (!usr.can_join_a_channel())
		return TOOMANYCHANNELS(usr, chan_name);
	chan_iterator	chan;
	if (!this->channel_exists(chan_name) && (nbr_of_channels < MAX_CHANNEL))
	{
		channel	new_chan(chan_name, &usr);
		this->add_channel(new_chan);
		chan = search_channel(chan_name);
		usr.add_a_chan(&(*chan));
		mode_cmd(usr, list_param);
	}
	else
	{
		chan = search_channel(chan_name);
		if (chan == _channels.end())
			return NOSUCHCHANNEL(usr, chan_name);
		if ((*chan).is_full())
			return CHANNELISFULL(usr, chan_name);
		if ((*chan).is_invite_only() && !(*chan).is_invited(&usr))
			return INVITEONLYCHANNEL(usr, chan_name);
		(*chan).add_user(usr);
	}
	message	m(usr.to_prefix(), "JOIN");
	m.add_params(chan_name);
	usr.add_to_buffer(m);
	std::string	nicks;
	channel::user_iterator	b = (*chan).users().begin();
	channel::user_iterator	e = (*chan).users().end();
	while (b != e)
	{
		nicks += " ";
		if ((*chan).is_op(*(*b)))
			nicks += "@";
		nicks += (*b)->nickname();
		b++;
	}
	if (((*chan).topic()).empty())
	{
		usr.add_to_buffer(RPL_NOTOPIC(usr, (*chan).name()));
	}
	else
	{
		usr.add_to_buffer(RPL_TOPIC(usr, (*chan).name(), (*chan).topic()));
	}
	usr.add_to_buffer(NAMEREPLY(usr.nickname(), (*chan).name(), nicks));
	usr.add_to_buffer(ENDOFNAMES(usr.nickname(), (*chan).name()));

	usr.send_buffer();
	(*chan).send_a_message(m, usr);
	return 0;
}

/* WHO
 * 0: <name>
 * The WHO message is used by a client to generate a query which returns
 * a list of information of the <name> parameter given by
 * the client. In the absence of the <name> parameter, all visible
 * (users who aren't invisible (user mode +i) and who don't have a
 * common channel with the requesting client) are listed.  The same
 * result can be achieved by using a <name> of "0".
 * The <name> passed to WHO is matched against users' host, server, real
 * name and nickname if the channel <name> cannot be found.
 *
 * Numeric Replies:
 * ERR_NOSUCHSERVER
 * RPL_WHOREPLY
 * RPL_ENDOFWHO */

int	server::who_cmd(user& usr, std::list<std::string> list_param)
{
	std::string	cible = list_param.front();

	if (cible[0] == '#')
	{
		chan_iterator bch = search_channel(cible);

		if (bch !=  this->_channels.end() && (*bch).is_user_in_channel(usr) && cible.size() != 1)
		{
			std::list<user*>::const_iterator	_bu = ((*bch).users()).begin();
			std::list<user*>::const_iterator	_eu = ((*bch).users()).end();

			for (; _bu != _eu; _bu++)
			{
				usr.add_to_buffer(WHOREPLY(	usr, cible,(*_bu)->username(),(*_bu)->hostname(),(*_bu)->servername(),(*_bu)->nickname(),(*_bu)->realname(), (*bch).is_op(**_bu)));
			}
		}
		usr.add_to_buffer(ENDOFWHO(usr, usr.username()));
		usr.send_buffer();
		return (0);
	}

	user_iterator serv_u = search_user(cible);

	if (serv_u != this->_users.end())
	{
		channel* last_chan_ptr = (*serv_u).get_last_channel();

		if (last_chan_ptr != NULL)
			usr.add_to_buffer(WHOREPLY(	usr, (*last_chan_ptr).name(), (*serv_u).username(), (*serv_u).hostname(), (*serv_u).servername(), (*serv_u).nickname(), (*serv_u).realname(), last_chan_ptr && (*last_chan_ptr).is_op((*serv_u))));
		else
			usr.add_to_buffer(WHOREPLY(	usr, "*", (*serv_u).username(), (*serv_u).hostname(), (*serv_u).servername(), (*serv_u).nickname(), (*serv_u).realname(), last_chan_ptr && (*last_chan_ptr).is_op((*serv_u))));
	}

	usr.add_to_buffer(ENDOFWHO(usr, usr.username()));
	usr.send_buffer();
	return (0);
}

/* PRIVMSG
 * 2: <receiver> <text to be sent>
 * PRIVMSG is used to send private messages between users.
 * <receiver> is the nickname of the receiver of the message.
 * <receiver> can also be a channel name.
 *
 * Numeric Replies:
 * ERR_NORECIPIENT
 * ERR_CANNOTSENDTOCHAN
 * ERR_NOSUCHNICK
 * ERR_NOTEXTTOSEND */

int	server::privmsg_cmd(user& sender, std::list<std::string> list_param, std::string cmd_name) 
{
	if (list_param.size() < 2)
		return NORECIPIENT(sender, std::string(cmd_name));

	std::vector<std::string> list_receivers;
	std::string	buff = list_param.front();
	size_t c = 0;
	while ((c = buff.find(",")) != std::string::npos)
	{
		if (buff[0] != ',')
			list_receivers.push_back(buff.substr(0, c));
		buff = buff.substr(c + 1);
		buff.erase(0, buff.find_first_not_of(","));

	}
	if (!buff.empty())
		list_receivers.push_back(buff);

	std::vector<std::string>::iterator	b = list_receivers.begin();
	std::vector<std::string>::iterator	e = list_receivers.end();
	for (; b != e; b++)
	{
		message	m(sender.to_prefix(), cmd_name);
		m.add_params((*b));
		 std::list<std::string>::iterator b_param = list_param.begin();
		 std::list<std::string>::iterator e_param = list_param.end();
		 b_param++;
		for (; b_param != e_param; b_param++)
			m.add_params(*b_param);

		if ((*b)[0] != '#')
		{
			server::user_iterator receiver = this->search_user((*b));
			if (receiver != _users.end()) 
			{
				(*receiver).send_a_message(m);
			}
			else
			{
					NOSUCHNICK(sender, (*b));
			}
		}
		else
		{
			server::chan_iterator receiver = this->search_channel((*b));
			if (receiver == _channels.end())
				NOSUCHCHANNEL(sender, (*b));
			if (!(*receiver).is_user_in_channel(sender))
				CANNOTSENDTOCHAN(sender, (*b));
			(*receiver).send_a_message(m, sender);
		}
	}
	return 0;
}

/* PART
 * 1: <channel>
 * The PART message causes the client sending the message to be removed
 * from the list of active users for all given channels listed in the
 * parameter string.
 *
 * Numeric Replies:
 *
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHCHANNEL
 * ERR_NOTONCHANNEL */

int	server::part_cmd(user& usr, std::list<std::string> list_param)
{
	std::vector<std::string> list_chan;
	std::string	buff = list_param.front();
	size_t c = 0;
	while ((c = buff.find(",")) != std::string::npos)
	{
		if (buff[0] != ',')
			list_chan.push_back(buff.substr(0, c));
		buff = buff.substr(c + 1);
		buff.erase(0, buff.find_first_not_of(","));

	}
	if (!buff.empty())
		list_chan.push_back(buff);

	std::vector<std::string>::iterator	b = list_chan.begin();
	std::vector<std::string>::iterator	e = list_chan.end();



	for (; b != e; b++)
	{
	//	std::cout << "i'm parting this channel : " << *b << std::endl;
		std::string	chan_name = to_lower_string(*b);
		if (chan_name[0] != '#')
			chan_name = "#" + chan_name;
		server::chan_iterator chan = this->search_channel(chan_name);
		if (chan == this->_channels.end())
			NOSUCHCHANNEL(usr, *b);
		else
		{
			if (!(*chan).is_user_in_channel(usr))
				NOTONCHANNEL(usr, *b);
			else
			{
				message msg(usr.to_prefix(), "PART");
				msg.add_params(*b);

				if (list_param.size() >= 2)
				{
					std::list<std::string>::iterator b_param = list_param.begin();
					std::list<std::string>::iterator e_param = list_param.end();
					b_param++;
					for (; b_param != e_param; b_param++)
					{
					    msg.add_params(*b_param);
					}
				}
				usr.send_a_message(msg);
				(*chan).send_a_message(msg, usr);
				this->pop_user_from_chan(usr, chan);
			}
		}
	}
	return 0;
}

/* QUIT
 * 0: <quit message>
 * A client session is ended with a quit message.  The server must close
 * the connection to a client which sends a QUIT message. If a "Quit
 * Message" is given, this will be sent instead of the default message,
 * the nickname.
 *
 * If, for some other reason, a client connection is closed without  the
 * client  issuing  a  QUIT  command  (e.g.  client  dies and EOF occurs
 * on socket), the server is required to fill in the quit  message  with
 * some sort  of  message  reflecting the nature of the event which
 * caused it to happen. */

int	server::quit_cmd(user& usr, std::list<std::string> list_param)
{
	std::cout << "QUIT SPELL"<< std::endl;

	std::cout << "///" <<fd_buffer[usr.getfd()] << "///" << std::endl;
	extern fd_set set;
	user_iterator	uit = this->search_user(usr.nickname());
	message	m(usr.to_prefix(), "QUIT");
	if (list_param.empty())
		m.add_params("Leaving");
	else
		m.add_params(list_param.front());
	if (uit != _users.end())
	{
		std::map<int, std::string>::iterator	_b = fd_buffer.begin();
		std::map<int, std::string>::iterator	_e = fd_buffer.end();
		for (; _b != _e; _b++)
		{
			if ((*_b).first == usr.getfd())
				break ;
		}
		(*_b).second.clear();
		(*uit).send_to_all_known_users(m);
		close(usr.getfd());
		FD_CLR(usr.getfd(), &set);
		remove_user(uit);
	}
	return 0;
}

//////////////////////
// COMMANDS TO SEND //
//////////////////////

/* NOTICE
 * 2: <nickname> <text>
 * The NOTICE message is used similarly to PRIVMSG. The difference
 * between NOTICE and PRIVMSG is that automatic replies must never be
 * sent in response to a NOTICE message. */

/*
void server::notice_cmd(user& sender, std::list<std::string> list_param)
{
	message	m(sender.to_prefix(), "NOTICE");
	m.add_params(list_param.front());
	m.add_params(at(list_param, 1));
	if (list_param.front()[0] != '#')
	{
		server::user_iterator receiver = this->search_user(list_param.front());
		if (receiver != _users.end()) 
		{
			(*receiver).send_a_message(m);
			return ;
		}
		return ;
	}
	else
	{
		server::chan_iterator receiver = this->search_channel(at(list_param, 0));
		if (receiver == _channels.end())
			return ;
		if (!(*receiver).is_user_in_channel(sender))
			return ;
		(*receiver).send_a_message(m, sender);
	}
	return ;
}
*/

int	server::notice_cmd(user& sender, std::list<std::string> list_param)
{
	if (list_param.size() < 2)
		return NORECIPIENT(sender, std::string("NOTICE"));
	message	m(sender.to_prefix(), "NOTICE");
	m.add_params(list_param.front());
	m.add_params(at(list_param, 1));
	if (list_param.front()[0] != '#')
	{
		server::user_iterator receiver = this->search_user(list_param.front());
		if (receiver == _users.end()) 
			return NOSUCHNICK(sender, list_param.front());
		(*receiver).send_a_message(m);
	}
	else
	{
		server::chan_iterator receiver = this->search_channel(at(list_param, 0));
		if (receiver == _channels.end())
			return NOSUCHCHANNEL(sender, list_param.front());
		if (!(*receiver).is_user_in_channel(sender))
			return CANNOTSENDTOCHAN(sender, list_param.front());
		(*receiver).send_a_message(m, sender);
	}
	return 0;
}

/* PONG
 * <daemon>
 * PONG message is a reply to PING message. Is the name of the server. */

void server::pong(user& usr)
{
	message	msg;
	msg.command(std::string("PONG"));
	msg.add_params(usr.servername());
	usr.send_a_message(msg);
}

/*
 * INVITE
 * 2: <nickname> <channel>
 *
 * The INVITE command is used to invite a user to a channel.  The
 * parameter <nickname> is the nickname of the person to be invited to
 * the target channel <channel>.  There is no requirement that the
 * channel the target user is being invited to must exist or be a valid
 * channel.  However, if the channel exists, only members of the channel
 * are allowed to invite other users.  When the channel has invite-only
 * flag set, only channel operators may issue INVITE command.
 * Only the user inviting and the user being invited will receive
 * notification of the invitation.  Other channel members are not
 * notified.  (This is unlike the MODE changes, and is occasionally the
 * source of trouble for users.)
 *
 * Numeric Replies:
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHNICK
 * ERR_NOTONCHANNEL
 * ERR_USERONCHANNEL
 * ERR_CHANOPRIVSNEEDED
 * RPL_INVITING
*/

int	server::invite_cmd(user& usr, std::list<std::string> list_param)
{
	std::string	chan_name	= at(list_param, 1);
	user_iterator target	= this->search_user(list_param.front());
	chan_iterator chan		= this->search_channel(chan_name);

	if (target == this->_users.end())
		return NOSUCHNICK(usr, list_param.front());	
	if (chan == this->_channels.end())
		return NOSUCHCHANNEL(usr, list_param.front());
	if (!(*chan).is_user_in_channel(usr))
		return NOTONCHANNEL(usr, chan_name);
	if (!(*chan).is_op(usr))
		return CHANOPRIVSNEEDED(usr, chan_name);
	if ((*chan).is_user_in_channel(*target))
		return USERONCHANNEL(usr, (*target).nickname(), chan_name);
	(*chan).add_to_invite_list(&(*target));

	{
		// REPLY INVITING	
		message msg("irc.local", "341");
		msg.add_params(usr.nickname());
		msg.add_params((*target).nickname());
		msg.add_params((*chan).name());
		usr.send_a_message(msg);
	}

	{
		// INFORM TARGET
		message msg(usr.to_prefix(), "INVITE");
		msg.add_params((*target).nickname());
		msg.add_params((*chan).name());
		(*target).send_a_message(msg);
	}

	{
		// NOTIFY CHANNEL
		message msg("irc.local", "NOTICE");
		msg.add_params((*chan).name());
		std::string	text = usr.nickname() + " invited " + (*target).nickname() + " into the channel";
		msg.add_params(text);
		(*chan).send_a_message(msg, usr);
	}
	return 0;
}
