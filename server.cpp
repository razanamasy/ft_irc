# include "server.hpp"

server::server(int port)
{
	extern fd_set set;
	this->nbr_of_channels = 0;
	this->nbr_of_users = 0;
	this->port = port;
	FD_ZERO(&(set));
	if (this->ini_server(this->port))
		exit(1);
	ft_bzero((char*)&addr_client, sizeof(addr_client));
}

std::string server::getkey() const {return this->key;}
std::string server::gethash() const {return this->hash;}

server::~server(){}

// ACCESSORS

size_t	server::number_of_users()		const { return nbr_of_users; }
size_t	server::number_of_channels()	const { return nbr_of_channels; }

void server::setkey(std::string key) {this->key = key; return ;}
void server::sethash(std::string hash) {this->hash = hash; return ;}

std::list<user>&	server::Users()		{ return _users; }
std::list<channel>&	server::channels()	{ return _channels; }

// CHANS

void	server::add_channel(channel& chan)
{
	_channels.push_back(chan);
	nbr_of_channels++;
}

void	server::remove_channel(server::chan_iterator chan)
{
	_channels.erase(chan);
	nbr_of_channels--;
}

void	server::pop_user_from_chan(user& usr, chan_iterator it_chan)
{
	(*it_chan).remove_user(usr);
	usr.remove_a_chan(*it_chan);
	if ((*it_chan).is_empty())
		remove_channel(it_chan);
}

server::chan_iterator	server::search_channel(std::string name)
{
	server::chan_iterator	b = _channels.begin();
	server::chan_iterator	e = _channels.end();
	while (b != e)
	{
		if ((*b).name() == name)
			break;
		b++;
	}
	return b;
}

bool	server::channel_exists(std::string name)
{ return (search_channel(name) != _channels.end()); }

// USERS

void	server::add_user(const user& usr)
{
	_users.push_front(usr);
	nbr_of_users++;
}

void	server::remove_user(user_iterator ui)
{
	std::list<chan_iterator>	chan_to_destroy;
	chan_iterator				b = _channels.begin();
	chan_iterator				e = _channels.end();
	while(b != e)
	{
		if ((*b).is_invited(&(*ui)))
			(*b).remove_from_invite_list(&(*ui));
		else
		{
			(*b).remove_user(*ui);
			(*ui).remove_a_chan(*b);
			if ((*b).is_empty())
				chan_to_destroy.push_front(b);
		}
		b++;
	}
	_users.erase(ui);
	nbr_of_users--;
	while (!chan_to_destroy.empty())
	{
		remove_channel(chan_to_destroy.front());
		chan_to_destroy.pop_front();
	}
}

server::user_iterator	server::search_user(const std::string& nick)
{
	server::user_iterator	b = _users.begin();
	server::user_iterator	e = _users.end();
	while (b != e)
	{
		if ((*b).nickname() == nick)
			break;
		++b;
	}
	return b;
}

server::user_iterator server::get_user_by_fd(int fd)
{
	for (std::list<user>::iterator it = this->_users.begin() ; it != this->_users.end() ; it++)
	{
		if (it->getfd() == fd)
		{
			return (it);
		}
	}
	return (this->_users.end());
}

bool	server::is_ip_already_here(std::string ip)
{
	if (!nbr_of_users)
		return false;
	server::user_iterator	b = _users.begin();
	server::user_iterator	e = _users.end();
	while (b != e)
	{
		if ((*b).hostname() == ip)
			return true;
		b++;
	}
	return false;
}

bool	server::user_exists(const std::string& nick)
{ return (search_user(nick) != _users.end()); }

bool	server::user_exists(const int sockfd)
{ return (get_user_by_fd(sockfd) != _users.end()); }

// CRYPTO

void	server::generate_key(size_t len)
{
	srand(time(NULL));
	std::string    res;
	char    c;
	size_t    sz = 32;
	while (sz < len)
		sz *= 2;
	for (size_t i = 0; i < sz; i++)
	{
		c = rand() % 128;
		res += c;
	}
	this->key = res;
}

void	server::cipher_password(std::string password)
{
	std::string    res;
	std::string		key = this->key;

	for (size_t i = 0; i < key.length(); i++)
	{
		if (i >= password.length())
    		res += key[i];
		else
    		res += (password[i] + key[i]) % 128;
	}
	this->hash = res;
}

