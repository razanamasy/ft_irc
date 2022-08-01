#include "server.hpp"

std::ostream&	operator<<(std::ostream& o, channel& chan)
{
	o << "channel " << chan.name();
	if (chan.is_invite_only())
		o << " (invite only)";
	o << ": " << chan.number_of_users() << "/" << MAX_USER_PER_CHANNEL << std::endl << "\t";
	{
		std::list<user*>::iterator	ub = chan.users().begin();
		std::list<user*>::iterator	ue = chan.users().end();
		while (ub != ue)
		{
			o << (*ub)->nickname();
			ub++;
			if (ub != ue)
				o << ", ";
		}
		o << std::endl;
	}
	o << "Invite list: ";
	{
		std::list<user*>::iterator	ub = chan.invite_list().begin();
		std::list<user*>::iterator	ue = chan.invite_list().end();
		while (ub != ue)
		{
			o << (*ub)->nickname();
			ub++;
			if (ub != ue)
				o << ", ";
		}
		o << std::endl;
	}
	return o;
}

std::ostream&	operator<<(std::ostream& o, user& usr)
{
	o	<< "user " << usr.getfd() << ": '"
		<< usr.nickname() << "', '"
		<< usr.username() << "', '"
		<< usr.hostname() << "', '"
		<< usr.servername() << "'";
	if (usr.is_passed())
		o << " PASSED";
	o	<< std::endl
		<< "in " << usr.get_in_channels() << "/" << MAX_CHANNEL_PER_USER << " channels:"
		<< std::endl << "\t";
	std::list<channel*>::iterator	cb = usr.get_channels().begin();
	std::list<channel*>::iterator	ce = usr.get_channels().end();
	while (cb != ce)
	{
		o << (*cb)->name() << " ";
		cb++;
	}
	return o;
}

std::ostream&	operator<<(std::ostream& o, server& serv)
{
	o	<< "========== DISPLAYING SERVER ===========" << std::endl
		<< "USERS: " << serv.number_of_users() << "/" << MAX_USERS;
	{
		size_t	unregistered = 0;
		size_t	not_passed = 0;
		for (server::user_iterator	ub = serv.Users().begin() ; ub != serv.Users().end(); ub++)
		{
			if (!(*ub).is_passed())
				not_passed++;
			else if ((*ub).username().empty())
				unregistered++;
		}
		o << ", " << not_passed << " not passsed, " << unregistered << " not registered";
	}
	o<< std::endl;
	server::user_iterator	ub = serv.Users().begin();
	server::user_iterator	ue = serv.Users().end();
	while (ub != ue)
	{
		o << (*ub) << std::endl;
		ub++;
	}
	o << "CHANNELS: " << serv.number_of_channels() << "/" << MAX_CHANNEL << std::endl;
	server::chan_iterator	cb = serv.channels().begin();
	server::chan_iterator	ce = serv.channels().end();
	while (cb != ce)
	{
		o << (*cb) << std::endl;
		cb++;
	}
	o << "================= END ==================" << std::endl;
	return o;
}
