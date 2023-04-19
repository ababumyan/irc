#if !defined(CLIENT_HPP)
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include "IMessenger.hpp"

class Client : public IMessenger
{
	private:
		bool isPassed;
		bool isUsered;
		bool isNicked;
		std::string name;
		std::string nick;
		std::string hostname;
		int	fd;

	public:
		Client();
		Client(int _fd);
		~Client();
		
		std::string getName() const ;
		std::string getNick() const ;
		int getSocket() const ;

		bool getIsPassed()const;
		bool getIsUsered()const;
		bool getIsNicked()const;

		void setIsPassed(bool is_passed);
		void setIsUsered(bool is_usered);
		void setIsNicked(bool is_Nicked);
		void setName(std::string const &name);
		void setNick(std::string const &nick);
		bool isDone();


		std::string	GetFormattedText() const;
		void	SendMessage(const Client &reciever, const std::string &commmand,
			const std::string message) const;
		void	SendPongMessage(const std::string &message) const;
		void	SendSelf(const std::string &message) const;


		void	SendMessageFromChannel(const Client &reciever, const std::string &command,
			const std::string &channel, const std::string message) const;
		bool	operator==(const Client &rhs) const;
};

#endif // CLIENT_HPP