std::string	server::cipher_test_password(std::string password, std::string key)
{
	std::string    res;
	for (size_t i = 0; i < key.length(); i++)
	{
		if (i >= password.length())
			res += key[i];
		else
			res += (password[i] + key[i]) % 128;
	}
	return (res);
}

bool	server::password_match(std::string candidate, std::string hash, std::string key)
{ return (cipher_test_password(candidate, key) == hash); }

void	server::load_local_serv_ip(sockaddr_in & addr, int port)
{
	ft_bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; //adresse locale
	addr.sin_port = htons(port);
}

bool server::uncorrect_param(std::string str)
{
	std::string accepted("(){}[]-_");
	
	for (size_t i = 0 ; i < str.size() ; i++)
	{
		if (		!std::isalpha(str[i])
				&& 	!std::isdigit(str[i])
				&& 	accepted.find(str[i]) == std::string::npos	)
			return (true);
	}
	return (false);
}

int	server::ini_server(int port)
{
	extern int serv_socket;
	extern fd_set set;
	struct sockaddr_in serv_addr;
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket < 0)
	{
		std::cerr << "Error socket creation" << std::endl;
		return(1);
	} 
	load_local_serv_ip(serv_addr, port);

	int val = 1;
	setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

	if (bind(serv_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "Error binding" << std::endl;
		return (1);
	}

	listen(serv_socket, 5);

	FD_SET(serv_socket, &(set));

	return (0);
}

std::string commands[] = {"PASS", "NICK", "USER", "MODE", "PING", "JOIN", "KICK", "WHO", "PRIVMSG", "PART", "QUIT", "NOTICE", "TOPIC", "INVITE"};

int	identify_command(std::string cmd)
{
	for (size_t i = 0 ; i < 14 ; i++)
	{
		if (cmd == commands[i])
			return i;
	}
	return -1;
}

bool	uncorrect_params(user &usr, message & msg, size_t nb)
{
	size_t nbp;

	nbp = msg.getparams().size();

	if (nbp == 1 && msg.getparams().front().size() == 0)
		nbp = 0;

	if (nbp < nb)
	{
		NEEDMOREPARAMS(usr, msg.command());
		return (true);
	}
	return (false);
}


int	server::manage(int fd, message msg)
{
	std::cout	<< "//////// RECU ////////" << std::endl
				<< msg
				<< "//////////////////////" << std::endl;
	server::user_iterator	usr = get_user_by_fd(fd);
	int	command_id = identify_command(msg.command());
	if ((*usr).username().empty() && command_id > 2)
	{
		NOTREGISTERED(*usr, msg.command());
		return 0;
	}
	switch (command_id)
	{
		case 0:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			if (pass_cmd(*usr, msg.params()))
				return 1;
			break;
		case 1:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			nick_cmd(*usr, msg.params());
			break;
		case 2:
			if (uncorrect_params(*usr, msg, 4))
				return 0;
			user_cmd(*usr, msg.params());
			break ;
		case 3:
			mode_cmd(*usr, msg.params());
			break ;
		case 4:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			ping_cmd(*usr);
			break ;
		case 5:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			join_cmd(*usr, msg.params());
			break ;
		case 6:
			if (uncorrect_params(*usr, msg, 2))
				return 0;
			kick_cmd(*usr, msg.params());
			break ;
		case 7:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			who_cmd(*usr, msg.params());
			break ;
		case 8:
			privmsg_cmd(*usr, msg.params(), "PRIVMSG");
			break ;
		case 9:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			part_cmd(*usr, msg.params());
			break ;
		case 10:
			if (uncorrect_params(*usr, msg, 0))
				return 0;
			quit_cmd(*usr, msg.params());
			break ;
		case 11:
			privmsg_cmd(*usr, msg.params(), "NOTICE");
			break ;
		case 12:
			if (uncorrect_params(*usr, msg, 1))
				return 0;
			topic_cmd(*usr, msg.params());
			break ;
		case 13:
			if (uncorrect_params(*usr, msg, 2))
				return 0;
			invite_cmd(*usr, msg.params());
			break ;
	}
	return 0;
}

