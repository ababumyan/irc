#if !defined(BOT_HPP)
#define BOT_HPP

#include "../server/irc.hpp"

class Bot
{
    private:
		socklen_t	addrlen;
		struct sockaddr_in address;
		int				port;
		int				socketfd;
		int				clientfd;
		std::string		host;
		std::string		pass;
		std::string 	user;
		std::string 	nick;
		char			buffer[1025];
		std::string		recvMessage;

		void	Setup();
		void	CreateServer();
		void	SetOptions();
		void	BindSocket();
		void	ConnectToServer();
		void	ReceiveMsg();
		void	AcceptSocket();
		void	SendReply();
		std::string	GiveResponse(const std::string &command);


	public:
		int socketCLIENT;
		void	RunBot();
		Bot(const std::string &host, int _port, const std::string &_pass,
			const std::string &_user = "havayi", const std::string &_nick = "butul");
        ~Bot();
		
		void	SetUser(const std::string &_user);
		void	SetNick(const std::string &_nick);

		void		AddToRecvMsg(const std::string &msg);
		std::string	GetRecvMsg(void) const;
};	


#endif // BOT_HPP
