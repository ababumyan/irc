#include "ClientManager.hpp"
#include "Server.hpp"
#include "Exceptions.hpp"
#include "MessageController.hpp"

ClientManager	*ClientManager::instance = NULL;


ClientManager	*ClientManager::getManager()
{
	if (!instance)
		new ClientManager();
	return (instance);
}


ClientManager::ClientManager() : messageController(MessageController::getController())
{
	if (!instance)
		instance = this;
	else
	{
		std::cout << "Creating second instance of Server!!!" << std::endl 
			<< "Bad idea, try new tricks!!!" << std::endl;
		delete this;
	}
}

ClientManager::~ClientManager()
{

}

void	ClientManager::AddClient(int socketFd)
{
	std::pair<int, Client> newClient(socketFd, Client(socketFd));
	clientMap.insert(newClient);
	std::cout << "Adding to list of sockets as "
		<< clientMap.size() << std::endl;
}

void	ClientManager::RemoveClient(int socketFd)
{
	std::map<int, Client>::iterator it = clientMap.find(socketFd);

	if (it != clientMap.end())
		clientMap.erase(it);
	else
		std::cout << "No such Client!!" << std::endl;
}

void	ClientManager::RemoveClient(std::map<int, Client>::iterator &iter)
{
	clientMap.erase(iter);
}



int		ClientManager::AddClientstToReadFds(fd_set *readfds)
{
	for (it = clientMap.begin(); it != clientMap.end(); it++)
		FD_SET(it->first, readfds);
	if (clientMap.size() > 0)
		return (clientMap.rbegin()->first);
	return (0);
}

bool	ClientManager::HasClient(int clientSocket) const
{
	for (it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second.getSocket() == clientSocket)
			return (true);
	}
	return (false);
}

bool	ClientManager::HasClient(const std::string &clientNicck) const
{
	for (it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second.getNick() == clientNicck)
			return (true);
	}
	return (false);
}


int	ClientManager::GetClientSocket(const std::string &clientName) const
{
	for (it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second.getNick() == clientName)
			return (it->first);
	}
	return (-1);
}

void	ClientManager::HandleMessage(Client &client)
{
	messageController->AddChunk(client.getSocket(), buffer);
	
	if (!messageController->GotEndOfMessage(buffer))
		return ;
	std::string readyMessage = messageController->ConstructFullMessage(client.getSocket());
	std::vector<CommandData> commands = messageController->Parse(readyMessage);
	messageController->PrintData(commands);

	for(std::vector<CommandData>::iterator data = commands.begin();
		data != commands.end(); data++)
	{
		try
		{
			CommandHandler::getHandler()->ExecuteCommand(client, *data);
		}
		catch(const IRCException& exception)
		{
			Server::getServer()->SendMessageToClient(client,
				exception.what());
		}
	}
	messageController->ClearChunk(client.getSocket());
}

void	ClientManager::CloseClient(int	clientSocket, const std::string &reason)
{
	Server	*server = Server::getServer();
	struct sockaddr_in *address;
	socklen_t	addrlen;

	address = server->GetAddress();
	addrlen = server->getaddrlen();
	//Somebody disconnected , get his details and print
	getpeername(clientSocket , (sockaddr *)address , &addrlen);
	
	std::cout << "Host disconnected, IP: " << inet_ntoa(address->sin_addr)
		<< ", PORT: " << ntohs(address->sin_port) << std::endl
		<< "Reason: " << (reason.length() == 0 ? "not specified." : reason) << std::endl;
	//Close the socket and mark as 0 in list for reuse
	server->ClearClientFromChannels(clientMap[clientSocket]);
	if (clientSocket == server->getBotDescriptor())
		server->RemoveBot();
	close(clientSocket);
}

void	ClientManager::HandleInput(fd_set *readfds)
{
	int sd, valread;


	for (std::map<int, Client>::iterator it = clientMap.begin(); it != clientMap.end();)
	{
		sd = it->first;
		if (FD_ISSET(sd , readfds))
		{
			if ((valread = recv(sd, buffer, 1024, MSG_DONTWAIT)) == 0)
			{
				++it;
				CloseClient(sd, "");
				RemoveClient(sd);
			}
			else // in case if client inputed message
			{
				buffer[valread] = '\0';
				if (buffer[0] && !((buffer[0] == '\n' || buffer[0] == '\r') && !messageController->ContainsChunk(sd)))
					HandleMessage(it->second);

				if (clientMap.size() == 0)
					return ;
				it++;
			}
		}
		else
			++it;
	}
}

const Client &ClientManager::getClient(int clientSocket) const
{
	std::map<int, Client>::const_iterator it;
	for (it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second.getSocket() == clientSocket)
			break;
	}
	return it->second;
}

const Client &ClientManager::getClient(const std::string &clientNick) const
{
	std::map<int, Client>::const_iterator it;
	for (it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second.getNick() == clientNick)
			break;
	}
	return it->second;
}