#include <cstdarg>
#include <vector>
#include "server.hpp"

//SPLIT comma for PRIV_MSG PART_CMD ET KICK_CMD AND JOIN
std::vector<std::string>	split_comma(std::string	buff)
{
	std::vector<std::string> list_something;
	size_t c = 0;
	while ((c = buff.find(",")) != std::string::npos)
	{
		if (buff[0] != ',')
			list_something.push_back(buff.substr(0, c));
		buff = buff.substr(c + 1);
		buff.erase(0, buff.find_first_not_of(","));
	}
	if (!buff.empty())
		list_something.push_back(buff);
	return (list_something);
}

//PASS
int	server::pass_cmd(user& usr, std::list<std::string> list_param)
{
	std::list<user>::const_iterator	_b = _users.begin();
	std::list<user>::const_iterator	_e = _users.end();

	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (ALREADYREGISTERED(usr));

	while (_b != _e)
	{
		if (*_b == usr && !( (usr.username()).empty() ))
		{
			return (ALREADYREGISTERED(usr));
		}
		_b++;
	}
	if (this->password_match(list_param.front(), this->key))
	{
		usr.pass();
	}
	return (0);
}

//NICK
int	server::nick_cmd(user& usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (ERR_NONICKNAMEGIVEN(usr));

	std::string new_nick = at(list_param, 0);

	if (list_param.size() > 1)
	{
		std::list<std::string>::iterator	b = list_param.begin();
		std::list<std::string>::iterator	e = list_param.end();
		b++;
		for (; b != e; b++)
		{
			new_nick += " ";
			new_nick += *b;
		}
	}
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

//TOPIC
int server::topic_cmd(user &usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (NOSUCHCHANNEL(usr, "*"));

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
			m = RPL_TOPIC(usr, at(list_param, 0), (*chan).topic());
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
		TOPIC_CHANOPRIVSNEEDED(usr, at(list_param, 0));
	}
	return (0);
}

