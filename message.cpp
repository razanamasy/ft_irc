# include "message.hpp"

// CTOR DTOR

message::message(){}

message::message(std::string raw)
{
	std::string	tmp;
	size_t		pos = raw.find(' ');
	if (pos == std::string::npos)
		pos = raw.length();
	tmp = raw.substr(raw[0] == ':' ? 1 : 0, raw[0] == ':' ? pos - 1 : pos);
	if (raw[0] == ':')
	{
		_prefix = tmp;
		pos++;
		size_t	pos1 = raw.find(' ', pos);
		if (pos1 == std::string::npos)
			pos1 = raw.length();
		tmp = raw.substr(pos, pos1 - pos);
		pos = pos1;
	}
	_command = tmp;
	if (pos >= raw.length())
		return;
	size_t	last_pos;
	while (pos < raw.length())
	{
		pos++;
		last_pos = pos;
		if (raw[pos] == ':')
		{
			last_pos++;
			pos = raw.length();
		}
		else
		{
			pos = raw.find(' ', pos);
			if (pos == std::string::npos)
				pos = raw.length();
		}
		_params.push_back(raw.substr(last_pos, pos - last_pos));
	}
}

message::message(std::string prfx, std::string cmd)
{
	_prefix = prfx;
	_command = cmd;
}

message::~message(){}

// ACCESS

const std::list<std::string> &	message::getparams() const {
		return this->_params;
}

const std::list<std::string>&	message::params() const	{ 
		return _params; 
}

const std::string&		message::prefix() const	{ 
		return _prefix; 
}

const std::string&		message::command() const { 
		return _command; 
}

size_t					message::number_of_params() const { 
		return _params.size(); 
}

// MODIFY

void	message::prefix(const std::string pfx) { 
		_prefix = pfx; 
}

void	message::command(const std::string cmd) { 
		_command = cmd; 
}

void	message::params(std::list<std::string> prm) { 
		_params = prm; 
}

void	message::add_params(std::string p) { 
		_params.push_back(p); 
}
		
std::string		message::raw() const
{
	std::string	res;
	if (_prefix.length() != 0)
		res += ":" + _prefix + " ";
	res += _command;
	if (_params.empty())
		return	res;
	res += " ";
	std::list<std::string>::const_iterator	it = _params.begin();
	std::list<std::string>::const_iterator	e = _params.end();
	while (it != e)
	{
		if ((*it).find(' ') != std::string::npos)
			res += ":";
		res += *it;
		it++;
		if (it != e)
			res += " ";
	}
	res += "\r\n";
	return res;
}

std::ostream&	operator<<(std::ostream& o, message& m)
{
	o	<< "PFX: '" << m.prefix() << "'" << std::endl
		<< "CMD: '" << m.command() << "'" << std::endl;
	const std::list<std::string>&	param = m.params();
	std::list<std::string>::const_iterator	it = param.begin();
	std::list<std::string>::const_iterator	e = param.end();
	while (it != e)
	{
		o << *it << " ; ";
		it++;
	}
	o << std::endl;
	return o;
}

