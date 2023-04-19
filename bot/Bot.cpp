#include "Bot.hpp"
#include <iostream>

Bot::Bot(const std::string &_host, int _port, const std::string &_pass,
	const std::string &_user, const std::string &_nick) :
	port(_port), host(_host), pass(_pass), user(_user), nick(_nick) { }

Bot::~Bot()
{

}

void	Bot::SetUser(const std::string &_user) { user = _user; }

void	Bot::SetNick(const std::string &_nick) { nick = _nick; }

void	Bot::Setup()
{
	addrlen = sizeof(address);
	CreateServer();
	SetOptions();
	BindSocket();
	ConnectToServer();
}

void	Bot::CreateServer()
{

	if( (socketfd = socket(AF_INET , SOCK_STREAM , 0)) < 0) // prev  if( (socketfd = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("Failed to create a socket");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(host.c_str());
	address.sin_port = htons(port);
}

void Bot::SetOptions()
{
	int opt = 1;
	if( setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("Failed to set socket option");
		exit(EXIT_FAILURE);
	}
}

void	Bot::BindSocket()
{
	if (bind(socketfd, (struct sockaddr *)&address, addrlen) < 0)
	{
		perror("Failed to bind sockets");
		exit(EXIT_FAILURE);
	}
}

void	Bot::ConnectToServer()
{
	std::cout << "The bot is running " << port << std::endl;
	std::cout << nick << std::endl;

	if (connect(socketfd, (struct sockaddr *)&address, sizeof(address)) != 0)
	{
		perror("Failed to connect to server");
		exit(EXIT_FAILURE);
	}
	std::string	message = "PASS " + pass + "\nUSER " + user + " a a a\nNICK " + nick + "\nbotme\n";
	if (send(socketfd, message.c_str(), message.length() + 1, 0) < 0) 
	{
		std::cerr << "Vori e" << std::endl;
	}
	std::cout << "Waiting for connections ..." << std::endl;
}

void	Bot::AddToRecvMsg(const std::string &msg)
{
	recvMessage += msg;
}

void	Bot::AcceptSocket()
{
	addrlen = sizeof(address);
	if( (clientfd = accept(socketfd, (struct sockaddr *)&address, &addrlen) ) < 0)
	{
		perror("Failed to accept socket");
		exit(EXIT_FAILURE);
	}
    fcntl(socketfd, F_SETFL, O_NONBLOCK);
}

void	Bot::ReceiveMsg()
{
	ssize_t valRead;
	while ((valRead = recv(socketfd, buffer, 1024, 0)) > 0)
	{
		buffer[valRead] = '\0';
		AddToRecvMsg(std::string(buffer));
		if (valRead < 1024)
			SendReply();
	}

	if (valRead == -1)
	{
		perror("Failed to receive message");
		exit(EXIT_FAILURE);
	}
	if (valRead == 0)
	{
		std::cout <<  "Server has been closed!" << std::endl;
		close(socketfd);
		exit(EXIT_SUCCESS);
	}
}

std::string	Bot::GiveResponse(const std::string &command)
{
	std::string magic = "																																	\
	 ### ### #     #    #     #####  ###  #####  ### ###																							\
	 ### ### ##   ##   # #   #     #  #  #     # ### ###																							\
	 ### ### # # # #  #   #  #        #  #       ### ###																							\
	  #   #  #  #  # #     # #  ####  #  #        #   # 																							\
	         #     # ####### #     #  #  #             																								\
	 ### ### #     # #     # #     #  #  #     # ### ###																							\
	 ### ### #     # #     #  #####  ###  #####  ### ###																							\
	";
	if (command == "help")
		return ("You can use the following commands:																								\
			  \ttime																																\
			  \tdate																																\
			  \tmagic																																\
			");
	else if (command == "time")
		return __TIME__;
	else if (command  == "date")
		return __DATE__;
	else if (command == "magic")
		return (magic);
	else
		return ("I am not sure what you mean.																												\
			  Use help command to find out what you can do with me.");
}

void	Bot::SendReply()
{

	int	spacePos = recvMessage.find_first_of(' ');
	std::string command = recvMessage.substr(0, spacePos);

	std::string user = recvMessage.substr(spacePos + 1, recvMessage.length());
	std::string replyMessage = "PRIVMSG " + user + " :" + GiveResponse(command) + "\r\n";
	if ((send(socketfd, replyMessage.c_str(), replyMessage.length() + 1, 0)) < 0)
	{
		perror("Failed to send message");
		exit(EXIT_FAILURE);
	}
	recvMessage = "";
}


void	Bot::RunBot()
{
	CreateServer();
	ConnectToServer();
	int valRead;
	// ignoring welcome message
	if ((valRead = recv(socketfd, buffer, 1024, 0)) > 0)
		buffer[valRead] = '\0';
	ReceiveMsg();
}