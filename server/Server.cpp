#include "Server.hpp"
#include "MessageController.hpp"

Server *Server::instance = NULL;

Server::Server()
{

}

Server::Server(int _port, std::string _password) 
	: port(_port), password(_password), bot_fd(0)
{
	if (!instance)
		instance = this;
	else
	{
		std::cout << "Creating second instance of Server!!!" << std::endl 
			<< "Bad idea, try new tricks!!!" << std::endl;
		this->~Server();
	}
}

Server::~Server()
{

}

void	Server::Setup()
{
	addrlen = sizeof(address);
	CreateServer();
	SetOptions();
	BindSocket();
	StartListening();
}

void	Server::ResetSockets()
{
	FD_ZERO(&readfds);
	FD_SET(master_socket, &readfds);
	
	int max_fd_in_clients = ClientManager::getManager()->AddClientstToReadFds(&readfds);
	max_sd = std::max(master_socket, max_fd_in_clients);
}

void	Server::CreateServer()
{
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
}

void Server::SetOptions()
{
	int opt = 1;
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
}

void	Server::BindSocket()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(master_socket, (struct sockaddr *)&address, addrlen)<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

void	Server::StartListening()
{
	std::cout << "Listener on port " << port << std::endl;
	// specifying maximum of 3 pending connections for the master socket
	if (listen(master_socket, 100) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	fcntl(this->master_socket,F_SETFL,O_NONBLOCK);
	std::cout << "Waiting for connections ..." << std::endl;
}

void	Server::ListenForClientInput()
{
	ClientManager::getManager()->HandleInput(&readfds);
}

void	Server::SendToClient(int sockfd, const char *message) const
{
	if (send(sockfd, message, strlen(message), 0) < 0)
		perror("send");
}

void	Server::SendMessageToBot(const std::string &message) const
{
	if (send(bot_fd, message.c_str(), message.length() + 1, 0) < 0)
		perror("send");
}

void	Server::WaitForActivity()
{
	int activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);
	
	if ((activity < 0) && (errno!=EINTR))
		std::cout << "select error" << std::endl;
}

int	Server::AcceptNewSocket()
{
	int new_socket;
	if ((new_socket = accept(master_socket,
				(struct sockaddr *)&address, &addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	return (new_socket);
}

void	Server::HandleIncomingConnections()
{
	int new_socket;
	if (FD_ISSET(master_socket, &readfds))
	{
		new_socket = AcceptNewSocket();
		//inform user of socket number - used in send and receive commands
		std::cout << "New connection , socket fd is " << new_socket
			<< ", ip is : " << getHost() << ", port : "
			<< ntohs(address.sin_port) << std::endl;
		
		ClientManager::getManager()->AddClient(new_socket);
	}
}

void	Server::ClearClientFromChannels(const Client &client)
{
	int socket = client.getSocket();
	for(std::map<std::string, Channel>::iterator it = channels.begin();
		it != channels.end(); it++)
	{
		it->second.LeaveIfMember(socket);
	}
}

Server	*Server::getServer()
{
	if (!instance)
		instance = new Server();
	return (instance);
}

int	Server::getaddrlen() { return (addrlen); }

struct	sockaddr_in	*Server::GetAddress()
{
	return (&address);
}

std::string const &Server::getPass()const
{
	return this->password;
}


bool Server::HasChannel(std::string const &name)
{
	std::map<std::string,Channel >::iterator it = channels.find(name);
	if(it != channels.end())
		return true;
	return false;
}

Channel &Server::getChannel(std::string const &name)
{
	if(!HasChannel(name))
		channels.insert(std::pair<std::string, Channel>(name, Channel(name)));
	return channels[name];
}

std::string	const Server::getHost() const
{
	return (inet_ntoa(address.sin_addr));
}

void Server::removeChannel(std::string const &name)
{
	std::cout << "Removing Channel" << std::endl << std::endl;
	channels.erase(name);
}

void	Server::SendHelloMessage(const Client &client) const
{
	std::string mess = client.GetFormattedText() + " 001 " + client.getNick() + " :Welcome to irc server";
	SendMessageWithSocket(client.getSocket(), mess);
}

int		Server::getBotDescriptor() const 
{
	return this->bot_fd;
}


void	Server::SetBotDescriptor(int new_fd)
{
	this->bot_fd = new_fd;
}

void	Server::RemoveBot()
{
	this->bot_fd = 0;
}

bool	Server::IsBotConnected() const
{
	return (bot_fd != 0);
}

bool	Server::IsBot(const Client &client) const
{
	return (bot_fd == client.getSocket());
}
