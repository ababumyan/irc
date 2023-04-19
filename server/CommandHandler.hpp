#if !defined(COMMAND_HANDLER_HPP)
#define COMMAND_HANDLER_HPP

#include "ICommand.hpp"
#include "CommandData.hpp"

// class ICommand;

class CommandHandler
{
	private: //for singleton
		static CommandHandler *instance;
	private:
		std::map<std::string, ICommand *> commands;
		std::map<std::string, ICommand *>::iterator it;

		void	InitilizeCommands();
		void	ClearCommands();

	public:
		CommandHandler();
		~CommandHandler();

		void	ExecuteCommand(Client &client, const CommandData &data);
		static	CommandHandler *getHandler();
};

#endif // COMMAND_HANDLER_HPP
