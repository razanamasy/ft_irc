# include "replies.hpp"

message RPL_TOPIC(user& usr, std::string chan, std::string top)
{
	message m("irc.local", "332");
	m.add_params(usr.nickname());
	m.add_params(chan);
	m.add_params(top);
	return m;
}

message RPL_NOTOPIC(user& usr, std::string chan)
{
	message m("irc.local", "331");
	m.add_params(usr.nickname());
	m.add_params(chan);
	m.add_params("No topic is set");
	return m;
}

int ERR_NOTEXTTOSEND(user & usr)
{
	message m("irc.local", "412");
	m.add_params(usr.nickname());
	m.add_params("No text to send");
	usr.send_a_message(m);
	return (0);
}

int RPL_ENDOFINVITELIST(user & usr)
{
	message m("irc.local", "337");
	m.add_params(usr.nickname());
	m.add_params("End of INVITE list");
	usr.send_a_message(m);
	return (0);
}

int	NOTREGISTERED(user& usr, std::string command)
{
	message	m("irc.local", "451");
	m.add_params(usr.nickname());
	m.add_params("*");
	m.add_params(command);
	m.add_params("You have not registered.");
	usr.send_a_message(m);
	return 0;
}

message NAMEREPLY(std::string nickname, std::string channel, std::string user_lst)
{
    message msg("irc.local", "353");
	msg.add_params(nickname);
	msg.add_params("=");
	msg.add_params(channel);
	msg.add_params(user_lst);
    return (msg);
}


message ENDOFNAMES(std::string nickname, std::string channel)
{
    message msg("irc.local", "366");
	msg.add_params(nickname);
	msg.add_params(channel);
	msg.add_params("End of /NAMES list.");
    return (msg);
}

int NONICKNAMEGIVEN(user &usr)
{
	message msg("irc.local", "431");
	msg.add_params(usr.nickname());
	msg.add_params("No nickname given");
	usr.send_a_message(msg);
	return (0);
}

int ERRONEUSNICKNAME(user &usr, std::string &nick)
{
	message msg("irc.local", "432");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	msg.add_params("Erroneus nickname");
	usr.send_a_message(msg);
	return (0);
}

int NICKNAMEINUSE(user &usr, std::string &nick)
{
	message msg("irc.local", "433");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	usr.send_a_message(msg);
	return (0);
}

int NEEDMOREPARAMS(user &usr, std::string command)
{
	message msg("irc.local", "461");
	msg.add_params(usr.nickname());
	msg.add_params(command);
	msg.add_params("Not enough parameters.");
	usr.send_a_message(msg);
	return (0);
}

int ALREADYREGISTERED(user &usr)
{
	message msg("irc.local", "462");
	msg.add_params(usr.nickname());
	msg.add_params("You may not reregister");
	usr.send_a_message(msg);
	return (0);
}

int BADCHANMASK(user &usr, std::string channel_name) 
{
	message msg("irc.local", "476");
	msg.add_params(usr.nickname());
	msg.add_params(channel_name);
	msg.add_params("Invalid channel name");
	usr.send_a_message(msg);
	return (0);
}

int ERR_NONICKNAMEGIVEN(user &usr) 
{
	message msg("irc.local", "431");
	msg.add_params(usr.nickname());
	msg.add_params("No nickname given");
	usr.send_a_message(msg);
	return (0);
}

int NOSUCHCHANNEL(user &usr, std::string chanel_name)
{
	message msg("irc.local", "403");
	msg.add_params(usr.nickname());
	msg.add_params(chanel_name);
	msg.add_params("No such channel");
	usr.send_a_message(msg);
	return (0);
}

int CHANOPRIVSNEEDED(user &usr, std::string chanel)
{
	message msg("irc.local", "482");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("You're not channel operator");
	usr.send_a_message(msg);
	return (0);
}

