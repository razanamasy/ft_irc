#ifndef REPLIES_HPP
# define REPLIES_HPP
#include "user.hpp"
#include "message.hpp"

/* replies that COULD be needed to sent back */

int	NOTREGISTERED(user& usr, std::string command);
int NONICKNAMEGIVEN(user &usr);
int ERRONEUSNICKNAME(user &usr, std::string &nick);
int NICKNAMEINUSE(user &usr, std::string &nick);
int NEEDMOREPARAMS(user &usr, std::string command);
int ALREADYREGISTERED(user &usr);
int BADCHANMASK(user &usr, std::string channel_name); 
int NOSUCHCHANNEL(user &usr, std::string chanel_name);
int CHANOPRIVSNEEDED(user &usr, std::string chanel);
int NOTONCHANNEL(user &usr, std::string chanel);
int USERONCHANNEL(user &usr, std::string nick, std::string &chanel);
int CHANNELISFULL(user &usr, std::string &chanel);
int TOOMANYCHANNELS(user &usr, std::string &chanel_name);
int NORECIPIENT(user &usr, std::string commands);
int NOTEXTTOSEND(user &usr);
int NOSUCHNICK(user &usr, std::string nickname);
int USERSDONTMATCH(user& usr);
int UNKNOWNMODE(user &usr, std::string &car);
int UMODEUNKNOWNFLAG(user &usr);
int NOORIGIN(user &usr);
int CANNOTSENDTOCHAN(user &usr, std::string &channel_name);
int WHOISUSER(user &usr, std::string nick, std::string user, std::string host, std::string real_name);
int NOTOPIC(user &usr, std::string &channel);
int CHANNELMODEIS(user &usr, std::string channel, std::string mode, std::string mode_params);
message WHOREPLY(user &usr, std::string channel, std::string user, std::string host, std::string server, std::string nick, std::string real_name, bool is_op);
message ENDOFWHO(user &usr, std::string name);
int ENDOFBANLIST(user &usr, std::string &channel);
int LUSERCLIENT(user &usr, std::string int1, std::string int2, std::string int3);
int LUSERUNKNOWN(user &usr, std::string &integer);
int LUSERCHANNELS(user &usr, std::string &integer);
int LUSERME(user &usr, std::string int1, std::string int2);
int MOTDSTART(user &usr, std::string server);
int MOTD(user &usr, std::string &text);
int ENDOFMOTD(user &usr);
int UMODEIS(user &usr, std::string &user_mode_string);
int	INVITEONLYCHANNEL(user& usr, std::string channel);
int ERR_NOTEXTTOSEND(user &usr);
message NAMEREPLY(std::string nickname, std::string channel, std::string user_lst);
message ENDOFNAMES(std::string nickname, std::string channel);
message RPL_TOPIC(user& usr, std::string chan, std::string top);
message RPL_NOTOPIC(user& usr, std::string chan);
#endif
