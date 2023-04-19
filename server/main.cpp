#include "Server.hpp"

void	ValidateInput(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong number of arguments!!" << std::endl
			<< "Try like: ./irc <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
	int port = atoi(argv[1]);
	if (port < 0 || port > 65535)
	{
		std::cout << "Wrong PORT specified!!! Please input a valid PORT" << std::endl
		<< "It should be a number between 0 and 65535" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	ValidateInput(argc, argv);

	Server	server(atoi(argv[1]), argv[2]);

	server.Setup();

	while(true)
	{
		server.ResetSockets();
		server.WaitForActivity();
		server.HandleIncomingConnections();
		server.ListenForClientInput();
	}
	return (0);
}