int TOPIC_CHANOPRIVSNEEDED(user &usr, std::string chanel)
{
	message msg("irc.local", "482");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("You do not have access to change the topic on this channel");
	usr.send_a_message(msg);
	return (0);
}

int USERNOTINCHANNEL(user &usr, std::string nick, std::string chanel)
{
	message msg("irc.local", "441");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	msg.add_params(chanel);
	msg.add_params("They aren't on that channel");
	usr.send_a_message(msg);
	return (0);
}

int NOTONCHANNEL(user &usr, std::string chanel)
{
	message msg("irc.local", "442");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("You're not on that channel");
	usr.send_a_message(msg);
	return (0);
}

int USERONCHANNEL(user &usr, std::string nick, std::string &chanel) 
{
	message msg("irc.local", "443");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	msg.add_params(chanel);
	msg.add_params("is already on channel");
	usr.send_a_message(msg);
	return 0;
}

int CHANNELISFULL(user &usr, std::string &chanel)
{
	message msg("irc.local", "471");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("Cannot join channel (+l)");
	usr.send_a_message(msg);
	return (0);
}

int TOOMANYCHANNELS(user &usr, std::string &chanel_name)
{
	message msg("irc.local", "405");
	msg.add_params(usr.nickname());
	msg.add_params(chanel_name);
	usr.send_a_message(msg);
	return (0);
}

int NORECIPIENT(user &usr, std::string commands)
{
	message msg("irc.local", "411");
	msg.add_params(usr.nickname());
	msg.add_params(commands);
	msg.add_params("No recipient given");
	usr.send_a_message(msg);
	return (0);
}


int NOTEXTTOSEND(user &usr)
{
	message msg("irc.local", "412");
	msg.add_params(usr.nickname());
	msg.add_params("No text to send");
	usr.send_a_message(msg);
	return (0);
}

int NOSUCHNICK(user &usr, std::string nickname)
{
	message msg("irc.local", "401");
	msg.add_params(usr.nickname());
	msg.add_params(nickname);
	msg.add_params("No such nick");
	usr.send_a_message(msg);
	return (0);
}

int UNKNOWNMODE(user &usr, std::string &car)
{
	message msg("irc.local", "472");
	msg.add_params(usr.nickname());
	msg.add_params(car);
	msg.add_params("is unknown mode char to me");
	usr.send_a_message(msg);
	return (0);
}

int UMODEUNKNOWNFLAG(user &usr)
{
	message msg("irc.local", "501");
	msg.add_params(usr.nickname());
	msg.add_params("Unknown MODE flag");
	usr.send_a_message(msg);
	return (0);
}

int NOORIGIN(user &usr)
{
	message msg("irc.local", "409");
	msg.add_params(usr.nickname());
	msg.add_params("No origin specified");
	usr.send_a_message(msg);
	return (0);
}

int CANNOTSENDTOCHAN(user &usr, std::string &channel_name)
{
	message msg("irc.local", "404");
	msg.add_params(usr.nickname());
	msg.add_params(channel_name);
	msg.add_params("Cannot send to channel");
	usr.send_a_message(msg);
	return (0);
}


int WHOISUSER(user &usr, std::string nick, std::string user, std::string host, std::string real_name)
{
	message msg("irc.local", "311");
	msg.add_params(nick);	msg.add_params(user);	msg.add_params(host);
	msg.add_params("*");
	msg.add_params(real_name);
	usr.send_a_message(msg);
	return (0);
}

int NOTOPIC(user &usr, std::string &channel)
{
	message msg("irc.local", "331");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("No topic is set");
	usr.send_a_message(msg);
	return (0);
}

int NAMEREPLY(user &usr, std::string &channel)
{
	message msg("irc.local", "353");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("[[@|+]<nick> [[@|+]<nick> [...]]]");
	usr.send_a_message(msg);
	return (0);
}

int ENDOFNAMES(user &usr, std::string &channel)
{
	message msg("irc.local", "366");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("End of /NAMES list");
	usr.send_a_message(msg);
	return (0);
}