//USER
int	server::user_cmd(user& usr, std::list<std::string> list_param) 
{
	extern fd_set set;
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (NEEDMOREPARAMS(usr, "USER"));

	if (usr.nickname().empty())
		return 0;
	if (!this->user_exists(usr.nickname()))
		return 1;
	else if (!usr.username().empty()) 
		return ALREADYREGISTERED(usr);

	if (!usr.is_passed())
	{
		message	m("", "ERROR");
		std::string	str = "Closing link: (" + list_param.front() + 	"@" + usr.hostname() + 	") [Access denied by configuration]";
		m.add_params(str);
		usr.send_a_message(m);
		close(usr.getfd());
		FD_CLR(usr.getfd(), &set);
		remove_user(get_user_by_fd(usr.getfd()));
		return 1;
	}

	usr.username(at(list_param, 0));
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

//MODE
int	server::mode_cmd(user& usr, std::list<std::string> params)
{
	if (params.size() == 1 && params.front().size() == 0)
	{
		if (usr.username().empty())
			return (NOTREGISTERED(usr, "MODE"));
		else
			return (NOSUCHNICK(usr, "*"));

	}
	if (params.size() < 1)
		return NEEDMOREPARAMS(usr, "MODE");
	if ((params.front())[0] == '#')
	{
		chan_iterator	chan_it = this->search_channel(params.front());
		if (chan_it == _channels.end())
			return NOSUCHCHANNEL(usr, params.front());
		if (params.size() == 1)
			return CHANNELMODEIS(usr, usr.nickname(), params.front(), ((*chan_it).is_invite_only() ? ":+int" : ":+nt"));
		if (!(*chan_it).is_op(usr))
			return CHANOPRIVSNEEDED(usr, params.front());
		std::string	flag = at(params, 1);
		if (flag == "+i" && !(*chan_it).is_invite_only())
			(*chan_it).invite_only(true);
		else if (flag == "-i" && (*chan_it).is_invite_only())
			(*chan_it).invite_only(false);
		else
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


//KICK
int	server::kick_cmd(user& asskicker, std::list<std::string> list_param) 
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		NEEDMOREPARAMS(asskicker, "KICK");

	std::string	buff = at(list_param, 1);
	std::vector<std::string> list_user = split_comma(buff);
	std::string comment;

	if (list_param.size() > 2)
	{
		std::list<std::string>::iterator	b_c = list_param.begin();	
		std::list<std::string>::iterator	e_c = list_param.end();	
		b_c++;
		b_c++;
		comment = *b_c;
		for (; b_c != e_c; b_c++)
		{
			comment += " ";
			comment += *b_c;
		}
	}

	std::vector<std::string>::iterator	b = list_user.begin();
	std::vector<std::string>::iterator	e = list_user.end();
	for (; b != e; b++)
	{
		kick_each_user(asskicker, b, list_param, comment);
	}
	return (0);
}

//PING
int	server::ping_cmd(user& usr) 
{
	message	msg("irc.local", "PONG");
	msg.command(std::string("PONG"));
	msg.add_params("irc.local");
	usr.send_a_message(msg);
	return 0;
}

//JOIN
int	server::join_cmd(user& usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (BADCHANMASK(usr, ""));
	std::string buff = list_param.front();
	std::vector<std::string> list_chan = split_comma(buff);

	std::vector<std::string>::iterator	b = list_chan.begin();
	std::vector<std::string>::iterator	e = list_chan.end();
	for (; b != e; b++)
		this->join_each_channel(usr, *b);
	return 0;
}

//WHO
int	server::who_cmd(user& usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (NEEDMOREPARAMS(usr, "WHO"));

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


//PRIVMSG
int	server::privmsg_cmd(user& sender, std::list<std::string> list_param, std::string cmd_name) 
{
	if (list_param.size() < 2)
		return NEEDMOREPARAMS(sender, std::string(cmd_name));

	if (list_param.size() == 2 && at(list_param, 1).empty())
	{
		return (ERR_NOTEXTTOSEND(sender));
	}
	
	std::string	buff = list_param.front();

	std::vector<std::string> list_receivers = split_comma(buff);
	std::vector<std::string>::iterator	b = list_receivers.begin();
	std::vector<std::string>::iterator	e = list_receivers.end();
	for (; b != e; b++)
	{
		msg_each_receivers(sender, b, list_param, cmd_name);
	}
	return 0;
}

//PART
int	server::part_cmd(user& usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (NOSUCHCHANNEL(usr, "*"));

	std::string	buff = list_param.front();

	std::vector<std::string> list_chan = split_comma(buff);
	std::vector<std::string>::iterator	b = list_chan.begin();
	std::vector<std::string>::iterator	e = list_chan.end();
	for (; b != e; b++)
	{
		part_each_channel(usr, b, list_param);
	}
	return 0;
}

//QUIT
int	server::quit_cmd(user& usr, std::list<std::string> list_param)
{
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

//INVITE
int	server::invite_cmd(user& usr, std::list<std::string> list_param)
{
	if (list_param.size() == 1 && list_param.front().size() == 0)
		return (RPL_ENDOFINVITELIST(usr));

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
		message msg("irc.local", "341");
		msg.add_params(usr.nickname());
		msg.add_params((*target).nickname());
		msg.add_params((*chan).name());
		usr.send_a_message(msg);
	}

	{
		message msg(usr.to_prefix(), "INVITE");
		msg.add_params((*target).nickname());
		msg.add_params((*chan).name());
		(*target).send_a_message(msg);
	}

	{
		message msg("irc.local", "NOTICE");
		msg.add_params((*chan).name());
		std::string	text = usr.nickname() + " invited " + (*target).nickname() + " into the channel";
		msg.add_params(text);
		(*chan).send_a_message(msg, usr);
	}
	return 0;
}
