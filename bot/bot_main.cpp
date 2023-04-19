#include "Bot.hpp"


void	ValidateInput(int argc, char **argv)
{
	if (argc < 4)
	{
		std::cerr << "Wrong Usage!!!" << std::endl <<
			"USAGE: ./botserv <host> <port> <pass> <<user>> <<nick>>" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (argc > 6)
	{
		std::cerr << "Too many arguments!!!" << std::endl << 
			"USAGE: ./botserv <host> <port> <pass> <<user>> <<nick>>" << std::endl;
		exit(EXIT_FAILURE);
	}
	int port = atoi(argv[1]);
	if (port < 0 || port > 65535) // 0-1023 system ports. i think its bad idea to use these ports
	{
		std::cerr << "Wrong PORT specified!!! Please input a valid PORT" << std::endl
		<< "It should be a number between 0 and 65535" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	ValidateInput(argc, argv);
	int port = atoi(argv[2]);
	Bot bot(argv[1], port, argv[3]);
	if (argc > 4)
		bot.SetUser(argv[4]);
	if (argc == 6)
		bot.SetNick(argv[5]);
	bot.RunBot();

}