bool	is_up = true;

void handle_interruption(int sig)
{
	extern int serv_socket;
	extern fd_set set;
	if (sig == 2)
	{
		close(serv_socket);

        std::cout << std::endl << "Server Interrupted" << std::endl;
		is_up = false;
		for (int i = 0; i < 1024; i++)
		{
			if (FD_ISSET(i, &set))
			{
					FD_CLR(i, &set);
					close(i);
			}
		}
		FD_ZERO(&set);
	}
}

void	handle_signal() {
	struct sigaction    sig_struct;

	ft_bzero((char *)&sig_struct.sa_mask, sizeof(sigset_t));
    sig_struct.sa_flags = SA_NODEFER;
    sig_struct.sa_handler = handle_interruption;
    sigaction(SIGINT, &sig_struct, 0);
	return ;
}

int server::new_connection()
{
	int fd_acc;
	extern int serv_socket;
	extern fd_set set;
	socklen_t	slt = sizeof(this->addr_client);

	fd_acc = accept(serv_socket, (struct sockaddr*)&addr_client, &slt);
	if (this->nbr_of_users >= MAX_USERS)
	{
		send(fd_acc, "ERROR :server is full, you may not connect [access denied by configuration]\r\n", 77, 0);
		close(fd_acc);
		return (1);
	}

		std::string ip = inet_ntoa(addr_client.sin_addr);
		FD_SET(fd_acc, &(set));
		this->nbr_of_users++;
		this->_users.push_back(user(fd_acc, ip));
		return (0);
}

int server::problem_on_client(int fdcur)
{
	server::user_iterator	it_del_user = get_user_by_fd(fdcur);
	std::list<std::string>	lst;

	if (it_del_user == this->_users.end())
		return (1);

	lst.push_back("disconnected");
	quit_cmd(*it_del_user, lst);

	return (0);
}

int server::parcing_manage_request(int fdcur)
{
		size_t b = 0;
		size_t c = 0;
		while ((b = fd_buffer[fdcur].find("\n")) != std::string::npos)
		{
			if ((c = fd_buffer[fdcur].find("\r\n")) != std::string::npos)
			{
				message	msg(fd_buffer[fdcur].substr(0, c));
				fd_buffer[fdcur] = fd_buffer[fdcur].substr(c + 2);				
						
				if (manage(fdcur, msg) == 1)
				{
					std::cout << *this;
					return (1);
				}
				std::cout << *this;
			}
			else
			{
				message	msg(fd_buffer[fdcur].substr(0, b));
				fd_buffer[fdcur] = fd_buffer[fdcur].substr(b + 1);
				if (manage(fdcur, msg) == 1)
				{
					std::cout << *this;
					return (1);
				}
				std::cout << *this;
			}
		}
	return (0);
}

int server::request_from_client(int fdcur)
{
	char buf[1024];
	int cur;
	ft_bzero(buf, 1024);
	cur = recv(fdcur, buf, 1024, 0);
	if (cur <= 0)
	{
		problem_on_client(fdcur);
	}
	else
	{
		std::string	messages(buf);
		std::map<int, std::string>::iterator	_e = fd_buffer.end();
		if (fd_buffer.find(fdcur) == _e)
		{
			fd_buffer.insert(make_pair(fdcur, messages));
		}
		else
		{
			if (!messages.empty())
				fd_buffer[fdcur] += messages;
		}
		if (parcing_manage_request(fdcur))
			return (1);
	}
	return (0);
}

int server::listen_all_socks() 
{
	extern int serv_socket;
	extern fd_set set;
	fd_set save;
	const int MAX_FD = sysconf(_SC_OPEN_MAX);

	handle_signal();
	while (is_up)
	{
		save = set;
		select(MAX_FD + 1, &save, NULL, NULL, NULL);
		for (int fdcur = 0 ; (fdcur < MAX_FD && is_up) ; fdcur++)
		{
			if (FD_ISSET(fdcur, &save))
			{
				if (fdcur == serv_socket)
				{
					if (new_connection())
						continue;
				}
				else
				{
					if (request_from_client(fdcur))
						break;
				}
			}
		}
	}
	return (0);
}
