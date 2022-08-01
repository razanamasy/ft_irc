#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include <iostream>
# include <list>

class message
{
	private:

		std::string				_prefix;		// <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
		std::string				_command;		// <letter> { <letter> } | <number> <number> <number>
		std::list<std::string>	_params;		// <SPACE> [ ':' <trailing> | <middle> <params> ]
	
		//<middle>  = <Any *non-empty* sequence of octets not including SPACE or NUL or CR or LF, the first of which may not be ':'>
		//<trailing> = <Any, possibly *empty*, sequence of octets not including NUL or CR or LF>

	public:

		// CTOR DTOR

		message();
		message(std::string raw);
		message(std::string prfx, std::string cmd);

		~message();

		// ACCESS

		const std::list<std::string> & 	getparams() const;
		const std::string&				prefix() const;
		const std::string&				command() const;		
		const std::list<std::string>&	params() const;
		size_t							number_of_params() const;

		// MODIFY

		void	prefix(const std::string pfx);		
		void	command(const std::string cmd);
		void	params(std::list<std::string> prm);
		void	add_params(std::string p);
		
		std::string	raw() const;
};

std::ostream&	operator<<(std::ostream& o, message& m);

#endif
