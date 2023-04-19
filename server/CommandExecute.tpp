#include <fstream>
#include "CommandResponse.hpp"
#include "irc.hpp"

template <CommandType::Type type>
void	Command<type>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	(void) sender;
	(void) arguments;
}



template<>
void	Command<CommandType::pass>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	(void) arguments;
	sender.setIsPassed(true);
}

template<>
void	Command<CommandType::user>::execute(Client &sender,const std::vector<std::string> &arguments)
{
	Server *server = Server::getServer();
	sender.setName(arguments[0]);
	sender.setIsUsered(true);
	if(sender.isDone())
		server->SendHelloMessage(sender);
}

template<>
void	Command<CommandType::nick>::execute(Client &sender,const std::vector<std::string> &arguments)
{
	Server *server = Server::getServer();
	sender.setNick(arguments[0]);
	sender.setIsNicked(true);
	if(sender.isDone())
		server->SendHelloMessage(sender);

}

template<>
void	Command<CommandType::ping>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	sender.SendPongMessage(arguments[0]);
}


template<>
void	Command<CommandType::pong>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	sender.SendPongMessage(arguments[0]);
}


template<>
void	Command<CommandType::privmsg>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	MessageController *messageController = MessageController::getController();
	ClientManager *clientManager = ClientManager::getManager();
	Server *server = Server::getServer();
	std::string MessageBody = "";
	for (size_t i = 1; i < arguments.size(); i++)
		MessageBody = MessageBody + arguments[i] + " ";
	
	std::vector<std::string> args = messageController->Split(arguments[0],",");
	for (size_t i = 0; i < args.size(); i++)
	{

		std::string channelName = messageController->GetChannelName(args[i]);
		if (server->HasChannel(channelName))
			server->getChannel(channelName).Broadcast(sender, MessageBody, "PRIVMSG");
		else
		{
			sender.SendMessage(clientManager->getClient(args[i]),
				"PRIVMSG", MessageBody);
			if (arguments[1].find("DCC") != std::string::npos)
			{
				
				std::vector<std::string> params = messageController->Split(arguments[1], " ");
				if(params.size() < 3)
					return ;
				int port = std::stoi(params[4]);
				//int sockfd  = socket(AF_INET , SOCK_STREAM , 0);
				int sockfd = socket(AF_INET, SOCK_STREAM, 0);
				int flags = fcntl(sockfd, F_GETFL, 0);
				fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
				struct sockaddr_in address;
				uint32_t ip_int = params.size() > 2 ? std::stoi(params[3]) : 2130706433;
				struct in_addr addr;
				addr.s_addr = htonl(ip_int);
				std::string ip_str = inet_ntoa(addr);
				address.sin_family = AF_INET;
				address.sin_addr.s_addr = inet_addr("127.0.0.1");
				address.sin_port = htons(port);
			 	if (connect(sockfd,(struct sockaddr *)&address, sizeof(address)) != 0)
				{
					perror("Connect");
				}
				std::string buffer;
				
				
				//std::string response = /* clientManager->getClient(arguments[0]).GetFormattedText() + */ ":DCC GET " + ip_str + " " + std::to_string(port) + " " + params[2] + " " + std::to_string(0) + "\r\n";
				std::string response_2 =  "DCC GET " + ip_str + " " + std::to_string(port) + " " + params[2] + " " + std::to_string(0) + "\r\n";
				// if (send(clientManager->GetClientSocket(arguments[0]), response.c_str(), response.length() + 1, 0) != 0)
				// {
				// 	perror("Send");
				// }
				if (send(sender.getSocket(), response_2.c_str(), response_2.length() + 1, 0) != 0)
				{
					perror("Send");
				}
				// while (recv(sockfd,(void *)buffer.c_str(),1024,0) > 0)
				// {
				// 	std::cout << "recv ===> "<< buffer << std::endl;
				// 	/* code */
				// }
			}
		}
	}
}


template<>
void	Command<CommandType::notice>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		return ;
	MessageController *messageController = MessageController::getController();
	ClientManager *client_managar = ClientManager::getManager();
	Server *server = Server::getServer();
	
	if(arguments.size() <= 1)
		return ;
	std::vector<std::string> args = messageController->Split(arguments[0],",");
	std::string MessageBody = "";
	
	for (size_t i = 1; i < arguments.size(); i++)
		MessageBody = MessageBody + arguments[i] + " ";
	
	for (size_t i = 0; i < args.size(); i++)
	{
		bool isValidChannel = messageController->IsValidChannelName(args[i]);
		std::string channelName = messageController->GetChannelName(args[i]);
		if(isValidChannel && server->HasChannel(channelName))
		{
			Channel channel = server->getChannel(channelName);
			if (channel.HasMember(sender.getSocket()))
				channel.Broadcast(sender, MessageBody, "NOTICE");
		}
		else if(client_managar->HasClient(args[i]))
		{
			sender.SendMessage(client_managar->getClient(args[i]),
				"NOTICE", arguments.back());
		}
		else return;
	}
}