int CHANNELMODEIS(user &usr, std::string channel, std::string mode, std::string mode_params)
{
	message msg("irc.local", "324");
	msg.add_params(usr.nickname());
	msg.add_params(channel);	msg.add_params(mode);	msg.add_params(mode_params);
	usr.add_to_buffer(msg);
	usr.send_buffer();
	return (0);
}

message WHOREPLY(user &usr, std::string channel, std::string user, std::string host, std::string server, std::string nick, std::string real_name, bool is_op)
{
	(void)usr;
    message msg("irc.local", "352");
    msg.add_params(nick); msg.add_params(channel); msg.add_params(user); msg.add_params(host); msg.add_params(server); msg.add_params(nick);
	if (is_op)
		msg.add_params("H@");
	else
		msg.add_params("H");
    msg.add_params("0 " + real_name);
    return (msg);
}


message ENDOFWHO(user &usr, std::string name)
{
	(void)usr;
	message msg("irc.local", "315");
	msg.add_params(usr.nickname());
	msg.add_params(name);
	msg.add_params("End of /WHO list");
	return (msg);
}

int ENDOFBANLIST(user &usr, std::string &channel)
{
	message msg("irc.local", "368");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("End of channel ban list");
	usr.send_a_message(msg);
	return (0);
}


int LUSERCLIENT(user &usr, std::string int1, std::string int2, std::string int3)
{
	message msg("irc.local", "251");
	msg.add_params(usr.nickname());
	msg.add_params("There are " + int1 + " users and " + int2 + " invisible on " + int3 + "servers");
	usr.send_a_message(msg);
	return (0);
}

int LUSERUNKNOWN(user &usr, std::string &integer)
{
	message msg("irc.local", "253");
	msg.add_params(usr.nickname());
	msg.add_params(integer);
	msg.add_params("unknown connection(s)");
	usr.send_a_message(msg);
	return (0);
}


int LUSERCHANNELS(user &usr, std::string &integer)
{
	message msg("irc.local", "254");
	msg.add_params(usr.nickname());
	msg.add_params(integer);
	msg.add_params("channels formed");
	usr.send_a_message(msg);
	return (0);
}

int LUSERME(user &usr, std::string int1, std::string int2)
{
	message msg("irc.local", "255");
	msg.add_params(usr.nickname());
	msg.add_params("I have " + int1 + " clients and " + int2 + " servers");
	usr.send_a_message(msg);
	return (0);
}

int MOTDSTART(user &usr, std::string server)
{
	message msg("irc.local", "375");
	msg.add_params(usr.nickname());
	msg.add_params("- " + server + " Message of the day - ");
	usr.send_a_message(msg);
	return (0);
}

int MOTD(user &usr, std::string &text)
{
	message msg("irc.local", "372");
	msg.add_params(usr.nickname());
	msg.add_params("- " + text);
	usr.send_a_message(msg);
	return (0);
}

int ENDOFMOTD(user &usr)
{
	message msg("irc.local", "376");
	msg.add_params(usr.nickname());
	msg.add_params("End of /MOTD command");
	usr.send_a_message(msg);
	return (0);
}

int UMODEIS(user &usr, std::string &user_mode_string)
{
	message msg("irc.local", "221");
	msg.add_params(usr.nickname());
	msg.add_params(user_mode_string);
	usr.send_a_message(msg);
	return (0);
}

int	INVITEONLYCHANNEL(user& usr, std::string channel)
{
	message	msg("irc.local", "473");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("Cannot join channel (+i)");
	usr.send_a_message(msg);
	return 0;
}

int	USERSDONTMATCH(user& usr)
{
	message	msg("irc.local", "502");
	msg.add_params(usr.nickname());
	msg.add_params("Cannot change mode for other users");
	usr.send_a_message(msg);
	return 0;
}
