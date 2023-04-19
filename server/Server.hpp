#if !defined(SERVER_HPP)
#define SERVER_HPP

#include "ClientManager.hpp"
#include "Channel.hpp"
#include "CommandResponse.hpp"

class Server : public CommandResponse
{
	private: // for singleton
		static Server *instance;
	
	private:
		int	master_socket;
		int max_sd;
		int	port;
		std::string password;
		int	bot_fd;
		std::map<std::string, Channel> channels;
		struct sockaddr_in address;
		socklen_t	addrlen;
		fd_set readfds;

	public:
		Server(int port, std::string password);
		~Server();

		static Server	*getServer();
		void	Setup();
		void	ResetSockets();
		void	CreateServer();
		void	SetOptions();
		void	BindSocket();
		void	StartListening();
		int		AcceptNewSocket();
		void	ListenForClientInput();
		void	SendToClient(int sockfd, const char *message) const;
		void	SendMessageToBot(const std::string &message) const;
		void	WaitForActivity();
		void	HandleIncomingConnections();
		void	ClearClientFromChannels(const Client &client);
		int		getaddrlen();
		struct	sockaddr_in	*GetAddress();
		std::string const &getPass()const;
		std::string	const getHost() const;
		void	SendHelloMessage(const Client &client) const;

	public:
		int		getBotDescriptor() const;
		void	SetBotDescriptor(int new_fd);
		void	RemoveBot();
		bool	IsBotConnected() const;
		bool	IsBot(const Client &client) const;

		bool HasChannel(std::string const &name);
		//const Channel &getChannel(std::string const &name)const;
		Channel &getChannel(std::string const &name);
		//void AddChannel(std::string const &name);
		void removeChannel(std::string const &name);
		void ChangeNick(Client const &client);

	private:
		Server();
};

/* void Server::ChangeNick(Client const &client)
{
	std::map<std::string,Channel>::iterator it = channels.begin();
	for (; it !=  channels.end() ; ++it)
	{
		Channel &chan  = this->getChannel(it->first);

	}
	 
} */
//void Server::AddChannel(std::string const &name);//der chem grel!!

#endif // SERVER_HPP