template<>
void	Command<CommandType::join>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	Server *server = Server::getServer();
	MessageController *message = MessageController::getController();
	std::vector<std::string> args = message->Split(arguments[0],",");
	for (size_t i = 0; i < args.size(); i++)
	{
		std::string channelName = message->GetChannelName(args[i]);
		if (server->HasChannel(args[i]))
		{
			Channel &channel = server->getChannel(channelName);
			channel.AddMember(sender.getSocket());
		}
		else
		{
			Channel &channel = server->getChannel(channelName);
			channel.AddMember(sender.getSocket());
			channel.AddMode(ModeType::write_);
			if (arguments.size() > 1)
			{
				channel.SetPassword(arguments[1]);
				channel.AddMode(ModeType::private_);
			}
		}
		Channel &channel = server->getChannel(channelName);
		channel.ChannelJoinResponse(sender);
	}
}

template<>
void	Command<CommandType::part>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	validate(sender,arguments);
	MessageController *message = MessageController::getController();
	std::vector<std::string> args = message->Split(arguments[0],",");
	std::string channelName;
	for (size_t i = 0; i < args.size(); i++)
	{
		Server *server = Server::getServer();
		channelName = message->GetChannelName(args[i]);

		Channel &channel = server->getChannel(channelName);
		Server::getServer()->PartMessage(sender,channelName);
    	channel.LeaveMember(sender.getSocket());
    	if(channel.getMemberCount() == 0)
			server->removeChannel(channelName);
	}
	
}

template<>
void	Command<CommandType::kick>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	validate(sender,arguments);
	Server *server = Server::getServer();
	ClientManager *clientManager = ClientManager::getManager();
	std::string channelName = MessageController::getController()->GetChannelName(arguments[0]);
	Channel &channel = server->getChannel(channelName);
	server->KickMessage(clientManager->getClient(arguments[1]),channelName,sender.getNick());
	int	memberSocket = clientManager->GetClientSocket(arguments[1]);
	channel.KickMember(sender.getSocket(), memberSocket);
}


template<>
void	Command<CommandType::quit>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	ClientManager *manager = ClientManager::getManager();
	int socket = sender.getSocket();
	std::string reason = arguments.size() == 0 ? " no reason " : arguments[0];
	manager->CloseClient(socket, reason);
	manager->RemoveClient(socket);
}

template<>
void	Command<CommandType::mode>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	MessageController	*messageController = MessageController::getController();
	Server	*server =  Server::getServer();
	std::string target = arguments[0];

	if (messageController->IsValidChannelName(target))
	{
		std::string channelName = messageController->GetChannelName(arguments[0]);
		Channel &channel = server->getChannel(channelName);
		if (arguments.size() == 1)
		{
			server->ChannelModeMessage(sender, channelName);
			return ;
		}
		std::string modeString = arguments[1];
		std::string addingModes = messageController->GetModesString(modeString, '+');
		std::string removingModes = messageController->GetModesString(modeString, '-');
		for (int i = 0; addingModes[i]; ++i)
		{
			char mode = addingModes[i];

			if (mode == 'w')
				channel.AddMode(ModeType::write_);
			else if (mode == 'r')
				channel.AddMode(ModeType::read);
			else if (mode == 'i')
				channel.AddMode(ModeType::invite);
			else if (mode == 'k')
			{
				channel.AddMode(ModeType::private_);
				channel.SetPassword(arguments[2]);
			}
			else if(mode == 'o')
			{
				int	modifyingClientSocket = ClientManager::getManager()->GetClientSocket(arguments[2]);
				channel.MakeAdmin(sender.getSocket(), modifyingClientSocket);
			}
		}
		for (int i = 0; removingModes[i]; ++i)
		{
			char mode = removingModes[i];
			if (mode == 'w')
				channel.RemoveMode(ModeType::write_);
			else if (mode == 'r')
				channel.RemoveMode(ModeType::read);
			else if (mode == 'i')
				channel.RemoveMode(ModeType::invite);
			else if (mode == 'k')
			{
				channel.RemoveMode(ModeType::private_);
				channel.SetPassword("");
			}
			else if(mode == 'o')
			{
				int	modifyingClientSocket = ClientManager::getManager()->GetClientSocket(arguments[2]);
				channel.RemoveFromAdmins(sender.getSocket(), modifyingClientSocket);
			}
		}
	}
	else
		server->UserModeMessage(sender);
}

template<>
void	Command<CommandType::who>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	std::string target = arguments[0];
	MessageController *controller = MessageController::getController();
	Server *server = Server::getServer();
	if(controller->IsValidChannelName(target))
	{
		std::string channelName = controller->GetChannelName(arguments[0]);
		server->getChannel(channelName).ChannelWhoResponse(sender);
		return ;
	}
}

template<>
void	Command<CommandType::bot>::execute(Client &sender, const std::vector<std::string> &arguments)
{

	std::string receiver;
	if (arguments.size() == 1)
		receiver = sender.getNick();
	else 
		receiver = arguments[1];
	Server::getServer()->SendMessageToBot(arguments[0] + " " + receiver);
}

template<>
void	Command<CommandType::botme>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	(void) arguments;
	Server::getServer()->SetBotDescriptor(sender.getSocket());
}

#include <fstream>

template<>
void	Command<CommandType::ftp>::execute(Client &sender, const std::vector<std::string> &arguments)
{
	std::string defaultFileName = "Makefile";
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	//validation !!!
	std::ifstream input(arguments.size() >= 1 ? arguments[0] : defaultFileName);
	
    if (!input) 
	{
		Server::getServer()->SendMessageToClient(sender, "Cannot Open the given File");
        return ;
    }

    std::string line;
	std::string mem;
    while (std::getline(input, line)) 
	{
		mem +=line;
    }
	Server::getServer()->SendMessageToClient(sender, mem);
}