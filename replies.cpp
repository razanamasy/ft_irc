# include "replies.hpp"

int RPL_TOPIC(user& usr, std::string chan, std::string top)
{
	message m("irc.local", "332");
	m.add_params(usr.nickname());
	m.add_params(chan);
	m.add_params(top);
	usr.send_a_message(m);
	return (0);
}

int RPL_NOTOPIC(user& usr, std::string chan)
{
	message m("irc.local", "331");
	m.add_params(usr.nickname());
	m.add_params(chan);
	m.add_params("No topic is set");
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

# define RPL_NAMEREPLY 353
message NAMEREPLY(std::string nickname, std::string channel, std::string user_lst)
{
    message msg("irc.local", "353");
	msg.add_params(nickname);
	msg.add_params("=");
	msg.add_params(channel);
	msg.add_params(user_lst);
    return (msg);
}


# define RPL_ENDOFNAMES 366
message ENDOFNAMES(std::string nickname, std::string channel)
{
    message msg("irc.local", "366");
	msg.add_params(nickname);
	msg.add_params(channel);
	msg.add_params("End of /NAMES list.");
    return (msg);
}

# define ERR_NONICKNAMEGIVEN 431 // /nick
//:No nickname given
int NONICKNAMEGIVEN(user &usr)
{
	message msg("irc.local", "431");
	msg.add_params(usr.nickname());
	msg.add_params("No nickname given");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_ERRONEUSNICKNAME 432 // /nick
//<nick> :Erroneus nickname
int ERRONEUSNICKNAME(user &usr, std::string &nick)
{
	message msg("irc.local", "432");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	msg.add_params("Erroneus nickname");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NICKNAMEINUSE 433 // in case of /nick after succesfull connection
//<nick> :Nickname is already in use
int NICKNAMEINUSE(user &usr, std::string &nick)
{
	message msg("irc.local", "433");
	msg.add_params(usr.nickname());
	msg.add_params(nick);
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NEEDMOREPARAMS 461
//<command> :Not enough parameters

int NEEDMOREPARAMS(user &usr, std::string command)
{
	message msg("irc.local", "461");
	msg.add_params(usr.nickname());
	msg.add_params(command);
	msg.add_params("Not enough parameters");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_ALREADYREGISTRED 462 // /user, /pass
//:You may not reregister

int ALREADYREGISTERED(user &usr)
{
	message msg("irc.local", "462");
	msg.add_params(usr.nickname());
	msg.add_params("You may not reregister");
	usr.send_a_message(msg);
	return (0);
}

//ERR_BADCHANMASK
//:irc.local 476 <nick> <channel> :Bad Channel Mask

# define ERR_BADCHANMASK 476

int BADCHANMASK(user &usr, std::string channel_name) 
{
	message msg("irc.local", "476");
	msg.add_params(usr.nickname());
	msg.add_params(channel_name);
	msg.add_params("Invalid channel name");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NOSUCHCHANNEL 403
//<channel_name> :No such channel

int NOSUCHCHANNEL(user &usr, std::string chanel_name)
{
	message msg("irc.local", "403");
	msg.add_params(usr.nickname());
	msg.add_params(chanel_name);
	msg.add_params("No such channel");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_CHANOPRIVSNEEDED 482 //in case of /kick
//<channel> :You're not channel operator

int CHANOPRIVSNEEDED(user &usr, std::string chanel)
{
	message msg("irc.local", "482");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("You're not channel operator");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NOTONCHANNEL 442
//<channel> :You're not on that channel

int NOTONCHANNEL(user &usr, std::string chanel)
{
	message msg("irc.local", "442");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("You're not on that channel");
	usr.send_a_message(msg);
	return (0);
}

//     443    ERR_USERONCHANNEL
//             "<user> <channel> :is already on channel"
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

# define ERR_CHANNELISFULL 471
//<channel> :Cannot join channel (+l)

int CHANNELISFULL(user &usr, std::string &chanel)
{
	message msg("irc.local", "471");
	msg.add_params(usr.nickname());
	msg.add_params(chanel);
	msg.add_params("Cannot join channel (+l)");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_TOOMANYCHANNELS 405
//<channel name> :You have joined too many channels"
/* Sent to a user when they have joined the maximum number of allowed channels
 * and they try to join another channel */

int TOOMANYCHANNELS(user &usr, std::string &chanel_name)
{
	message msg("irc.local", "405");
	msg.add_params(usr.nickname());
	msg.add_params(chanel_name);
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NORECIPIENT 411
//:No recipient given (<command>)


int NORECIPIENT(user &usr, std::string commands)
{
	message msg("irc.local", "411");
	msg.add_params(usr.nickname());
	msg.add_params(commands);
	msg.add_params("No recipient given");
	usr.send_a_message(msg);
	return (0);
}


# define ERR_NOTEXTTOSEND 412 // /PRIVMSG
int NOTEXTTOSEND(user &usr)
{
	message msg("irc.local", "412");
	msg.add_params(usr.nickname());
	msg.add_params("No text to send");
	usr.send_a_message(msg);
	return (0);
}
//:No text to send
# define ERR_NOSUCHNICK 401 // /PRIVMSG
//<nickname> :No such nick/channel

int NOSUCHNICK(user &usr, std::string nickname)
{
	message msg("irc.local", "401");
	msg.add_params(usr.nickname());
	msg.add_params(nickname);
	msg.add_params("No such nick");
	usr.send_a_message(msg);
	return (0);
}
# define ERR_UNKNOWNMODE 472
//<char> :is unknown mode char to me
int UNKNOWNMODE(user &usr, std::string &car)
{
	message msg("irc.local", "472");
	msg.add_params(usr.nickname());
	msg.add_params(car);
	msg.add_params("is unknown mode char to me");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_UMODEUNKNOWNFLAG 501
//:Unknown MODE flag
/* Returned by the server to indicate that a MODE
 * message was sent with a nickname parameter and that
 * the a mode flag sent was not recognized. */

int UMODEUNKNOWNFLAG(user &usr)
{
	message msg("irc.local", "501");
	msg.add_params(usr.nickname());
	msg.add_params("Unknown MODE flag");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_NOORIGIN 409
//:No origin specified
/* PING or PONG message missing the originator parameter
 * which is required since these commands must work
 * without valid prefixes.*/

int NOORIGIN(user &usr)
{
	message msg("irc.local", "409");
	msg.add_params(usr.nickname());
	msg.add_params("No origin specified");
	usr.send_a_message(msg);
	return (0);
}

# define ERR_CANNOTSENDTOCHAN 404
//<channel name> :Cannot send to channel
/* Sent to a user who is either (a) not on a channel
 * which is mode +n or (b) not a chanop (or mode +v) on
 * a channel which has mode +m set and is trying to send
 * a PRIVMSG message to that channel. */

int CANNOTSENDTOCHAN(user &usr, std::string &channel_name)
{
	message msg("irc.local", "472");
	msg.add_params(usr.nickname());
	msg.add_params(channel_name);
	msg.add_params("Cannot send to channel");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_WHOISUSER 311
//<nick> <user> <host> * :<real name>

int WHOISUSER(user &usr, std::string nick, std::string user, std::string host, std::string real_name)
{
	message msg("irc.local", "311");
	msg.add_params(nick);	msg.add_params(user);	msg.add_params(host);
	msg.add_params("*");
	msg.add_params(real_name);
	usr.send_a_message(msg);
	return (0);
}
# define RPL_NOTOPIC 331 // after a /join, don't expect a topic, our server won't handle them
//<channel> :No topic is set

int NOTOPIC(user &usr, std::string &channel)
{
	message msg("irc.local", "331");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("No topic is set");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_NAMEREPLY 353 
int NAMEREPLY(user &usr, std::string &channel)
{
	message msg("irc.local", "353");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("[[@|+]<nick> [[@|+]<nick> [...]]]");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_ENDOFNAMES 366
//<channel> :End of /NAMES list

int ENDOFNAMES(user &usr, std::string &channel)
{
	message msg("irc.local", "366");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("End of /NAMES list");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_CHANNELMODEIS 324 // if we use it, it's with default params
//<channel> <mode> <mode params>

int CHANNELMODEIS(user &usr, std::string channel, std::string mode, std::string mode_params)
{
	message msg("irc.local", "324");
	msg.add_params(usr.nickname());
	msg.add_params(channel);	msg.add_params(mode);	msg.add_params(mode_params);
	usr.add_to_buffer(msg);
//	message msg2("irc.local", "329");
//	msg2.add_params(channel);	msg2.add_params(mode);	msg2.add_params("1658587792");
//	usr.add_to_buffer(msg2);
	usr.send_buffer();
//	usr.send_a_message(msg);
	return (0);
}

# define RPL_WHOREPLY 352 // what does H@ mean ???
//<channel> <user> <host> <server> <nick> H@ :0 <real name>

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
 //   usr.send_a_message(msg);
}


# define RPL_ENDOFWHO 315
//<name> :End of /WHO list

message ENDOFWHO(user &usr, std::string name)
{
	(void)usr;
	message msg("irc.local", "315");
	msg.add_params(usr.nickname());
	msg.add_params(name);
	msg.add_params("End of /WHO list");
	return (msg);
}

# define RPL_ENDOFBANLIST 368 // we don't have any ban list for now
//<channel> :End of channel ban list

int ENDOFBANLIST(user &usr, std::string &channel)
{
	message msg("irc.local", "368");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("End of channel ban list");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_LUSERCLIENT 251
//:There are <integer> users and <integer> invisible on <integer> servers

int LUSERCLIENT(user &usr, std::string int1, std::string int2, std::string int3)
{
	message msg("irc.local", "251");
	msg.add_params(usr.nickname());
	msg.add_params("There are " + int1 + " users and " + int2 + " invisible on " + int3 + "servers");
	usr.send_a_message(msg);
	return (0);
}
# define RPL_LUSERUNKNOWN 253
//<integer> :unknown connection(s)

int LUSERUNKNOWN(user &usr, std::string &integer)
{
	message msg("irc.local", "253");
	msg.add_params(usr.nickname());
	msg.add_params(integer);
	msg.add_params("unknown connection(s)");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_LUSERCHANNELS 254
//<integer> :channels formed

int LUSERCHANNELS(user &usr, std::string &integer)
{
	message msg("irc.local", "254");
	msg.add_params(usr.nickname());
	msg.add_params(integer);
	msg.add_params("channels formed");
	usr.send_a_message(msg);
	return (0);
}

# define RPL_LUSERME 255
//:I have <integer> clients and <integer> servers
int LUSERME(user &usr, std::string int1, std::string int2)
{
	message msg("irc.local", "255");
	msg.add_params(usr.nickname());
	msg.add_params("I have " + int1 + " clients and " + int2 + " servers");
	usr.send_a_message(msg);
	return (0);
}
# define RPL_MOTDSTART 375
//:- <server> Message of the day -

int MOTDSTART(user &usr, std::string server)
{
	message msg("irc.local", "375");
	msg.add_params(usr.nickname());
	msg.add_params("- " + server + " Message of the day - ");
	usr.send_a_message(msg);
	return (0);
}
# define RPL_MOTD 372
//:- <text>

int MOTD(user &usr, std::string &text)
{
	message msg("irc.local", "372");
	msg.add_params(usr.nickname());
	msg.add_params("- " + text);
	usr.send_a_message(msg);
	return (0);
}

# define RPL_ENDOFMOTD 376
int ENDOFMOTD(user &usr)
{
	message msg("irc.local", "376");
	msg.add_params(usr.nickname());
	msg.add_params("End of /MOTD command");
	usr.send_a_message(msg);
	return (0);
}
//:End of /MOTD command
# define RPL_UMODEIS 221
//<user mode string>
/* To answer a query about a client's own mode, RPL_UMODEIS is sent back */
int UMODEIS(user &usr, std::string &user_mode_string)
{
	message msg("irc.local", "221");
	msg.add_params(usr.nickname());
	msg.add_params(user_mode_string);
	usr.send_a_message(msg);
	return (0);
}

# define ERR_INVITEONLYCHAN 473
int	INVITEONLYCHANNEL(user& usr, std::string channel)
{
	message	msg("irc.local", "473");
	msg.add_params(usr.nickname());
	msg.add_params(channel);
	msg.add_params("Cannot join channel (+i)");
	usr.send_a_message(msg);
	return 0;
}

#define ERR_USERSDONTMATCH 502
int	USERSDONTMATCH(user& usr)
{
	message	msg("irc.local", "502");
	msg.add_params(usr.nickname());
	msg.add_params("Cannot change mode for other users");
	usr.send_a_message(msg);
	return 0;
}
