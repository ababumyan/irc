#if !defined(COMMAND_DATA_HPP)
#define COMMAND_DATA_HPP

#include <iostream>
#include <vector>

struct CommandData
{
	std::string command;
	std::vector<std::string> args;
};

#endif // COMMAND_DATA_HPP
