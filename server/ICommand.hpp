#if !defined(ICOMMAND_HPP)
#define ICOMMAND_HPP
 
#include <map>
#include <vector>
#include "Client.hpp"

class ICommand
{
	public:
		virtual ~ICommand() {}
		virtual void	execute(Client &sender,const std::vector<std::string> &arguments) = 0;
		virtual void	validate(Client &sender,const std::vector<std::string> &arguments) = 0;
};

#endif // ICOMMAND_HPP
