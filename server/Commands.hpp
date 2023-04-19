#if !defined(COMMANDS_HPP)
#define COMMANDS_HPP

#include "ICommand.hpp"
#include <iostream>
#include <string>
#include "Exceptions.hpp"
#include "Server.hpp"
#include "MessageController.hpp"
#include "ClientManager.hpp"




struct CommandType
{
	enum Type
	{
		pass,
		nick,
		user,
		ping,
		pong,
		privmsg,
		notice,
		join,
		part,
		kick,
		quit,
		mode,
		who,
		cap,
		ls,
		bot,
		botme,
		ftp
	};
};

// class ICommand;
template <CommandType::Type type>
class Command : public ICommand
{
	virtual void	execute(Client &sender, const std::vector<std::string> &arguments);

	virtual void	validate(Client &sender,const std::vector<std::string> &arguments);

};

#include "CommandValidate.tpp"
#include "CommandExecute.tpp"

#endif // COMMANDS_HPP


