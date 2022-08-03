#include "server.hpp"

//JOIN
void	server::create_channel(user& usr, std::string chan_name, std::list<std::string> list_param, chan_iterator& chan)
{
	channel	new_chan(chan_name, &usr);
	this->add_channel(new_chan);
	chan = search_channel(chan_name);
	usr.add_a_chan(&(*chan));
	mode_cmd(usr, list_param);
}

int	server::fill_channel(user& usr, std::string chan_name, chan_iterator& chan)
{
	chan = search_channel(chan_name);
	if (chan == _channels.end())
		return NOSUCHCHANNEL(usr, chan_name);
	if ((*chan).is_full())
		return CHANNELISFULL(usr, chan_name);
	if ((*chan).is_invite_only() && !(*chan).is_invited(&usr))
		return INVITEONLYCHANNEL(usr, chan_name);
	(*chan).add_user(usr);
	return (0);
}

void	server::set_up_messages(message& m, std::string chan_name, chan_iterator& chan, user& usr)
{
	//set message for current user
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
		usr.add_to_buffer(RPL_NOTOPIC(usr, (*chan).name()));
	else
		usr.add_to_buffer(RPL_TOPIC(usr, (*chan).name(), (*chan).topic()));
	usr.add_to_buffer(NAMEREPLY(usr.nickname(), (*chan).name(), nicks));
	usr.add_to_buffer(ENDOFNAMES(usr.nickname(), (*chan).name()));
}

int	server::join_each_channel(user& usr, std::string chan_name)
{
	std::list<std::string> list_param(1, chan_name);
	chan_name = to_lower_string(chan_name);
	if (chan_name.empty())
		return 1;
	if (chan_name[0] != '#')
		return BADCHANMASK(usr, chan_name);
	if (!usr.can_join_a_channel())
		return TOOMANYCHANNELS(usr, chan_name);
	chan_iterator	chan;
	if (!this->channel_exists(chan_name) && (nbr_of_channels < MAX_CHANNEL))
		create_channel(usr, chan_name, list_param, chan);
	else
		fill_channel(usr, chan_name, chan);
	message	m(usr.to_prefix(), "JOIN");
	set_up_messages(m, chan_name, chan, usr);
	usr.send_buffer();
	(*chan).send_a_message(m, usr);
	return 0;
}

//KICK
void	server::kick_each_user(user& asskicker, std::vector<std::string>::iterator b, std::list<std::string> list_param)
{
	//COEUR DE KICK A SORTIR
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

//PART
void	server::part_each_channel(user& usr, std::vector<std::string>::iterator	b, std::list<std::string> list_param)
{
	//COEUR DE PART A SORTIR
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

//PRIVMSG
void	server::msg_each_receivers(user& sender, std::vector<std::string>::iterator	b, std::list<std::string> list_param, std::string cmd_name)
{
		//COEUR DE FOR A SORTIR
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
